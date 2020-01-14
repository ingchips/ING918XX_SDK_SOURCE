#include "uart_console.h"

#include "cm32gpm3.h"

#include <stdio.h>
#include <string.h>

#include "bluetooth.h"
#include "platform_api.h"

extern uint8_t *get_slave_addr(const uint8_t index);
extern void set_slave_addr(const uint8_t index, const uint8_t *addr);

typedef void (*f_cmd_handler)(const char *param);

typedef struct
{
    const char *cmd;
    f_cmd_handler handler;
} cmd_t;

const static char error[] = "error";

static void tx_data(const char *d, const uint16_t len);

static const char help[] =  "commands:\n"
                            "h/?                          show this info\n"
                            "addr     01:02:03:04:05:06   set dev address\n"
                            "slave n  01:02:03:04:05:06   set slave n (0..3) addr\n"
                            "reboot                       reboolt\n"
                            "start                        start\n";

void cmd_help(const char *param)
{
    tx_data(help, strlen(help) + 1);
}

void cmd_reboot(const char *param)
{
    NVIC_SystemReset();
	while(1);
}

extern void set_adv_local_name(const char *name, int16_t len);

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

void print_addr(const uint8_t *addr)
{
    iprintf("%02X:%02X:%02X:%02X:%02X:%02X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

extern bd_addr_t slave_addr_lst[4];

void cmd_slave(const char *param)
{
    bd_addr_t rand_addr;
    int index;
    int addr[6];
    if (sscanf(param, "%d %2x:%2x:%2x:%2x:%2x:%2x", &index, &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]) == 6)
    {
        int i;
        for (i = 0; i < 6; i++) rand_addr[i] = addr[i];
        set_slave_addr(index, rand_addr);
    }

    for (index = 0; index < sizeof(slave_addr_lst) / sizeof(slave_addr_lst[0]); index++)
    {
        iprintf("slave[%d]: ", index);
        print_addr(get_slave_addr(index));
    }
}

extern void start_run(void);

void cmd_start(const char *param)
{
    start_run();
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
        .cmd = "addr",
        .handler = cmd_addr
    },
    {
        .cmd = "slave",
        .handler = cmd_slave
    },
    {
        .cmd = "start",
        .handler = cmd_start
    }
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
        iprintf((const char *)output.buf);
        output.size = 0;
    }
}

uint8_t *console_get_clear_tx_data(uint16_t *len)
{
    *len = output.size;
    output.size = 0;
    return (uint8_t *)output.buf;
}
