#include "uart_console.h"

#include "ingsoc.h"

#include <stdio.h>
#include <string.h>

#include "platform_api.h"

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

static const char help[] =  "commands:\n"
                            "h/?                        show this\n"
                            "r 0x..                     read data\n"
                            "w 0x.. 0x..                write data\n"
                            "reboot                     reboot\n"
                            "ver                        version\n"                            
                            "advpwr power               set adv power in dBm\n"
                            "name   name                set dev name\n"
                            "addr   01:02:03:04:05:06   set dev address\n"
                            "start                      start advertising\n"
                            "stop                       stop advertising\n";

void cmd_help(const char *param)
{
    tx_data(help, strlen(help) + 1);
}

void cmd_read(const char *param)
{
    uint32_t addr = 0;
    if (sscanf(param, "0x%x", &addr) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    sprintf(buffer, "[0x%08x] = 0x%08x", addr, *(volatile uint32_t *)(addr));
    tx_data(buffer, strlen(buffer) + 1);
}

void cmd_write(const char *param)
{
    uint32_t addr = 0;
    uint32_t value = 0;
    if (sscanf(param, "0x%x 0x%x", &addr, &value) != 2)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    *(volatile uint32_t *)(addr) = value;
    sprintf(buffer, "[0x%08x] = 0x%08x", addr, *(volatile uint32_t *)(addr));
    tx_data(buffer, strlen(buffer) + 1);
}

void cmd_reboot(const char *param)
{
    NVIC_SystemReset();
	while(1);
}

void cmd_version(const char *param)
{
    const platform_ver_t *ver = platform_get_version();
    sprintf(buffer, "version: %d.%d.%d", ver->major, ver->minor, ver->patch);
    tx_data(buffer, strlen(buffer) + 1);
}

void cmd_advpwr(const char *param)
{
    if (sscanf(param, "%d", &adv_tx_power) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    else
    {
        sprintf(buffer, "adv tx power: %ddBm", adv_tx_power);
        tx_data(buffer, strlen(buffer) + 1);
    }
}

extern void set_adv_local_name(const char *name, int16_t len);

void cmd_name(const char *param)
{
    while (*param == ' ') param++;
    set_adv_local_name(param, strlen(param));
}

extern uint8_t rand_addr[];
void update_addr(void);

void cmd_addr(const char *param)
{
    int addr[6];
    if (sscanf(param, "%2x:%2x:%2x:%2x:%2x:%2x", &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]) != 6)
    {
        int i;
        for (i = 0; i < 6; i++) rand_addr[i] = addr[i];
        tx_data(error, strlen(error) + 1);
        return;
    }
    update_addr();
}

extern void start_adv(void);
extern void stop_adv(void);

void cmd_start(const char *param)
{
    start_adv();
}

void cmd_stop(const char *param)
{
    stop_adv();
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
        .cmd = "reboot",
        .handler = cmd_reboot
    },
    {
        .cmd = "ver",
        .handler = cmd_version
    },
    {
        .cmd = "advpwr",
        .handler = cmd_advpwr
    },
    {
        .cmd = "name",
        .handler = cmd_name
    },
    {
        .cmd = "addr",
        .handler = cmd_addr
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
    tx_data(unknow_cmd, strlen(unknow_cmd));
    cmd_help(NULL);
}

typedef struct
{
    uint16_t size;
    char buf[512];
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

extern void stack_notify_tx_data(void);

static void tx_data(const char *d, const uint16_t len)
{
    append_data(&output, d, len);

    if ((output.size > 0) && (output.buf[output.size - 1] == '\0'))
    {
        printf((const char *)output.buf);
        output.size = 0;
    }
}

uint8_t *console_get_clear_tx_data(uint16_t *len)
{
    *len = output.size;
    output.size = 0;
    return (uint8_t *)output.buf;
}
