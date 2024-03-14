#include "ingsoc.h"

#include <stdio.h>
#include <string.h>

#include "platform_api.h"
#include "bluetooth.h"

typedef void (*f_cmd_handler)(const char *param);

typedef struct
{
    const char *cmd;
    f_cmd_handler handler;
} cmd_t;

const static char error[] = "error";
static char buffer[100] = {0};

static void tx_data(const char *d, const uint16_t len);

extern void scan_ctrl(int flag, int phy_chan, uint32_t acc_addr);

void cmd_scan(const char *param)
{
    int i;
    uint32_t acc_addr;
    if (sscanf(param, "%d,0x%8x", &i, &acc_addr) != 2)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    scan_ctrl(1, i, acc_addr);
}

static cmd_t cmds[] =
{
    {
        .cmd = "scan",
        .handler = cmd_scan
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
    tx_data(unknow_cmd, strlen(unknow_cmd));
}

typedef struct
{
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
        ((input.buf[input.size - 1] == '\r') || (input.buf[input.size - 1] == '\n')))
    {
        int16_t t = input.size - 2;
        while ((t > 0) && ((input.buf[t] == '\r') || (input.buf[t] == '\n'))) t--;
        input.buf[t + 1] = '\0';
        handle_command(input.buf);
        input.size = 0;
    }
}

extern void stack_notify_tx_data(void);

static void tx_data(const char *d, const uint16_t len)
{
    append_data(&output, d, len);

    if ((output.size > 0) && (output.buf[output.size - 1] == '\0'))
    {
        platform_printf("%s", (const char *)output.buf);
        output.size = 0;
    }
}

uint8_t *console_get_clear_tx_data(uint16_t *len)
{
    *len = output.size;
    output.size = 0;
    return (uint8_t *)output.buf;
}
