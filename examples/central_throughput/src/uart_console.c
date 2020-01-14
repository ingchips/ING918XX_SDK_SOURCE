#include "uart_console.h"

#include "cm32gpm3.h"

#include <stdio.h>
#include <string.h>

#include "platform_api.h"

typedef void (*f_cmd_handler)(const char *param);

int8_t adv_tx_power = 100;

typedef struct
{
    const char *cmd;
    f_cmd_handler handler;
} cmd_t;

void print_addr(const uint8_t *addr)
{
    printf("%02X:%02X:%02X:%02X:%02X:%02X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

static char buffer[20] = {0};

static const char help[] =  "commands:\n"
                            "h/?         show this\n"
                            "start dir   start throughput test on dir\n"
                            "stop  dir   stop throughput test on dir\n\n"
                            "note: dir = s->m, or m->s\n";

void cmd_help(const char *param)
{
    printf(help);
}

tpt_dir_t parse_dir(const char *param)
{
    buffer[0] = '\0';
    sscanf(param, "%s", buffer);
    if (strcmp(buffer, "s->m") == 0) return DIR_S_TO_M;
    if (strcmp(buffer, "m->s") == 0) return DIR_M_TO_S;
    return DIR_UNDEFINED;
}

extern void start_tpt(tpt_dir_t dir);
extern void stop_tpt(tpt_dir_t dir);

void cmd_start(const char *param)
{
    const tpt_dir_t dir = parse_dir(param);
    if (dir == DIR_UNDEFINED)
    {
        cmd_help(NULL);
        return;
    }
    start_tpt(dir);
}

void cmd_stop(const char *param)
{
    const tpt_dir_t dir = parse_dir(param);
    if (dir == DIR_UNDEFINED)
    {
        cmd_help(NULL);
        return;
    }
    stop_tpt(dir);
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
        .cmd = "start",
        .handler = cmd_start
    },
    {
        .cmd = "stop",
        .handler = cmd_stop
    },
};

void handle_command(char *cmd_line)
{
    static const char unknow_cmd[] =  "unknown command\n";
    char *param = cmd_line;
    int i;
    while ((*param != ' ') && (*param != '\0')) param++;
    *param = '\0'; param++;
    
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
    printf(unknow_cmd);
    cmd_help(NULL);
}

typedef struct
{
    uint16_t size;
    char buf[512];
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

void console_rx_data(const char *d, uint8_t len)
{
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
        ((input.buf[input.size - 1] == '\r') || (input.buf[input.size - 1] == '\r')))
    {
        int16_t t = input.size - 2;
        while ((t > 0) && ((input.buf[t] == '\r') || (input.buf[t] == '\n'))) t--;
        input.buf[t + 1] = '\0';
        handle_command(input.buf);
        input.size = 0;
    }
}
