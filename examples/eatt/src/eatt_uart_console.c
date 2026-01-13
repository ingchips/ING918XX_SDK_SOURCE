#include "port_gen_os_driver.h"
#include "eatt_uart_console.h"

#include "ingsoc.h"

#include <stdio.h>
#include <string.h>

#include "platform_api.h"
#include "bluetooth.h"
#include "sm.h"

//#include "btstack_mt.h"

#define GEN_OS          ((const gen_os_driver_t *)platform_get_gen_os_driver())

#ifdef TRACE_TO_FLASH
#include "trace.h"
#endif

typedef void (*f_cmd_handler)(const char *param);

int adv_tx_power = 100;

typedef struct
{
    const char *cmd;
    f_cmd_handler handler;
} cmd_t;

const static char error[] = "error";
static char buffer[100] = {0};

static void tx_data(const char *d, const uint16_t len);
static int parse_handle_value(int *handle, const char *param);
static const char help[] =  "commands:\n"
                            "h/?                                 show this\n"
                            "sconn  xx:xx:xx:xx:xx:xx            connect to dev using synced API\n"
                            "discover                            start discovery\n"
                            "read_several_char   value_handle1  value_handle2 ...              read several characteristics\n"
                            ;

void cmd_help(const char *param)
{
    tx_data(help, strlen(help) + 1);
}

extern uint8_t slave_addr[];
extern bd_addr_type_t slave_addr_type;

void sync_conn_to_slave(void);
void read_value_of_char(int handle);
int parse_addr(uint8_t *output, const char *param)
{
    unsigned int addr[6];
    int i = sscanf(param, "%2x:%2x:%2x:%2x:%2x:%2x", &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);

    if (i != 6)
    {
        if (i > 0)
            tx_data(error, strlen(error) + 1);
        return -1;
    }
    for (i = 0; i < 6; i++) output[i] = addr[i];
    return 0;
}

void cmd_sconn(const char *param)
{
    if (0 == parse_addr(slave_addr, param))
        sync_conn_to_slave();
}
void cmd_read_several_char(const char *param)
{
    const char *p = param;
    const char *end = param + strlen(param);
    int handle = 0;
    while (p < end) {

        while (p < end && *p == ' ') {
            p++;
        }

        if (p >= end || *p == '\n') {
            break;
        }

        if (sscanf(p, "%d", &handle) == 1) {
            platform_printf("read char handle: 0x%d\n", handle);
            read_value_of_char(handle);
        }

        while (p < end && *p != ' ' && *p != '\n') {
            p++;
        }
    }
}

static void cmd_discover(const char *param)
{
    extern void discover_services(void);
    discover_services();
}

static cmd_t cmds[] =
{
    {
        .cmd = "h",
        .handler = cmd_help
    },
    {
        .cmd = "?",
        .handler = cmd_help
    },
    {
        .cmd = "sconn",
        .handler = cmd_sconn
    },
    {
        .cmd = "read_several_char",
        .handler = cmd_read_several_char
    },
    {
        .cmd = "discover",
        .handler = cmd_discover
    },
};

void handle_command(char *cmd_line)
{
    static const char unknow_cmd[] =  "unknown command\n";
    char *param = cmd_line;
    int i;
    while (*param)
    {
        if (*param == ' ')
        {
            *param = '\0';
            param++;
            break;
        }
        else
            param++;
    }

    for (i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++)
    {
        if (strcasecmp(cmds[i].cmd, cmd_line) == 0)
            break;
    }
    if (i >= sizeof(cmds) / sizeof(cmds[0]))
        goto show_help;

    cmds[i].handler(param);
    return;

show_help:
    tx_data(unknow_cmd, strlen(unknow_cmd) + 1);
    cmd_help(NULL);
}

typedef struct
{
    uint8_t busy;
    uint16_t size;
    char buf[712];
} str_buf_t;

str_buf_t input = {0};
str_buf_t output = {0};

static void append_data(str_buf_t *buf, const char *d, const uint16_t len)
{
    if (buf->size + len > sizeof(buf->buf))
        buf->size = 0;

    if (buf->size + len <= sizeof(buf->buf))
    {
        memcpy(buf->buf + buf->size, d, len);
        buf->size += len;
    }
}

static gen_handle_t cmd_event = NULL;

static void console_task_entry(void *_)
{
    while (1)
    {
        GEN_OS->event_wait(cmd_event);

        handle_command(input.buf);
        input.size = 0;
        input.busy = 0;
    }
}

void uart_console_start(void)
{
    cmd_event = GEN_OS->event_create();
    GEN_OS->task_create("console",
        console_task_entry,
        NULL,
        1024,
        GEN_TASK_PRIORITY_LOW);
}

void console_rx_data(const char *d, uint8_t len)
{
    if (input.busy)
    {
        return;
    }

    if (0 == input.size)
    {
        while ((len > 0) && ((*d == '\r') || (*d == '\n')))
        {
            d++;
            len--;
        }
    }
    if (len == 0) return;

    append_data(&input, d, len);

    if ((input.size > 0) &&
        ((input.buf[input.size - 1] == '\r') || (input.buf[input.size - 1] == '\n')))
    {
        int16_t t = input.size - 2;
        while ((t > 0) && ((input.buf[t] == '\r') || (input.buf[t] == '\n'))) t--;
        input.buf[t + 1] = '\0';
        input.busy = 1;
        GEN_OS->event_set(cmd_event);
    }
}

static void tx_data(const char *d, const uint16_t len)
{
    if ((output.size == 0) && (d[len - 1] == '\0'))
    {
        puts(d);
        return;
    }

    append_data(&output, d, len);

    if ((output.size > 0) && (output.buf[output.size - 1] == '\0'))
    {
        puts(output.buf);
        output.size = 0;
    }
}

uint8_t *console_get_clear_tx_data(uint16_t *len)
{
    *len = output.size;
    output.size = 0;
    return (uint8_t *)output.buf;
}
