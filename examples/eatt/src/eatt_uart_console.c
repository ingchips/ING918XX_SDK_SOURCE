#include "port_gen_os_driver.h"
#include "eatt_uart_console.h"

#include "ingsoc.h"

#include <stdio.h>
#include <string.h>

#include "platform_api.h"
#include "bluetooth.h"
#include "sm.h"
#include "FreeRTOS.h"
#include "task.h"
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
                            "reboot                              reboot\n"
                            "ver                                 version\n"

                            "sconn  xx:xx:xx:xx:xx:xx            connect to dev using synced API\n"
                            "read   value_handle                 read value of a characteristic\n"
                            "sread  value_handle                 read value of a characteristic using sync. API\n"
                            "write  handle XX XX ...             write value to a characteristic\n"
                            "w/or   handle XX XX ...             write without response to a char.\n"
                            "sub    handle                       subscribe to a characteristic\n"
                            "sub/o  handle                       subscribe to a characteristic without callback\n"
                            "unsub  handle                       unsubscribe\n"
                            ;
block_value_t periodic_test_data;
int periodic_test_handle = 3; // 
extern void notify_value_of_char(int handle, block_value_t *value);
static void notify_task_entry(void *pdata)
{
    if (parse_handle_value(&periodic_test_handle, pdata) != 0)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    
    periodic_test_data.len = sizeof(periodic_test_data.value);
    uint8_t i = 0;
    for(i = 0; i < periodic_test_data.len; i++)
    {
        periodic_test_data.value[i] = i;
    }
    int j = 0;
    for (;;)
    {
        j++;
        vTaskDelay(500);
        platform_printf("periodic notify %d times\n", j);
        notify_value_of_char(periodic_test_handle, &periodic_test_data);

    }
}
void cmd_periodic_notify_start(const char *param)
{

    GEN_OS->task_create("notify task",
        notify_task_entry,
        (void *)param,
        1024,
        GEN_TASK_PRIORITY_LOW);
}

void cmd_help(const char *param)
{
    tx_data(help, strlen(help) + 1);
}
void cmd_reboot(const char *param)
{
    platform_reset();
}
extern uint8_t slave_addr[];
extern bd_addr_type_t slave_addr_type;

void sync_conn_to_slave(void);
void read_value_of_char(int handle);
void sync_read_value_of_char(int handle);
void write_value_of_char(int handle, block_value_t *value);
void wor_value_of_char(int handle, block_value_t *value);
void sub_to_char(int handle, uint8_t reg_cb);
void unsub_to_char(int handle);
void set_mtu(uint16_t mtu);

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


void cmd_read_char(const char *param)
{
    int t = 0;
    if (sscanf(param, "%d", &t) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    read_value_of_char(t);
}

void cmd_sread_char(const char *param)
{
    int t = 0;
    if (sscanf(param, "%d", &t) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    sync_read_value_of_char(t);
}

block_value_t char_value;

static int parse_handle_value(int *handle, const char *param)
{
    const char *c = param;
    char *t;
    while (*c == ' ') c++;
    char_value.len = 0;
    strcpy(buffer, c);
    t = strtok(buffer, " ");
    if (sscanf(t, "%d", handle) != 1)
        return 1;
    while ((t = strtok(NULL, " ")) != NULL)
    {
        if (char_value.len >= sizeof(char_value.value)) break;
        if (sscanf(t, "%x", (int *)(char_value.value + char_value.len)) != 1)
            break;
        char_value.len++;
    }
    return 0;
}

void cmd_notify_char(const char *param)
{
    int handle;

    if (parse_handle_value(&handle, param) != 0)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    notify_value_of_char(handle, &char_value);
}
void cmd_indicate_char(const char *param)
{
    int handle;

    if (parse_handle_value(&handle, param) != 0)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    extern void indicate_value_of_char(int handle, block_value_t *value);
    indicate_value_of_char(handle, &char_value);
}
void cmd_write_char(const char *param)
{
    int handle;

    if (parse_handle_value(&handle, param) != 0)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    write_value_of_char(handle, &char_value);
}

void cmd_wor_char(const char *param)
{
    int handle;

    if (parse_handle_value(&handle, param) != 0)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    wor_value_of_char(handle, &char_value);
}

void cmd_sub_char(const char *param)
{
    int t = 0;
    if (sscanf(param, "%d", &t) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    sub_to_char(t, 1);
}

void cmd_sub_char_without_cb(const char *param)
{
    int t = 0;
    if (sscanf(param, "%d", &t) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    sub_to_char(t, 0);
}
void cmd_unsub_char(const char *param)
{
    int t = 0;
    if (sscanf(param, "%d", &t) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    unsub_to_char(t);
}


void change_conn_param(int interval, int latency, int timeout);
void ble_re_connect(void);
void ble_show_status(void);


static void cmd_reconn(const char *param)
{
    ble_re_connect();
}

static void cmd_status(const char *param)
{
    ble_show_status();
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
        .cmd = "reboot",
        .handler = cmd_reboot
    },
    {
        .cmd = "sconn",
        .handler = cmd_sconn
    },
    {
        .cmd = "read",
        .handler = cmd_read_char
    },
    {
        .cmd = "sread",
        .handler = cmd_sread_char
    },
    {
        .cmd = "indicate",
        .handler = cmd_indicate_char
    },
    {
        .cmd = "notify",
        .handler = cmd_notify_char
    },
    {
        .cmd = "periodic_notify",
        .handler = cmd_periodic_notify_start
    },
    {
        .cmd = "write",
        .handler = cmd_write_char
    },
    {
        .cmd = "w/or",
        .handler = cmd_wor_char
    },
    {
        .cmd = "sub",
        .handler = cmd_sub_char
    },
    {
        .cmd = "sub/o",
        .handler = cmd_sub_char_without_cb
    },
    {
        .cmd = "unsub",
        .handler = cmd_unsub_char
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

/*
void handle_command()
{
    tx_data("response: ", 10);
    tx_data(input.buf, input.size);
}
*/

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

extern void stack_notify_tx_data(void);

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
