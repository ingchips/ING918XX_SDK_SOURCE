#include "port_gen_os_driver.h"
#include "eatt_uart_console.h"

#include "ingsoc.h"

#include <stdio.h>
#include <string.h>

#include "platform_api.h"
#include "bluetooth.h"
#include "sm.h"
#include "profile.h"

#define GEN_OS          ((const gen_os_driver_t *)platform_get_gen_os_driver())

#ifdef TRACE_TO_FLASH
#include "trace.h"
#endif

typedef void (*f_cmd_handler)(const char *param);

typedef struct
{
    const char *cmd;
    f_cmd_handler handler;
} cmd_t;

static void tx_data(const char *d, const uint16_t len);
static const char help[] =  "commands:\n"
                            "h/?                                 show this\n"
                            "r                                   read custom characteristics\n"
                            "w                                   write custom characteristics\n"
                            "n                                   trigger notify test\n"
                            "i                                   trigger indicate test\n"
                            "adv                                 start advertising (slave)\n"
                            ;

void cmd_help(const char *param)
{
    tx_data(help, strlen(help) + 1);
}
void cmd_read(const char *param)
{
    btstack_push_user_msg(USER_MSG_READ_CHAR, NULL, 0);
}
static void cmd_write(const char *param)
{
    btstack_push_user_msg(USER_MSG_WRITE_CHAR, NULL, 0);
}
static void cmd_write_no_rsp(const char *param)
{
    btstack_push_user_msg(USER_MSG_WRITE_NO_RSP_CHAR, NULL, 0);
}
static void cmd_notify(const char *param)
{
    btstack_push_user_msg(USER_MSG_NOTIFY_CHAR, NULL, 0);
}
static void cmd_indicate(const char *param)
{
    btstack_push_user_msg(USER_MSG_INDICATE_CHAR, NULL, 0);
}
static void cmd_start_adv(const char *param)
{
    btstack_push_user_msg(USER_MSG_START_ADV, NULL, 0);
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
        .cmd = "r",
        .handler = cmd_read
    },
    {
        .cmd = "w",
        .handler = cmd_write
    },
    {
        .cmd = "wn",
        .handler = cmd_write_no_rsp
    },
    {
        .cmd = "n",
        .handler = cmd_notify
    },
    {
        .cmd = "i",
        .handler = cmd_indicate
    },
    {
        .cmd = "adv",
        .handler = cmd_start_adv
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
    if (GEN_OS == NULL) {
        // platform_raise_assertion(__FILE__, __LINE__);
         printf("No OS, console disabled.\n");
        return;
    }

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
