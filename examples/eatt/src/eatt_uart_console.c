#include "eatt_uart_console.h"

#include "ingsoc.h"

#include <stdio.h>
#include <string.h>

#include "platform_api.h"
#include "bluetooth.h"
#include "sm.h"

#include "btstack_mt.h"
#include "port_gen_os_driver.h"

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

static const char help[] =  "commands:\n"
                            "h/?                                 show this\n"
                            "r 0x..                              read data\n"
                            "w 0x.. 0x..                         write data\n"
                            "reboot                              reboot\n"
                            "ver                                 version\n"
                            "advpwr power                        set adv power in dBm\n"
                            "name   name                         set dev name\n"
                            "addr   01:02:03:04:05:06            set dev address\n"
                            "start                               start advertising\n"
                            "stop                                stop advertising\n"
                            "conpar interval latency timeout     set connection parameters\n"
                            "pat    0/1                          peer address type\n"
                            "conn   xx:xx:xx:xx:xx:xx            connect to dev and discover services\n"
                            "sconn  xx:xx:xx:xx:xx:xx            connect to dev using synced API\n"
                            "cancel                              cancel create connection\n"
                            "scan                                passive scan for all adv\n"
                            "scan   xx:xx:xx:xx:xx:xx            scan for adv from a device\n"
                            "ascan                               active scan for all adv\n"
                            "ascan  xx:xx:xx:xx:xx:xx            active scan for adv from a device\n"
                            "read   value_handle                 read value of a characteristic\n"
                            "sread  value_handle                 read value of a characteristic using sync. API\n"
                            "write  handle XX XX ...             write value to a characteristic\n"
                            "w/or   handle XX XX ...             write without response to a char.\n"
                            "sub    handle                       subscribe to a characteristic\n"
                            "sub/o  handle                       subscribe to a characteristic without callback\n"
                            "unsub  handle                       unsubscribe\n"
                            "bond   0/1                          bonding\n"
                            "phy    1m/2m/s2/s8                  central only\n"
                            "assert                              raise assertion\n"
                            "trace  0/1                          enable/disable flash trace\n"
                            "cpwr   power                        set connection tx power in dBm\n"
                            "pwrctl delta                        adjust peer tx power in dB\n"
                            "auto   0/1                          enable/disable auto power control\n"
                            "subr   factor                       subrate with factor\n"
                            "syncgap                             demo sync GAP APIs\n"
                            "lock   freq                         lock to freq (MHz). 0 to unlock\n"
                            "re-conn                             demo of abort & re-connect\n"
                            "status                              show controller status\n"
                            "mtu    n                            customize mtu for new connections\n"
                            ;

void cmd_help(const char *param)
{
    tx_data(help, strlen(help) + 1);
}

void cmd_sync_gap(const char *param)
{
    extern void ble_sync_gap(void);
    ble_sync_gap();
}

void cmd_auto(const char *param)
{
    extern void ble_set_auto_power_control(int enable);
    int enable = 0;
    if (sscanf(param, "%d", &enable) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    ble_set_auto_power_control(enable);
}

void cmd_cpwr(const char *param)
{
    extern void ble_set_conn_power(int level);
    int level = 0;
    if (sscanf(param, "%d", &level) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    ble_set_conn_power(level);
}

void cmd_subr(const char *param)
{
    extern void ble_set_conn_subrate(int factor);
    int value = 0;
    if (sscanf(param, "%d", &value) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    ble_set_conn_subrate(value);
}

void cmd_pwrctl(const char *param)
{
    extern void ble_adjust_peer_tx_power(int delta);
    int delta = 0;
    if (sscanf(param, "%d", &delta) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    ble_adjust_peer_tx_power(delta);
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
    platform_reset();
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

extern sm_persistent_t sm_persistent;
extern uint8_t slave_addr[];
extern bd_addr_type_t slave_addr_type;

void update_addr(void);
void conn_to_slave(void);
void sync_conn_to_slave(void);
void cancel_create_conn(void);
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

void cmd_addr(const char *param)
{
    if (0 == parse_addr(sm_persistent.identity_addr, param))
        update_addr();
}

void cmd_conn(const char *param)
{
    if (0 == parse_addr(slave_addr, param))
        conn_to_slave();
}

void cmd_sconn(const char *param)
{
    if (0 == parse_addr(slave_addr, param))
        sync_conn_to_slave();
}

void cmd_scan(const char *param)
{
    extern void start_scan(int targeted);
    start_scan(0 == parse_addr(slave_addr, param) ? 1 : 0);
}

void cmd_active_scan(const char *param)
{
    extern void start_scan_legacy(int targeted);
    start_scan_legacy(0 == parse_addr(slave_addr, param) ? 1 : 0);
}

void cmd_conn_cancel(const char *param)
{
    cancel_create_conn();
}

void cmd_pat(const char *param)
{
    int t = 0;
    if (sscanf(param, "%d", &t) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    slave_addr_type = (bd_addr_type_t)t;
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

void cmd_bond(const char *param)
{
    extern void set_bonding(int f);
    int flag;
    if (sscanf(param, "%d", &flag) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    set_bonding(flag);
}

block_value_t char_value;

int parse_handle_value(int *handle, const char *param)
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

void cmd_mtu(const char *param)
{
    int t = 0;
    if (sscanf(param, "%d", &t) != 1)
    {
        tx_data(error, strlen(error) + 1);
        return;
    }
    set_mtu((uint16_t)t);
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

void set_phy(int phy);
void change_conn_param(int interval, int latency, int timeout);
void ble_re_connect(void);
void ble_show_status(void);

void cmd_phy(const char *param)
{
    int phy;
    if (strcmp(param, "1m") == 0) phy = 0;
    else if (strcmp(param, "2m") == 0) phy = 1;
    else if (strcmp(param, "s2") == 0) phy = 2;
    else if (strcmp(param, "s8") == 0) phy = 3;
    else return;
    set_phy(phy);
}

void cmd_conpar(const char *param)
{
    int interval = 0;
    int latency = 0;
    int timeout = 0;
    if (sscanf(param, "%d %d %d", &interval, &latency, &timeout) < 1) return;
    change_conn_param(interval, latency, timeout);
}

void cmd_assert(const char *param)
{
    platform_raise_assertion("uart_console.c", __LINE__);
}

int lock_to_freq = 0;

void cmd_lock(const char *param)
{
    sscanf(param, "%d", &lock_to_freq);
    if (lock_to_freq > 0)
    {
        ll_lock_frequency(lock_to_freq);
        platform_printf("locked to %dMHz\n", lock_to_freq);
    }
    else
    {
        ll_unlock_frequency();
        platform_printf("unlocked\n");
    }
}

static void cmd_reconn(const char *param)
{
    ble_re_connect();
}

static void cmd_status(const char *param)
{
    ble_show_status();
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
    {
        .cmd = "conn",
        .handler = cmd_conn
    },
    {
        .cmd = "sconn",
        .handler = cmd_sconn
    },
    {
        .cmd = "pat",
        .handler = cmd_pat
    },
    {
        .cmd = "cancel",
        .handler = cmd_conn_cancel
    },
    {
        .cmd = "scan",
        .handler = cmd_scan
    },
    {
        .cmd = "ascan",
        .handler = cmd_active_scan
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
        .cmd = "bond",
        .handler = cmd_bond
    },
    {
        .cmd = "phy",
        .handler = cmd_phy
    },
    {
        .cmd = "conpar",
        .handler = cmd_conpar
    },
    {
        .cmd = "assert",
        .handler = cmd_assert
    },
    {
        .cmd = "cpwr",
        .handler = cmd_cpwr
    },
    {
        .cmd = "pwrctl",
        .handler = cmd_pwrctl
    },
    {
        .cmd = "subr",
        .handler = cmd_subr
    },
    {
        .cmd = "auto",
        .handler = cmd_auto
    },
    {
        .cmd = "syncgap",
        .handler = cmd_sync_gap
    },
    {
        .cmd = "lock",
        .handler = cmd_lock
    },
    {
        .cmd = "re-conn",
        .handler = cmd_reconn
    },
    {
        .cmd = "status",
        .handler = cmd_status
    },
    {
        .cmd = "mtu",
        .handler = cmd_mtu
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
