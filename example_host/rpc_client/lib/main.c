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

#include "brpc.h"
#include "profile.h"

#include "port_gen_os_driver.h"
#include "platform_api.h"

#include "log.h"

static struct block first_block = {0};
static struct block *last_block = &first_block;

static pthread_mutex_t mutex;

#ifdef _WIN32
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

static HANDLE hTimerQueue = NULL;
#endif

#ifdef __linux__
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
    struct tm * tm = localtime(&tim);

    printf("[%2d-%02d %02d:%02d:%02d.%03d] ",
           tm->tm_mon + 1,
           tm->tm_mday,
           tm->tm_hour,
           tm->tm_min,
           tm->tm_sec,
           (int)(ts.tv_nsec / 1000000));
}

#endif

static void enter_critical(void)
{
    pthread_mutex_lock(&mutex);
}

static void leave_critical(void)
{
    pthread_mutex_unlock(&mutex);
}

void comm_append_block(int len, const void *data)
{
    struct block *block = (struct block *)malloc(sizeof(struct block) + len);
    block->len = len;
    block->next = NULL;
    memcpy(block->buf, data, len);
    enter_critical();
    last_block->next = block;
    last_block = block;
    leave_critical();
}

struct block *comm_pop_block(void)
{
    struct block *r = NULL;
    enter_critical();

    r = first_block.next;
    if (r)
    {
        first_block.next = r->next;
        if (first_block.next == NULL)
            last_block = &first_block;
    }

exit:
    leave_critical();
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

    pthread_mutex_init(&mutex, NULL);

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


#ifdef __linux__
    socket_loop((void *)client_sockfd);
#endif

#ifdef _WIN32
    u_long iMode = 1;
    ioctlsocket(client_sockfd, FIONBIO, &iMode);

    socket_loop((void *)client_sockfd);
    //port_task_create("socket", socket_loop, (void *)client_sockfd, 10240, GEN_TASK_PRIORITY_LOW);
#endif

    return 0;
}

void platform_raise_assertion(const char *file_name, int line_no)
{
    LOG_E("ASSERT: %s @ %d", file_name, line_no);
}

#ifdef _WIN32

struct timer_obj
{
    HANDLE hTimer;
    uint8_t paused;
    uint32_t timeout_in_ms;
    void (* timer_cb)(void *);
    void *user_data;
};

static VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
    struct timer_obj *obj = (struct timer_obj *)lpParam;
    if (obj->paused) return;
    obj->timer_cb(obj->user_data);
}

gen_handle_t port_timer_create(
        uint32_t timeout_in_ms,
        void *user_data,
        void (* timer_cb)(void *)
    )
{
    if (hTimerQueue == NULL)
        hTimerQueue = CreateTimerQueue();
    if (hTimerQueue == NULL)
        LOG_E("failed to create timer queue");
    struct timer_obj *obj = (struct timer_obj *)malloc(sizeof(struct timer_obj));
    obj->timer_cb = timer_cb;
    obj->user_data = user_data;
    obj->timeout_in_ms = timeout_in_ms;
    obj->paused = 1;

    if (!CreateTimerQueueTimer(&obj->hTimer, hTimerQueue,
            (WAITORTIMERCALLBACK)TimerRoutine, obj, 0, 0, 0))
    {
        LOG_E("CreateTimerQueueTimer failed (%d)\n", GetLastError());
        return NULL;
    }

    return (gen_handle_t)obj;
}

void port_timer_start(gen_handle_t timer)
{
    struct timer_obj *obj = (struct timer_obj *)timer;
    obj->paused = 0;
    if (!ChangeTimerQueueTimer(hTimerQueue, obj->hTimer, obj->timeout_in_ms, 0))
        LOG_E("ChangeTimerQueueTimer failed (%d)\n", GetLastError());
}

void port_timer_stop(gen_handle_t timer)
{
    struct timer_obj *obj = (struct timer_obj *)timer;
    obj->paused = 1;
    if (!ChangeTimerQueueTimer(hTimerQueue, obj->hTimer, 0, 0))
        LOG_E("ChangeTimerQueueTimer failed (%d)\n", GetLastError());
}

void port_timer_delete(gen_handle_t timer)
{
    struct timer_obj *obj = (struct timer_obj *)timer;
    obj->paused = 1;
    DeleteTimerQueueTimer(hTimerQueue, obj->hTimer, INVALID_HANDLE_VALUE);
}

struct msg
{
    struct msg *next;
    uint8_t body[0];
};

struct msg_queue
{
    gen_handle_t event;
    int item_size;
    struct msg *first;
};

gen_handle_t port_queue_create(int len, int item_size)
{
    struct msg_queue *q = (struct msg_queue *)malloc(sizeof(struct msg_queue));
    memset(q, 0, sizeof(*q));
    q->event = port_event_create();
    q->item_size = item_size;
}

int port_queue_send_msg(gen_handle_t queue, void *msg)
{
    struct msg_queue *q = (struct msg_queue *)queue;
    struct msg *m = (struct msg *)malloc(sizeof(struct msg) + q->item_size);
    memcpy(m->body, msg, q->item_size);
    m->next = NULL;

    enter_critical();
    if (q->first)
    {
        struct msg *prev = q->first;
        while (prev->next) prev = prev->next;
        prev->next = m;
    }
    else
        q->first = m;
    leave_critical();

    port_event_set(q->event);
}

int port_queue_recv_msg(gen_handle_t queue, void *msg)
{
    struct msg_queue *q = (struct msg_queue *)queue;
    struct msg *m = NULL;

check_msg:
    enter_critical();
    m = q->first;
    if (m)
        q->first = m->next;
    leave_critical();

    if (m)
    {
        memcpy(msg, m->body, q->item_size);
        free(m);
        return 0;
    }

    int r = port_event_wait(q->event);
    if (r)
        return r;

    goto check_msg;
}

gen_handle_t port_event_create(void)
{
    return CreateEventA(NULL,
                        FALSE, //    bManualReset,
                        FALSE, //    bInitialState,
                        NULL);
}

int port_event_wait(gen_handle_t event)
{
    return WaitForSingleObject(event, INFINITE) == WAIT_OBJECT_0 ? 0 : 1;
}

void port_event_set(gen_handle_t event)
{
    SetEvent(event);
}

#endif
#ifdef __linux__

struct timer_obj
{
    timer_t id;
    uint32_t timeout_in_ms;
    void (* timer_cb)(void *);
    void *user_data;
};

static void expired(union sigval timer_data){
    struct timer_obj *data = (struct timer_obj *)timer_data.sival_ptr;
    data->timer_cb(data->user_data);
}

gen_handle_t port_timer_create(
        uint32_t timeout_in_ms,
        void *user_data,
        void (* timer_cb)(void *)
    )
{
    struct timer_obj *obj = (struct timer_obj *)malloc(sizeof(struct timer_obj));
    obj->timer_cb = timer_cb;
    obj->user_data = user_data;
    obj->timeout_in_ms = timeout_in_ms;

    /*  sigevent specifies behaviour on expiration  */
    struct sigevent sev = { 0 };

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = &expired;
    sev.sigev_value.sival_ptr = obj;

    int res = timer_create(CLOCK_REALTIME, &sev, &obj->id);
    if (res != 0){
        perror("timer_create:");
        return NULL;
    }

    return (gen_handle_t)obj;
}

void port_timer_start(gen_handle_t timer)
{
    struct timer_obj *obj = (struct timer_obj *)timer;

    uint32_t ms = obj->timeout_in_ms % 1000;
    struct itimerspec its = {   .it_value.tv_sec  = obj->timeout_in_ms / 1000,
                                .it_value.tv_nsec = ms * 1000000,
                                .it_interval.tv_sec  = 0,
                                .it_interval.tv_nsec = 0
                            };

    /* start timer */
    int res = timer_settime(obj->id, 0, &its, NULL);
    if (res != 0){
        perror("timer_settime:");
    }
}

void port_timer_stop(gen_handle_t timer)
{
    struct timer_obj *obj = (struct timer_obj *)timer;
    struct itimerspec its = {   .it_value.tv_sec  = 0,
                                .it_value.tv_nsec = 0,
                                .it_interval.tv_sec  = 0,
                                .it_interval.tv_nsec = 0
                            };

    timer_settime(obj->id, 0, &its, NULL);
}

void port_timer_delete(gen_handle_t timer)
{
    struct timer_obj *obj = (struct timer_obj *)timer;
    timer_delete(obj->id);
    free(obj);
}

struct queue_obj
{
    mqd_t id_send;
    mqd_t id_read;
    int item_size;
};

#define QUEUE_PERMISSIONS (S_IRUSR | S_IWUSR)

gen_handle_t port_queue_create(int len, int item_size)
{

    struct queue_obj *obj = (struct queue_obj *)malloc(sizeof(struct queue_obj));
    obj->item_size = item_size;
    struct mq_attr attr =
    {
        .mq_flags = 0,
        .mq_maxmsg = len,
        .mq_msgsize = item_size,
        .mq_curmsgs = 0,
    };
    char name[100];
    sprintf(name, "/sq-port-%08x", rand());
    obj->id_read = mq_open(name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr);
    if (obj->id_read == -1)
        perror("mq_open(client)");
    obj->id_send = mq_open(name, O_WRONLY);
    if (obj->id_send == -1)
        perror("mq_open(server)");
    return obj;
}

int port_queue_send_msg(gen_handle_t queue, void *msg)
{
    struct queue_obj *obj = (struct queue_obj *)queue;
    return mq_send(obj->id_send, msg, obj->item_size, 0);
}


int port_queue_recv_msg(gen_handle_t queue, void *msg)
{
    struct queue_obj *obj = (struct queue_obj *)queue;
    return mq_receive(obj->id_read, msg, obj->item_size, NULL) == obj->item_size ? 0 : -1;
}

struct event_obj
{
    int state;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

gen_handle_t port_event_create(void)
{
    struct event_obj *obj = (struct event_obj *)malloc(sizeof(struct event_obj));
    obj->state = 0;
    if (pthread_mutex_init(&obj->mutex, NULL))
    {
        perror("pthread_mutex_init");
        return NULL;
    }
    if (pthread_cond_init(&obj->cond, NULL))
    {
        perror("pthread_cond_init");
        return NULL;
    }
    return (gen_handle_t)obj;
}

int port_event_wait(gen_handle_t event)
{
    struct event_obj *obj = (struct event_obj *)event;
    if (pthread_mutex_lock(&obj->mutex))
        return -1;

    while (!obj->state)
    {
        if (pthread_cond_wait(&obj->cond, &obj->mutex))
        {
            pthread_mutex_unlock(&obj->mutex);
            return -1;
        }
    }

    obj->state = 0;

    if (pthread_mutex_unlock(&obj->mutex))
        return -1;

    return 0;
}

void port_event_set(gen_handle_t event)
{
    struct event_obj *obj = (struct event_obj *)event;
    pthread_mutex_lock(&obj->mutex);

    obj->state = 1;

    pthread_cond_broadcast(&obj->cond);

    pthread_mutex_unlock(&obj->mutex);
}

#endif

struct thread_arg
{
    pthread_t thid;
    void *ret;
    void (*entry)(void *);
    void *parameter;
};

static void *thread(void *arg) {
    struct thread_arg *p = (struct thread_arg *)arg;
    p->entry(p->parameter);
    free(p);
    pthread_exit(NULL);
}

gen_handle_t port_task_create(
        const char *name,
        void (*entry)(void *),
        void *parameter,
        uint32_t stack_size,                    // stack size in bytes
        enum gen_os_task_priority priority
    )
{
    struct thread_arg *arg = (struct thread_arg *)malloc(sizeof(struct thread_arg));
    arg->entry = entry;
    arg->parameter = parameter;

    if (pthread_create(&arg->thid, NULL, thread, arg) != 0) {
        perror("pthread_create() error");
    }

    return (gen_handle_t)arg->thid;
}

gen_handle_t port_get_current_tid(void)
{
    return (gen_handle_t)pthread_self();
}

const static struct gen_os_driver driver =
{
    .timer_create = port_timer_create,
    .timer_start = port_timer_start,
    .timer_stop = port_timer_stop,
    .timer_delete = port_timer_delete,

    .task_create = port_task_create,

    .queue_create = port_queue_create,
    .queue_send_msg = port_queue_send_msg,
    .queue_recv_msg = port_queue_recv_msg,

    .event_create = port_event_create,
    .event_set = port_event_set,
    .event_wait = port_event_wait,

    .malloc = malloc,
    .free = free,
    .enter_critical = enter_critical,
    .leave_critical = leave_critical,
};

const void *platform_get_gen_os_driver(void)
{
    return &driver;
}
