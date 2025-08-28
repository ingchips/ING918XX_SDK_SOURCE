#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "profile.h"
#include "gatt_client.h"
#include "sig_uuid.h"
#include "str_util.h"

#include "../fatfs/ff.h"

#include "ring_buf.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "log_util.h"

#define RING_BUF_SIZE  (1024 * 4)
#define INVALID_HANDLE 0xffff

static uint8_t the_ring_buf[RING_BUF_SIZE];
struct ring_buf *ring_buf_obj;

enum
{
    mode_uart,
    mode_ble,
};

conn_para_t conn_param =  {
        .scan_int = 200,
        .scan_win = 100,
        .interval_min = 50,
        .interval_max = 50,
        .latency = 0,
        .supervision_timeout = 600,
        .min_ce_len = 90,
        .max_ce_len = 90
};

struct settings
{
    uint8_t mode;                   // uart | ble
    uint8_t use_timestamp;          // 0/1. for uart, only available when uart_data_fmt is 'str'
    uint32_t uart_baud;             // integer (115200, 9600, etc)
    uint8_t uart_parity;            // none | odd | even
    uint8_t uart_stop_bits;         // 1 | 2
    uint8_t uart_data_bits;         // 5 | 6 | 7 | 8
    uint8_t uart_data_fmt;          // raw | str
    uint8_t ble_data_fmt;           // raw | hex
    uint8_t ble_phy;                // coded | 1M | 2M
    uint8_t ble_addr_type;          // public | random
    uint8_t ble_addr[6];            // ble address
    uint8_t ble_service_uuid[16];   // ble service uuid
    uint8_t ble_char_uuid[16];      // ble characteristic uuid
} g_settings =
{
    .mode = mode_uart,
    .use_timestamp = 1,
    .uart_baud = 115200,
    .uart_parity = 0,
    .uart_stop_bits = 0,
    .uart_data_bits = UART_WLEN_8_BITS,
    .uart_data_fmt = 1,
    .ble_data_fmt = 1,
    .ble_addr_type = 1,
    .ble_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .ble_service_uuid = {0x00,0x00,0x00,0x06,0x49,0x4e,0x47,0x43,0x48,0x49,0x50,0x53,0x55,0x55,0x49,0x44},
    .ble_char_uuid = {0xbf,0x83,0xf3,0xf2,0x39,0x9a,0x41,0x4d,0x90,0x35,0xce,0x64,0xce,0xb3,0xff,0x67},
};

typedef struct slave_info
{
    gatt_client_service_t                   service;
    gatt_client_characteristic_t            output_char;
    gatt_client_characteristic_descriptor_t output_desc;
    gatt_client_notification_t              output_notify;
    uint16_t    conn_handle;
} slave_info_t;

slave_info_t slave;

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
        // add your code
    //case MY_MESSAGE_ID:
    //    break;
    default:
        ;
    }
}

static uint32_t get_sig_short_uuid(const uint8_t *uuid128)
{
    return uuid_has_bluetooth_prefix(uuid128) ? big_endian_read_32(uuid128, 0) : 0;
}

void print_to_ring_buf(const char *str)
{
    char c = '\r';
    ring_buf_write_data(ring_buf_obj, str, strlen(str));
    ring_buf_write_data(ring_buf_obj, &c, 1);
}

static void output_notification_handler_hex(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    const gatt_event_value_packet_t *value_packet;
    uint16_t value_size;
    char c = '\r';
    switch (packet[0])
    {
    case GATT_EVENT_NOTIFICATION:
        value_packet = gatt_event_notification_parse(packet, size, &value_size);
        ring_buf_write_data(ring_buf_obj, &c, 1);
        if (g_settings.use_timestamp)
        {
            c = '[';
            ring_buf_write_data(ring_buf_obj, &c, 1);
            const char *s = log_rtc_timestamp();
            ring_buf_write_data(ring_buf_obj, s, strlen(s));
            c = ']';
            ring_buf_write_data(ring_buf_obj, &c, 1);
            c = '\r';
            ring_buf_write_data(ring_buf_obj, &c, 1);
        }
        print_hex_table(value_packet->value, value_size, print_to_ring_buf);
        break;
    }
}

static void output_notification_handler_raw(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    const gatt_event_value_packet_t *value_packet;
    uint16_t value_size;
    char c = '\r';
    switch (packet[0])
    {
    case GATT_EVENT_NOTIFICATION:
        value_packet = gatt_event_notification_parse(packet, size, &value_size);
        ring_buf_write_data(ring_buf_obj, &c, 1);
        if (g_settings.use_timestamp)
        {
            c = '[';
            ring_buf_write_data(ring_buf_obj, &c, 1);
            const char *s = log_rtc_timestamp();
            ring_buf_write_data(ring_buf_obj, s, strlen(s));
            c = ']';
            ring_buf_write_data(ring_buf_obj, &c, 1);
        }

        ring_buf_write_data(ring_buf_obj, value_packet->value, value_size);
        break;
    }
}

static uint16_t char_config_notification = GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION;

void dummy_callback(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_QUERY_COMPLETE:
        if (gatt_event_query_complete_parse(packet)->status != 0)
            return;
        break;
    }
}

void descriptor_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT:
        {
            const gatt_event_all_characteristic_descriptors_query_result_t *result =
                gatt_event_all_characteristic_descriptors_query_result_parse(packet);
            if (get_sig_short_uuid(result->descriptor.uuid128) ==
                SIG_UUID_DESCRIP_GATT_CLIENT_CHARACTERISTIC_CONFIGURATION)
                slave.output_desc = result->descriptor;
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (slave.output_desc.handle != INVALID_HANDLE)
        {
            platform_printf("starting notification\n");
            gatt_client_listen_for_characteristic_value_updates(&slave.output_notify,
                g_settings.ble_data_fmt == 0 ? output_notification_handler_raw : output_notification_handler_hex,
                slave.conn_handle, slave.output_char.value_handle);
            gatt_client_write_characteristic_descriptor_using_descriptor_handle(dummy_callback, slave.conn_handle,
                slave.output_desc.handle, sizeof(char_config_notification),
                (uint8_t *)&char_config_notification);
        }
        else
        {
            platform_printf("No descriptor found\n");
            gap_disconnect(slave.conn_handle);
        }
        break;
    }
}

void characteristic_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_CHARACTERISTIC_QUERY_RESULT:
        {
            const gatt_event_characteristic_query_result_t *result =
                gatt_event_characteristic_query_result_parse(packet);
            slave.output_char = result->characteristic;
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (INVALID_HANDLE == slave.output_char.value_handle)
        {
            platform_printf("No output characteristic found\n");
            gap_disconnect(slave.conn_handle);
        }
        else
        {
            platform_printf("discovering descriptor\n");
            gatt_client_discover_characteristic_descriptors(descriptor_discovery_callback, slave.conn_handle, &slave.output_char);
        }
        break;
    }
}

void service_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_SERVICE_QUERY_RESULT:
        slave.service = gatt_event_service_query_result_parse(packet)->service;
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (slave.service.start_group_handle != INVALID_HANDLE)
        {
            platform_printf("discovering characteristics\n");
            if (uuid_has_bluetooth_prefix(g_settings.ble_char_uuid))
            {
                uint16_t uuid = (g_settings.ble_char_uuid[2] << 8) | g_settings.ble_char_uuid[3];
                gatt_client_discover_characteristics_for_handle_range_by_uuid16(characteristic_discovery_callback, slave.conn_handle,
                    slave.service.start_group_handle, slave.service.end_group_handle, uuid);
            }
            else
                gatt_client_discover_characteristics_for_handle_range_by_uuid128(characteristic_discovery_callback, slave.conn_handle,
                    slave.service.start_group_handle, slave.service.end_group_handle, g_settings.ble_char_uuid);
        }
        else
        {
            platform_printf("Service not found\n");
            gap_disconnect(slave.conn_handle);
        }
        break;
    }
}

static initiating_phy_config_t phy_configs[] =
{
    {
        .phy = PHY_1M,
    },
    {
        .phy = PHY_2M,
    },
    {
        .phy = PHY_CODED,
    }
};

static void initiate(void)
{
    slave.service.start_group_handle = INVALID_HANDLE;
    slave.output_desc.handle = INVALID_HANDLE;
    slave.output_char.value_handle = INVALID_HANDLE;
    slave.conn_handle = INVALID_HANDLE;
    gap_ext_create_connection(INITIATING_ADVERTISER_FROM_PARAM,
                                BD_ADDR_TYPE_LE_RANDOM,           // Own_Address_Type,
                                (bd_addr_type_t)g_settings.ble_addr_type,         // Peer_Address_Type,
                                g_settings.ble_addr,              // Peer_Address,
                                sizeof(phy_configs) / sizeof(phy_configs[0]),
                                phy_configs);
}

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    static const bd_addr_t rand_addr = { 0xD4, 0x24, 0x05, 0x66, 0x22, 0x2D };
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
        gap_set_random_device_address(rand_addr);
        platform_printf("create connection...\n");
        initiate();
        break;

    case HCI_EVENT_COMMAND_COMPLETE:
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:
            {
                const le_meta_event_enh_create_conn_complete_t * complete =
                    decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);
                platform_printf("connected\n");
                slave.conn_handle = complete->handle;
                gap_read_remote_used_features(slave.conn_handle);
            }
            break;
        case HCI_SUBEVENT_LE_READ_REMOTE_USED_FEATURES_COMPLETE:
            {
                const le_meta_event_read_remote_feature_complete_t * complete =
                    decode_hci_le_meta_event(packet, le_meta_event_read_remote_feature_complete_t);

                if (complete->status == 0)
                {
                    // try to change phy according to settings & features
                    if ((g_settings.ble_phy == 2) && (complete->features[1] & 0x01))
                        gap_set_phy(slave.conn_handle, 0, PHY_2M_BIT, PHY_2M_BIT, HOST_PREFER_S2_CODING);
                    else if ((g_settings.ble_phy == 0) && (complete->features[1] & 0x08))
                        gap_set_phy(slave.conn_handle, 0, PHY_CODED_BIT, PHY_CODED_BIT, HOST_PREFER_S2_CODING);
                    else if (g_settings.ble_phy == 1)
                        gap_set_phy(slave.conn_handle, 0, PHY_1M_BIT, PHY_1M_BIT, HOST_PREFER_S2_CODING);
                }
                platform_printf("discovering services\n");
                if (uuid_has_bluetooth_prefix(g_settings.ble_service_uuid))
                {
                    uint16_t uuid = (g_settings.ble_service_uuid[2] << 8) | g_settings.ble_service_uuid[3];
                    gatt_client_discover_primary_services_by_uuid16(service_discovery_callback, slave.conn_handle, uuid);
                }
                else
                    gatt_client_discover_primary_services_by_uuid128(service_discovery_callback, slave.conn_handle, g_settings.ble_service_uuid);
            }
            break;
        default:
            break;
        }
        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        initiate();
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        // add your code
        break;

    case BTSTACK_EVENT_USER_MSG:
        p_user_msg = hci_event_packet_get_user_msg(packet);
        user_msg_handler(p_user_msg->msg_id, p_user_msg->data, p_user_msg->len);
        break;

    default:
        break;
    }
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

FATFS fs = {0};

FIL fil;            /* File object */

char *split_kv(char *str, char **val)
{
    char *p = strchr(str, '=');
    if (!p)
        return NULL;
    *p = '\0';
    p++;
    *val = p;
    while (*p)
    {
        if ((*p == '\n') || (*p == '\r'))
        {
            *p = '\0';
            break;
        }
        p++;
    }
    return str;
}

static uint8_t hex2bin(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 0;
}

static void read_hex(const char *s, uint8_t *buf, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        while (s[0] == '-') s++;
        buf[i] = hex2bin(s[0]) << 4 | hex2bin(s[1]);
        s += 2;
    }
}

int write_hex_str(char *str, uint8_t *buf, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        sprintf(str, "%02X", buf[i]);
        str += 2;
    }
    return len * 2;
}

#define save_hex_item(name)     \
    len = sprintf(buf, #name "=");                                          \
    len += write_hex_str(buf + len, g_settings. name, sizeof(g_settings.name));    \
    len += sprintf(buf + len, "\n");                                        \
    f_write(fil, buf, len, &len);

#define save_int_item(stru, name)     \
    len = sprintf(buf, #name "=%d\n", stru . name);                    \
    f_write(fil, buf, len, &len);

void save_settings(FIL *fil)
{
    char buf[256];
    unsigned int len;

    len = sprintf(buf, g_settings.mode == 0 ? "mode=uart\n" : "mode=ble\n");
    f_write(fil, buf, len, &len);

    save_int_item(g_settings, use_timestamp);
    save_int_item(g_settings, uart_baud)

    switch (g_settings.uart_parity)
    {
    case UART_PARITY_ODD_PARITY:
        len = sprintf(buf, "uart_parity=odd\n");
        break;
    case UART_PARITY_EVEN_PARITY:
        len = sprintf(buf, "uart_parity=even\n");
        break;
    default:
        len = sprintf(buf, "uart_parity=none\n");
        break;
    }
    f_write(fil, buf, len, &len);

    len = sprintf(buf, "uart_stop_bits=%d\n", g_settings.uart_stop_bits == 1 ? 2 : 1);
    f_write(fil, buf, len, &len);

    switch (g_settings.uart_data_bits)
    {
    case UART_WLEN_5_BITS:
        len = sprintf(buf, "uart_data_bits=5\n");
        break;
    case UART_WLEN_6_BITS:
        len = sprintf(buf, "uart_data_bits=6\n");
        break;
    case UART_WLEN_7_BITS:
        len = sprintf(buf, "uart_data_bits=7\n");
        break;
    default:
        len = sprintf(buf, "uart_data_bits=8\n");
        break;
    }
    f_write(fil, buf, len, &len);

    len = sprintf(buf, "uart_data_fmt=%s\n", g_settings.uart_data_fmt ? "str" : "raw");
    f_write(fil, buf, len, &len);

    switch (g_settings.ble_phy)
    {
    case 1:
        len = sprintf(buf, "ble_phy=1M\n");
        break;
    case 2:
        len = sprintf(buf, "ble_phy=2M\n");
        break;
    default:
        len = sprintf(buf, "ble_phy=coded\n");
        break;
    }
    f_write(fil, buf, len, &len);

    len = sprintf(buf, "ble_data_fmt=%s\n", g_settings.ble_data_fmt ? "hex" : "raw");
    f_write(fil, buf, len, &len);

    len = sprintf(buf, "ble_addr_type=%s\n", g_settings.ble_addr_type ? "random" : "public");
    f_write(fil, buf, len, &len);

    save_hex_item(ble_addr);
    save_hex_item(ble_service_uuid);
    save_hex_item(ble_char_uuid);

    save_int_item(conn_param, scan_int);
    save_int_item(conn_param, scan_win);
    save_int_item(conn_param, interval_min);
    save_int_item(conn_param, interval_max);
    save_int_item(conn_param, latency);
    save_int_item(conn_param, supervision_timeout);
    save_int_item(conn_param, min_ce_len);
    save_int_item(conn_param, max_ce_len);
}

#define line_max_len 128

static void load_ble_conn_param(char *key, char *val)
{
    #define load_int_item(stru, name) \
    if (strcmp(key, #name) == 0) { stru . name = atoi(val);  } else

    load_int_item(conn_param, scan_int)
    load_int_item(conn_param, scan_win)
    load_int_item(conn_param, interval_min)
    load_int_item(conn_param, interval_max)
    load_int_item(conn_param, latency)
    load_int_item(conn_param, supervision_timeout)
    load_int_item(conn_param, min_ce_len)
    load_int_item(conn_param, max_ce_len);
}

static void load_settings(FIL *fil)
{
    char *s = malloc(line_max_len + 1);
    char *key;
    char *val;
    while (!f_eof(fil))
    {
        f_gets(s, line_max_len, fil);
        key = split_kv(s, &val);
        if (key == NULL) continue;
        if (strcmp(key, "mode") == 0)
        {
            if (strcmp(val, "uart") == 0)
                g_settings.mode = mode_uart;
            else if (strcmp(val, "ble") == 0)
                g_settings.mode = mode_ble;
        }
        else if (strcmp(key, "use_timestamp") == 0)
        {
            g_settings.use_timestamp = atoi(val);
        }
        else if (strcmp(key, "uart_baud") == 0)
        {
            g_settings.uart_baud = atoi(val);
        }
        else if (strcmp(key, "uart_parity") == 0)
        {
            if (strcmp(val, "none") == 0)
                g_settings.uart_parity = UART_PARITY_NOT_CHECK;
            else if (strcmp(val, "even") == 0)
                g_settings.uart_parity = UART_PARITY_EVEN_PARITY;
            else if (strcmp(val, "odd") == 0)
                g_settings.uart_parity = UART_PARITY_ODD_PARITY;
        }
        else if (strcmp(key, "uart_stop_bits") == 0)
        {
            if (strcmp(val, "2") == 0)
                g_settings.uart_stop_bits = 1;
        }
        else if (strcmp(key, "uart_data_bits") == 0)
        {
            if (strcmp(val, "5") == 0)
                g_settings.uart_data_bits = UART_WLEN_5_BITS;
            else if (strcmp(val, "6") == 0)
                g_settings.uart_data_bits = UART_WLEN_6_BITS;
            else if (strcmp(val, "7") == 0)
                g_settings.uart_data_bits = UART_WLEN_7_BITS;
        }
        else if (strcmp(key, "uart_data_fmt") == 0)
        {
            g_settings.uart_data_fmt = strcmp(val, "raw") == 0 ? 0 : 1;
        }
        else if (strcmp(key, "ble_data_fmt") == 0)
        {
            g_settings.ble_data_fmt = strcmp(val, "raw") == 0 ? 0 : 1;
        }
        else if (strcmp(key, "ble_phy") == 0)
        {
            if (strcmp(val, "1M") == 0)
                g_settings.ble_phy = 1;
            else if (strcmp(val, "2M") == 0)
                g_settings.ble_phy = 2;
            else
                g_settings.ble_phy = 0;
        }
        else if (strcmp(key, "ble_addr_type") == 0)
        {
            g_settings.ble_addr_type = strcmp(val, "random") == 0 ? 1 : 0;
        }
        else if (strcmp(key, "ble_addr") == 0)
        {
            read_hex(val, g_settings.ble_addr, sizeof(g_settings.ble_addr));
        }
        else if (strcmp(key, "ble_service_uuid") == 0)
        {
            read_hex(val, g_settings.ble_service_uuid, sizeof(g_settings.ble_service_uuid));
        }
        else if (strcmp(key, "ble_char_uuid") == 0)
        {
            read_hex(val, g_settings.ble_char_uuid, sizeof(g_settings.ble_char_uuid));
        }
        else
        {
            load_ble_conn_param(key, val);
        }
    }
    free(s);
}

#define STR_MAX_LEN 102
#define BIN_MAX_LEN 32
static char line_buf[STR_MAX_LEN + 1] = {0};
static int line_len = 0;

static uint32_t uart_isr_byte(void *user_data)
{
    uint32_t status;

    while(1)
    {
        status = apUART_Get_all_raw_int_stat(COMM_PORT);
        if (status == 0) break;
        COMM_PORT->IntClear = status;
        if (status & (1 << bsUART_RECEIVE_INTENAB))
        {
            while (apUART_Check_RXFIFO_EMPTY(COMM_PORT) != 1)
            {
                char c = COMM_PORT->DataRead;
                line_buf[line_len++] = c;
                if (line_len >= STR_MAX_LEN)
                {
                    ring_buf_write_data(ring_buf_obj, line_buf, line_len);
                    line_len = 0;
                }
            }
        }
    }
    return 0;
}

static void flush_line(FIL *fil)
{
    if (g_settings.use_timestamp)
    {
        char c = '[';
        ring_buf_write_data(ring_buf_obj, &c, 1);
        const char *s = log_rtc_timestamp();
        ring_buf_write_data(ring_buf_obj, s, strlen(s));
        c = ']';
        ring_buf_write_data(ring_buf_obj, &c, 1);
    }

    line_buf[line_len] = '\r';
    ring_buf_write_data(ring_buf_obj, line_buf, line_len + 1);
    line_len = 0;
}

static uint32_t uart_isr_str(void *user_data)
{
    uint32_t status;

    while(1)
    {
        status = apUART_Get_all_raw_int_stat(COMM_PORT);
        if (status == 0) break;
        COMM_PORT->IntClear = status;
        if (status & (1 << bsUART_RECEIVE_INTENAB))
        {
            while (apUART_Check_RXFIFO_EMPTY(COMM_PORT) != 1)
            {
                char c = COMM_PORT->DataRead;

                if ((c == '\n') || (c == '\r'))
                {
                    flush_line(&fil);
                }
                else
                {
                    line_buf[line_len++] = c;
                    if (line_len >= STR_MAX_LEN)
                        flush_line(&fil);
                }

            }
        }
    }
    return 0;
}

static void config_comm_uart()
{
    UART_sStateStruct config;

    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_UART1));
    PINCTRL_SetPadMux(PIN_UART_RX, IO_SOURCE_GENERAL);
    PINCTRL_SelUartRxdIn(UART_PORT_1, PIN_UART_RX);

    config.word_length       = (UART_eWLEN)g_settings.uart_data_bits;
    config.parity            = (UART_ePARITY)g_settings.uart_parity;
    config.fifo_enable       = 1;
    config.two_stop_bits     = g_settings.uart_stop_bits;
    config.receive_en        = 1;
    config.transmit_en       = 1;
    config.UART_en           = 1;
    config.cts_en            = 0;
    config.rts_en            = 0;
    config.rxfifo_waterlevel = 5;
    config.txfifo_waterlevel = 1;
    config.ClockFrequency    = OSC_CLK_FREQ;
    config.BaudRate          = g_settings.uart_baud;

    apUART_Initialize(COMM_PORT, &config, 1 << bsUART_RECEIVE_INTENAB);
    platform_set_irq_callback(PLATFORM_CB_IRQ_UART1, g_settings.uart_data_fmt ? uart_isr_str : uart_isr_byte, NULL);
}

#define MAX_LOG_ID  99999
#define print_err(tag, code) platform_printf(tag " err: %d\n", code)

static SemaphoreHandle_t sem_datamark = NULL;

static void highwater_cb(struct ring_buf *buf)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (IS_IN_INTERRUPT())
        xSemaphoreGiveFromISR(sem_datamark, &xHigherPriorityTaskWoken);
    else
        xSemaphoreGive(sem_datamark);
}

static int ring_peek_data(const void *data, int len, int has_more, FIL *fil)
{
    UINT bw;
    f_write(fil, data, (UINT)len, &bw);
    return (int)bw;
}

static void write_back_task(void *pdata)
{
    unsigned int cnt = 0;
    for (;;)
    {
        xSemaphoreTake(sem_datamark,  500);
        ring_buf_peek_data(ring_buf_obj, (f_ring_peek_data)ring_peek_data, &fil);
        cnt++;
        if ((cnt & 0x1f) == 0) f_sync(&fil);
    }
}

uint32_t setup_profile(void *data, void *user_data)
{
    int i;
    char log_name[20];

    sem_datamark = xSemaphoreCreateBinary();
    ring_buf_obj = ring_buf_init(the_ring_buf, sizeof(the_ring_buf), highwater_cb);

    platform_printf("mounting...\n");
    int res = f_mount(&fs, "", 1);
    if (res)
    {
        print_err("f_mount", res);
        return 0;
    }

    res = f_open(&fil, "config.txt", FA_OPEN_EXISTING | FA_READ);

    if (res == 0)
        load_settings(&fil);
    else
    {
        platform_printf("gen config...\n");
        f_open(&fil, "config.txt", FA_CREATE_ALWAYS | FA_WRITE);
        save_settings(&fil);
    }
    f_close(&fil);

    platform_printf("searching...\n");
    for (i = 0; i <= MAX_LOG_ID; i++)
    {
        FILINFO fno;
        sprintf(log_name, "%05d.txt\n", i);
        platform_printf("check %s\n", log_name);
        if (f_stat(log_name, &fno) == FR_NO_FILE)
            break;
    }
    if (i > MAX_LOG_ID)
    {
        platform_printf("disk used up\n");
        return 0;
    }

    res = f_open(&fil, log_name, FA_CREATE_NEW | FA_WRITE);
    if (res)
    {
        print_err("f_open(log)", res);
        return 0;
    }

    xTaskCreate(write_back_task,
               "b",
               configMINIMAL_STACK_SIZE,
               NULL,
               1,
               NULL);

    if (g_settings.mode == mode_uart)
    {
        platform_printf("uart setup...\n");
        config_comm_uart();
        return 0;
    }
    else;

    platform_printf("ble setup...\n");

    phy_configs[0].conn_param = conn_param;
    phy_configs[1].conn_param = conn_param;
    phy_configs[2].conn_param = conn_param;

    // Note: security has not been enabled.
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    gatt_client_register_handler(&user_packet_handler);
    return 0;
}

