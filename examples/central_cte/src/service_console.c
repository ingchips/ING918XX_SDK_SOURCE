#include "service_console.h"

#include "ingsoc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform_api.h"
#include "profile.h"
#include "kv_storage.h"

extern settings_t *settings;
extern void set_sample_offset(int n);
extern void set_channel(void);

typedef void (*f_cmd_handler)(const char *param);

typedef struct
{
    const char *cmd;
    f_cmd_handler handler;
} cmd_t;

static char buffer[100] = {0};

uint32_t stack[10] = {0};
int8_t sp;

static void tx_data(const char *d, const uint8_t len);

void cmd_help(const char *param)
{
    static const char help[] =  "commands:\n"
                                "def        id,...           default ant for each pattern\n"
                                "patterns   a0,a0..;b0,b1..  antenna patterns\n"
                                "iqsel      offset           iq sampling offset\n"
                                "duration   1/2              slot duration\n"
                                "ch         n                0..36\n"
                                "ver                         version\n"
                                "reboot                      reboot\n"
                                "led        r,g,b            periperal led\n"
                                "h/?                         show this\n";
    tx_data(help, strlen(help) + 1);
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

void cmd_def_ant(const char *param)
{
    char *first = (char *)param;
    int pat_id = 0;
    while ((pat_id < PAT_NUMBER) && *first)
    {
        char *p = first;

        if (('0' <= *p) && (*p <= '9'))
        {
            ;
        }
        else
        {
            *p = '\0';
            if (strlen(first) > 0)
                settings->patterns[pat_id++].def = atoi(first);
        }
        first = p + 1;
    }
    kv_value_modified();
}

void cmd_iqsel(const char *param)
{
    settings->iq_select = atoi(param);
    set_sample_offset(settings->iq_select);
    kv_value_modified();
}

void cmd_duration(const char *param)
{
    settings->slot_duration = atoi(param) == 1 ? 1 : 2;
    kv_value_modified();
}

void cmd_channel(const char *param)
{
    settings->channel = atoi(param);
    set_channel();
    kv_value_modified();
}

void cmd_led(const char *param)
{
    extern void set_responder_led(int r, int g, int b);
    int r, g, b;
    if (sscanf(param, "%d,%d,%d", &r, &g, &b) != 3)
        return;
    set_responder_led(r, g, b);
}

void cmd_patterns(const char *param)
{
    char *first = (char *)param;
    int pat_id = 0;
    int i = 0;
    while (pat_id < PAT_NUMBER)
    {
        char *p = first;
check_p:
        switch (*p)
        {
        case ';':
            *p = '\0';
            if (strlen(first) > 0)
                settings->patterns[pat_id].ant_ids[i++] = atoi(first);
            settings->patterns[pat_id].len = i;
            i = 0;
            pat_id++;
            first = p + 1;
            break;
        case ',':
            *p = '\0';
            if (strlen(first) > 0)
                settings->patterns[pat_id].ant_ids[i++] = atoi(first);
            first = p + 1;
            break;
        case '\0':
            if (strlen(first) > 0)
                settings->patterns[pat_id].ant_ids[i++] = atoi(first);                
            settings->patterns[pat_id].len = i;
            kv_value_modified();
            return;
        default:
            p++;
            goto check_p;
        }
    }
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
        .cmd = "def",
        .handler = cmd_def_ant
    },
    {
        .cmd = "patterns",
        .handler = cmd_patterns
    },
    {
        .cmd = "iqsel",
        .handler = cmd_iqsel
    },
    {
        .cmd = "duration",
        .handler = cmd_duration
    },
    {
        .cmd = "ver",
        .handler = cmd_version
    },
    {
        .cmd = "ch",
        .handler = cmd_channel
    },
    {
        .cmd = "led",
        .handler = cmd_led
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
    uint8_t size;
    char buf[255];
} str_buf_t;

str_buf_t input = {0};
str_buf_t output = {0};

static void append_data(str_buf_t *buf, const char *d, const uint8_t len)
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

static void tx_data(const char *d, const uint8_t len)
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

void dump_patterns(void)
{
    int i, j;
    for (i = 0; i < PAT_NUMBER; i++)
    {
        if (settings->patterns[i].len < 1) break;
        if (i) platform_printf(";");
        for (j = 0; j < settings->patterns[i].len; j++)
        {
            if (j) platform_printf(",");
            platform_printf("%d", settings->patterns[i].ant_ids[j]);
        }        
    }
}

void test()
{
    char cmd[] = ",,,1,2,3;11,12,,,13,,,;19,18,";
    dump_patterns();
    platform_printf("\n");
    cmd_patterns(cmd);
    dump_patterns();
    platform_printf("\n");
}
