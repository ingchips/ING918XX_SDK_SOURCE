#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <time.h>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <Ws2tcpip.h>
#elif __linux__
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <time.h>
    #include <signal.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <mqueue.h>
#else
    #error unsupported target OS
#endif

#include <string.h>
#include <pthread.h>

#include "gen_os.h"

#include "brpc.h"
#include "profile.h"

#include "port_gen_os_driver.h"
#include "platform_api.h"

#include "log.h"

#define GEN_OS()      ((struct gen_os_driver *)platform_get_gen_os_driver())

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN

void sleep_ms(uint32_t ms)
{
    Sleep(ms);
}

uint64_t get_sys_ms_time(void)
{
    SYSTEMTIME wtm;
    ULARGE_INTEGER ui;
    FILETIME ft = { 0, 0 };
    GetLocalTime(&wtm);
    SystemTimeToFileTime(&wtm, &ft);
    ui.LowPart = ft.dwLowDateTime;
    ui.HighPart = ft.dwHighDateTime;
    return ui.QuadPart / 10000;  // 100ns
}

void print_timestamp(void)
{
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    printf("[%2d-%02d %02d:%02d:%02d.%03d] ",
           wtm.wMonth,
           wtm.wDay,
           wtm.wHour,
           wtm.wMinute,
           wtm.wSecond,
           wtm.wMilliseconds);
}


#elif __linux__
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <time.h>
    #include <signal.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <mqueue.h>

void sleep_ms(uint32_t ms)
{
    if (ms > 1000 * 100)
        sleep(ms / 1000);
    else
        usleep(ms * 1000);
}

uint64_t get_sys_ms_time(void)
{
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);
    return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void print_timestamp(void)
{
    struct timespec ts = {0,0};
    clock_gettime(CLOCK_REALTIME, &ts);
    time_t tim = ts.tv_sec;
    struct tm tm = {0};
    localtime_r(&tim, &tm);

    printf("[%2d-%02d %02d:%02d:%02d.%03d] ",
           tm.tm_mon + 1,
           tm.tm_mday,
           tm.tm_hour,
           tm.tm_min,
           tm.tm_sec,
           (int)(ts.tv_nsec / 1000000));
}

#else
    #error unsupported target OS
#endif


static struct block first_block = {0};
static struct block *last_block = &first_block;

void comm_append_block(int len, const void *data)
{
    struct block *block = (struct block *)malloc(sizeof(struct block) + len);
    block->len = len;
    block->next = NULL;
    memcpy(block->buf, data, len);
    GEN_OS()->enter_critical();
    last_block->next = block;
    last_block = block;
    GEN_OS()->leave_critical();
}

struct block *comm_pop_block(void)
{
    struct block *r = NULL;
    GEN_OS()->enter_critical();

    r = first_block.next;
    if (r)
    {
        first_block.next = r->next;
        if (first_block.next == NULL)
            last_block = &first_block;
    }

exit:
    GEN_OS()->leave_critical();
    return r;
}

static int send_data(int sockfd)
{
    struct block *r;
    while ((r = comm_pop_block()) != NULL)
    {
        void *p = r->buf;
        int len = r->len;
        while (len > 0)
        {
            ssize_t c = send(sockfd, p, len, 0);
            if (c < 0)
            {
                free(r);
                return -1;
            }
            if (c == 0)
            {
                sleep_ms(2);
                continue;
            }
            len -= c;
            p += c;
        }
        free(r);
    }
    return 0;
}

static void socket_loop(void *param)
{
    int client_sockfd = (int)param;

    char buf[BUFSIZ];

    while (1)
    {
        if (send_data(client_sockfd) < 0)
            break;

#ifdef _WIN32
        int len = recv(client_sockfd, buf, BUFSIZ, 0);
        if (len == -1)
        {
            int lost = 0;
            switch (WSAGetLastError())
            {
            case WSAENETDOWN:
            case WSAENOTCONN:
            case WSAENETRESET:
            case WSAESHUTDOWN:
            case WSAECONNABORTED:
            case WSAETIMEDOUT:
            case WSAECONNRESET:
                lost = 1;
                break;
            default:
                break;
            }
        }
#else
        int len = recv(client_sockfd, buf, BUFSIZ, MSG_DONTWAIT);
        if (len == -1)
        {
            if ((errno == ENOTCONN) || (errno == ENOTSOCK))
            {
                break;
            }
        }
#endif
        if (len == 0)
        {
            sleep_ms(5);
        }

        int i;
        for (i = 0; i < len; i++)
            brpc_rx_byte(NULL, buf[i]);
    }

    LOG_E("connection lost");

#ifdef _WIN32
    closesocket(client_sockfd);
    WSACleanup();
#else
    close(client_sockfd);
#endif
}

int main(int argc, char *argv[])
{
    int client_sockfd;
    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    remote_addr.sin_port = htons(8888);

    if (sizeof(int) != 4)
    {
        LOG_E("32bit mode is required!!!");
        return -1;
    }

    int i;
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-addr") == 0)
        {
            i++;
            if (i < argc)
                remote_addr.sin_addr.s_addr = inet_addr(argv[i]);
        }
        else if (strcmp(argv[i], "-port") == 0)
        {
            i++;
            if (i < argc)
                remote_addr.sin_port = htons(atoi(argv[i]));
        }
    }

#ifdef __linux__
    do {
        FILE *f = fopen("/proc/sys/fs/mqueue/msg_max", "r");
        if (f == NULL)
        {
            LOG_W("cann't open /proc/sys/fs/mqueue/msg_max");
            break;
        }
        char buf[101] = {0};
        fgets(buf, 100, f);
        if (strcmp(buf, "unlimited") == 0) break;
        if (atoi(buf) < 100)
        {
            LOG_E("fs.mqueue.msg_max is too small. Use below command to modify it:");
            LOG_PLAIN("sudo sysctl fs.mqueue.msg_max=1000");
            return -2;
        }
        fclose(f);
    } while (0);
#endif

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != NO_ERROR)
    {
        LOG_E("WSAStartup failed");
        return 1;
    }
#endif

    gen_os_init();

    if ((client_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
        return 1;
    }

    if (connect(client_sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) < 0)
    {
        LOG_E("connect failed");
        return 1;
    }

    LOG_PROG("connected to server");

    brpc_init();

#ifdef _WIN32
    u_long iMode = 1;
    ioctlsocket(client_sockfd, FIONBIO, &iMode);
#endif

    GEN_OS()->task_create("socket", socket_loop, (void *)client_sockfd, 10240, GEN_TASK_PRIORITY_LOW);

    GEN_OS()->os_start();

    // `os_start` of native is empty
    while (1)
        sleep_ms(500);

    return 0;
}

void platform_raise_assertion(const char *file_name, int line_no)
{
    LOG_E("ASSERT: %s @ %d", file_name, line_no);
}

struct tagSCB __dummy = {0};
