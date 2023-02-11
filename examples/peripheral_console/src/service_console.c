#include "service_console.h"

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

static void tx_data(const char *d, const int len);

void cmd_help(const char *param)
{
    static const char help[] =  "commands:\n"
                                "h/?        help\n"
                                "r 0x..     read memory\n"
                                "w 0x.. ..  modify memory\n"
                                "m          show heap status\n"
                                "reboot     reboot\n"
                                "ps 1/0     enable/disable power saving\n"
                                "ver        show platform version\n"
                                "f          show 32k freq\n"
                                "f-cpu      show CPU freq\n"
                                "advpwr ..  set adv power\n"
                                "latency .. self assigned peripheral latency\n"
                                ;
    tx_data(help, strlen(help) + 1);
}

void cmd_latency(const char *param)
{
    extern void stack_set_latency(int latency);
    int latency = 0;
    if (sscanf(param, "%d", &latency) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    stack_set_latency(latency);
}

void cmd_read(const char *param)
{
    unsigned int addr = 0;
    if (sscanf(param, "0x%x", &addr) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    sprintf(buffer, "[0x%08x] = 0x%08x", addr, *(volatile unsigned int *)(addr));
    tx_data(buffer, strlen(buffer) + 1);
}

void cmd_write(const char *param)
{
    unsigned int addr = 0;
    unsigned int value = 0;
    if (sscanf(param, "0x%x 0x%x", &addr, &value) != 2)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    *(volatile uint32_t *)(addr) = (uint32_t)value;
    sprintf(buffer, "[0x%08x] = 0x%08x", addr, *(volatile unsigned int *)(addr));
    tx_data(buffer, strlen(buffer) + 1);
}

void cmd_powersaving(const char *param)
{
    static const char enabled[]  = "power saving enabled";
    static const char disabled[] = "power saving disabled";

    int flag = 0;
    if (sscanf(param, "%d", &flag) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    if (flag)
    {
        platform_config(PLATFORM_CFG_POWER_SAVING, PLATFORM_CFG_ENABLE);
        tx_data(enabled, strlen(enabled) + 1);
    }
    else
    {
        platform_config(PLATFORM_CFG_POWER_SAVING, PLATFORM_CFG_DISABLE);
        tx_data(disabled, strlen(disabled) + 1);
    }
}

void cmd_reboot(const char *param)
{
    platform_reset();
}

void cmd_version(const char *param)
{
    const platform_ver_t *ver = platform_get_version();
    sprintf(buffer, "version: %d.%d.%d", ver->major, ver->minor, ver->patch);
    tx_data(buffer, strlen(buffer) + 1);
}

extern void platform_get_heap_status(platform_heap_status_t *status);

__attribute((weak)) void show_stack_mem(char *buffer)
{
}

void cmd_mem(const char *param)
{
    platform_heap_status_t status;
    platform_get_heap_status(&status);
    sprintf(buffer, "heap:\nfree: %dB\nmin free:%dB", status.bytes_free, status.bytes_minimum_ever_free);
    tx_data(buffer, strlen(buffer) + 1);
    buffer[0] = '\0';
    show_stack_mem(buffer);
    int l = strlen(buffer);
    if (l > 0)
        tx_data(buffer, l + 1);
}

void cmd_freq(const char *param)
{
    uint32_t cali = platform_read_info(PLATFORM_INFO_32K_CALI_VALUE);
    float f = 1000000. / (cali / 65536.);
    sprintf(buffer, "32k @ %.1f Hz", f);
    tx_data(buffer, strlen(buffer) + 1);
}

void cmd_fcpu(const char *param)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    sprintf(buffer, "cpu @ 48MHz");
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    sprintf(buffer, "cpu @ %u Hz", SYSCTRL_GetHClk());
#endif
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

void cmd_calib(const char *param)
{
    sprintf(buffer, "adv tx power: %ddBm", adv_tx_power);
    tx_data(buffer, strlen(buffer) + 1);
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
        .cmd = "m",
        .handler = cmd_mem
    },
    {
        .cmd = "reboot",
        .handler = cmd_reboot
    },
    {
        .cmd = "ps",
        .handler = cmd_powersaving
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
        .cmd = "f",
        .handler = cmd_freq
    },
    {
        .cmd = "f-cpu",
        .handler = cmd_fcpu
    },
    {
        .cmd = "c",
        .handler = cmd_calib
    },
    {
        .cmd = "latency",
        .handler = cmd_latency
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
    int size;
    char buf[400];
} str_buf_t;

str_buf_t input = {0};
str_buf_t output = {0};

static void append_data(str_buf_t *buf, const char *d, const int len)
{
    if (buf->size + len > sizeof(buf->buf))
        buf->size = 0;

    if (buf->size + len <= sizeof(buf->buf))
    {
        memcpy(buf->buf + buf->size, d, len);
        buf->size += len;
    }
}

void console_rx_data(const char *d, const uint8_t len)
{
    append_data(&input, d, len);

    if ((input.size > 0) && (input.buf[input.size - 1] == '\0'))
    {
        handle_command(input.buf);
        input.size = 0;
    }
}

extern void stack_notify_tx_data(void);

static void tx_data(const char *d, const int len)
{
    append_data(&output, d, len);

    if ((output.size > 0) && (output.buf[output.size - 1] == '\0'))
        stack_notify_tx_data();
}

uint8_t *console_get_clear_tx_data(uint16_t *len)
{
    *len = output.size;
    output.size = 0;
    return (uint8_t *)output.buf;
}
