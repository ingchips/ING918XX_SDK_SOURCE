#include "platform_api.h"
#include "profile.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_util.h"
#include "btstack_defines.h"
#include "gatt_client.h"
#include "l2cap.h"
#include "sig_uuid.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "str_util.h"
#include "sm.h"
#include "trace.h"

#include "uart_console.h"
#include "gatt_client_util.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "../data/gatt.const"
#include "../data/advertising.const"

#if (defined TRACE_TO_AIR)
extern trace_air_t trace_ctx;
#endif

#define RX_GOLDEN_RAGE_MIN (-75)
#define RX_GOLDEN_RAGE_MAX (-50)

#define OPCODE(ogf, ocf)            (ocf | ogf << 10)
#define OPCODE_READ_RSSI            OPCODE(OGF_STATUS_PARAMETERS, 0x05)

sm_persistent_t sm_persistent =
{
    .er = {1, 2, 3},
    .ir = {4, 5, 6},
    .identity_addr_type     = BD_ADDR_TYPE_LE_RANDOM,
    .identity_addr          = {0xC6, 0xFA, 0x5C, 0x20, 0x87, 0xA7}
};

#define MAX_ADVERTISERS     50
bd_addr_t scaned_advertisers[MAX_ADVERTISERS] = {0};
int advertiser_num = 0;
int is_targeted_scan = 0;
uint64_t last_seen = 0;

struct gatt_client_discoverer *discoverer = NULL;
struct gatt_client_synced_runner *synced_runner = NULL;

#define find_char(handle)   gatt_client_util_find_char(discoverer, handle)
#define find_config_desc    gatt_client_util_find_config_desc

// GATT characteristic handles
#include "../data/gatt.const"

static uint8_t adv_data[31] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

const static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

#define INVALID_HANDLE  0xffff
uint16_t mas_conn_handle = INVALID_HANDLE;
uint16_t sla_conn_handle = INVALID_HANDLE;
static int bonding_flag = 0;
uint8_t peer_feature_power_control = 0;
uint8_t peer_feature_subrate = 0;
uint8_t auto_power_ctrl = 0;

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {

    default:
        return 0;
    }
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {

    default:
        return 0;
    }
}

uint8_t slave_addr[] = {0,0,0,0,0,0};
bd_addr_type_t slave_addr_type = BD_ADDR_TYPE_LE_RANDOM;

extern int8_t adv_tx_power;

void do_set_data()
{
    gap_set_ext_adv_data(0, adv_data[ADVERTISING_ITEM_OFFSET_COMPLETE_LOCAL_NAME - 2]
                            + ADVERTISING_ITEM_OFFSET_COMPLETE_LOCAL_NAME - 1, (uint8_t *)(adv_data));
}

#define iprintf platform_printf

void print_fun(const char *s)
{
    printf("%s\n", s);
}

void read_characteristic_value_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT:
        {
            uint16_t value_size;
            const gatt_event_value_packet_t *value =
                gatt_event_characteristic_value_query_result_parse(packet, size, &value_size);
            if (value_size)
            {
                iprintf("VALUE of %d:\n", value->handle);
                print_hex_table(value->value, value_size, print_fun);
            }
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        iprintf("read_characteristic_value COMPLETE: %d\n", gatt_event_query_complete_parse(packet)->status);
        if (gatt_event_query_complete_parse(packet)->status != 0)
        {
            gap_disconnect(0);
            break;
        }
        break;
    }
}

void write_characteristic_value_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_QUERY_COMPLETE:
        iprintf("write_characteristic_value COMPLETE: %d\n", gatt_event_query_complete_parse(packet)->status);
        if (gatt_event_query_complete_parse(packet)->status != 0)
        {
            gap_disconnect(0);
            break;
        }
        break;
    }
}

void write_characteristic_descriptor_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_QUERY_COMPLETE:
        iprintf("write_characteristic_descriptor COMPLETE: %d\n", gatt_event_query_complete_parse(packet)->status);
        if (gatt_event_query_complete_parse(packet)->status != 0)
        {
            gap_disconnect(0);
            break;
        }
        break;
    }
}

static void output_notification_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const gatt_event_value_packet_t *value;
    uint16_t value_size;
    switch (packet[0])
    {
    case GATT_EVENT_NOTIFICATION:
        value = gatt_event_notification_parse(packet, size, &value_size);
        if (value_size)
        {
            iprintf("NOTIFACATION of %d:\n", value->handle);
            print_hex_table(value->value, value_size, print_fun);
        }
        break;
    case GATT_EVENT_INDICATION:
        value = gatt_event_indication_parse(packet, size, &value_size);
        if (value_size)
        {
            iprintf("INDICATION of %d:\n", value->handle);
            print_hex_table(value->value, value_size, print_fun);
        }
        break;
    }
}

static void demo_synced_api(void *user_data)
{
    uint16_t handle = (uint16_t)(uintptr_t)user_data;
    static uint8_t data[255];
    int n = 5;
    iprintf("synced read value for %d times:\n", n);

    for (; n > 0; n--)
    {
        uint16_t length = sizeof(data);
        int err = gatt_client_sync_read_value_of_characteristic(synced_runner,
                mas_conn_handle, handle,
                data,
                &length);
        iprintf("[%d]: err = %d:\n", n, err);
        if (err) break;
        print_hex_table(data, length, print_fun);
        iprintf("wait for 200ms...\n", n, err);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    iprintf("done\n\n");
}

#define USER_MSG_START_ADV          0
#define USER_MSG_STOP_ADV           1
#define USER_MSG_UPDATE_ADV_DATA    2
#define USER_MSG_UPDATE_ADDR        3
#define USER_MSG_CONN_TO_SLAVE      4
#define USER_MSG_CONN_CANCEL        5
#define USER_MSG_READ_CHAR          6
#define USER_MSG_WRITE_CHAR         7
#define USER_MSG_WOR_CHAR           8
#define USER_MSG_SUB_TO_CHAR        9
#define USER_MSG_UNSUB_TO_CHAR      10
#define USER_MSG_SET_BONDING        11
#define USER_MSG_START_SCAN_ADDR    12
#define USER_MSG_START_SCAN_ALL     13
#define USER_MSG_SET_PHY            14
#define USER_MSG_CHANGE_CONN_PARAM      15
#define USER_MSG_START_SCAN_OLD_ADDR    16
#define USER_MSG_START_SCAN_OLD_ALL     17
#define USER_MSG_READ_RSSI              18
#define USER_MSG_SUBRATE_REQ            19
#define USER_MSG_SYNC_READ_CHAR         20
#define USER_MSG_SYNC_MSG_START         0x10000

const static ext_adv_set_en_t adv_sets_en[] = {{.handle = 0, .duration = 0, .max_events = 0}};

#define CONN_PARAM  {                   \
            .scan_int = 200,            \
            .scan_win = 100,            \
            .interval_min = 50,         \
            .interval_max = 50,         \
            .latency = 0,               \
            .supervision_timeout = 600, \
            .min_ce_len = 90,           \
            .max_ce_len = 90            \
    }

static initiating_phy_config_t phy_configs[] =
{
    {
        .phy = PHY_1M,
        .conn_param = CONN_PARAM
    },
    {
        .phy = PHY_2M,
        .conn_param = CONN_PARAM
    },
    {
        .phy = PHY_CODED,
        .conn_param = CONN_PARAM
    }
};

static scan_phy_config_t scan_configs[] =
{
    {
        .phy = PHY_1M,
        .type = SCAN_PASSIVE,
        .interval = 200,
        .window = 50
    },
    {
        .phy = PHY_CODED,
        .type = SCAN_PASSIVE,
        .interval = 200,
        .window = 50
    }
};

static const scan_phy_config_t scan_configs_lagecy[] =
{
    {
        .phy = PHY_1M,
        .type = SCAN_ACTIVE,
        .interval = 200,
        .window = 200
    }
};

struct
{
    uint16_t interval;
    uint16_t latency;
    uint16_t timeout;
} conn_param_requst = {0};

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MSG_START_ADV:
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        printf("adv started\n");
        break;
    case USER_MSG_STOP_ADV:
        gap_set_ext_adv_enable(0, 0, NULL);
        printf("adv stopped\n");
        break;
    case USER_MSG_UPDATE_ADV_DATA:
        do_set_data();
        printf("adv updated\n");
        break;
    case USER_MSG_UPDATE_ADDR:
        gap_set_adv_set_random_addr(0, sm_persistent.identity_addr);
        printf("addr changed: %02X:%02X:%02X:%02X:%02X:%02X\n",
                sm_persistent.identity_addr[0], sm_persistent.identity_addr[1],
                sm_persistent.identity_addr[2], sm_persistent.identity_addr[3],
                sm_persistent.identity_addr[4], sm_persistent.identity_addr[5]);
        break;
    case USER_MSG_CONN_TO_SLAVE:
        printf("create connection...\n");
        peer_feature_power_control = 0;
        peer_feature_subrate = 0;
        gap_ext_create_connection(INITIATING_ADVERTISER_FROM_PARAM,
                                                  BD_ADDR_TYPE_LE_RANDOM,           // Own_Address_Type,
                                                  slave_addr_type,                  // Peer_Address_Type,
                                                  slave_addr,                       // Peer_Address,
                                                  sizeof(phy_configs) / sizeof(phy_configs[0]),
                                                  phy_configs);
        break;
    case USER_MSG_CONN_CANCEL:
        printf("create connection cancelled.\n");
        gap_create_connection_cancel();
        break;
    case USER_MSG_READ_CHAR:
        {
            char_node_t *c = find_char(size);
            if (NULL == c)
            {
                iprintf("CHAR not found: %d\n", size);
                break;
            }

            gatt_client_read_value_of_characteristic_using_value_handle(
                read_characteristic_value_callback,
                mas_conn_handle,
                c->chara.value_handle);
        }
        break;
    case USER_MSG_SYNC_READ_CHAR:
        {
            char_node_t *c = find_char(size);
            if (NULL == c)
            {
                iprintf("CHAR not found: %d\n", size);
                break;
            }

            gatt_client_sync_run(synced_runner, demo_synced_api, (void *)(uintptr_t)c->chara.value_handle);
        }
        break;
    case USER_MSG_WRITE_CHAR:
        {
            char_node_t *c = find_char(size);
            block_value_t *v = (block_value_t *)(data);
            if (NULL == c)
            {
                iprintf("CHAR not found: %d\n", size);
                break;
            }

            gatt_client_write_value_of_characteristic(
                write_characteristic_value_callback,
                mas_conn_handle,
                c->chara.value_handle,
                v->len,
                v->value);
        }
        break;
    case USER_MSG_WOR_CHAR:
        {
            char_node_t *c = find_char(size);
            block_value_t *v = (block_value_t *)(data);
            if (NULL == c)
            {
                iprintf("CHAR not found: %d\n", size);
                break;
            }

            gatt_client_write_value_of_characteristic_without_response(
                mas_conn_handle,
                c->chara.value_handle,
                v->len,
                v->value);
        }
        break;
    case USER_MSG_SUB_TO_CHAR:
        {
            uint16_t config = GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION;
            char_node_t *c = find_char(size);
            desc_node_t *d = find_config_desc(c);
            if (NULL == d)
            {
                iprintf("CHAR/DESC not found: %d\n", size);
                break;
            }

            if (c->notification == NULL)
            {
                c->notification = (gatt_client_notification_t *)malloc(sizeof(gatt_client_notification_t));
                gatt_client_listen_for_characteristic_value_updates(
                    c->notification, output_notification_handler,
                    mas_conn_handle, c->chara.value_handle);
            }

            gatt_client_write_characteristic_descriptor_using_descriptor_handle(
                write_characteristic_descriptor_callback,
                mas_conn_handle,
                d->desc.handle,
                sizeof(config),
                (uint8_t *)&config);
        }
        break;
    case USER_MSG_UNSUB_TO_CHAR:
        {
            uint16_t config = 0;
            char_node_t *c = find_char(size);
            desc_node_t *d = find_config_desc(c);
            if (NULL == d)
            {
                iprintf("CHAR/DESC not found: %d\n", size);
                break;
            }

            gatt_client_write_characteristic_descriptor_using_descriptor_handle(
                write_characteristic_descriptor_callback,
                mas_conn_handle,
                d->desc.handle,
                sizeof(config),
                (uint8_t *)&config);
        }
        break;
    case USER_MSG_SET_BONDING:
        bonding_flag = size;
        if (bonding_flag)
            sm_set_authentication_requirements(SM_AUTHREQ_BONDING);
        else
            sm_set_authentication_requirements(SM_AUTHREQ_NO_BONDING);
        break;
    case USER_MSG_START_SCAN_ALL:
        advertiser_num = 0;
        gap_set_ext_scan_para(BD_ADDR_TYPE_LE_RANDOM, SCAN_ACCEPT_ALL_EXCEPT_NOT_DIRECTED,
                              sizeof(scan_configs) / sizeof(scan_configs[0]),
                              scan_configs);
        gap_set_ext_scan_enable(1, 0, 0, 0);   // start continuous scanning
        break;
    case USER_MSG_START_SCAN_ADDR:
        is_targeted_scan = 1;
        last_seen = 0;
        advertiser_num = 0;
        gap_clear_white_lists();
        gap_add_whitelist(slave_addr, slave_addr_type);
        gap_set_ext_scan_para(BD_ADDR_TYPE_LE_RANDOM, SCAN_ACCEPT_WLIST_EXCEPT_NOT_DIRECTED,
                              sizeof(scan_configs) / sizeof(scan_configs[0]),
                              scan_configs);
        gap_set_ext_scan_enable(1, 0, 0, 0);   // start continuous scanning
        break;
    case USER_MSG_START_SCAN_OLD_ALL:
        advertiser_num = 0;
        gap_set_ext_scan_para(BD_ADDR_TYPE_LE_RANDOM, SCAN_ACCEPT_ALL_EXCEPT_NOT_DIRECTED,
                              sizeof(scan_configs_lagecy) / sizeof(scan_configs_lagecy[0]),
                              scan_configs_lagecy);
        gap_set_ext_scan_enable(1, 0, 0, 0);   // start continuous scanning
        break;
    case USER_MSG_START_SCAN_OLD_ADDR:
        is_targeted_scan = 1;
        last_seen = 0;
        advertiser_num = 0;
        gap_clear_white_lists();
        gap_add_whitelist(slave_addr, slave_addr_type);
        gap_set_ext_scan_para(BD_ADDR_TYPE_LE_RANDOM, SCAN_ACCEPT_WLIST_EXCEPT_NOT_DIRECTED,
                              sizeof(scan_configs_lagecy) / sizeof(scan_configs_lagecy[0]),
                              scan_configs_lagecy);
        gap_set_ext_scan_enable(1, 0, 0, 0);   // start continuous scanning
        break;
    case USER_MSG_SET_PHY:
        {
            uint16_t       phy = size;
            phy_bittypes_t phy_bit;
            phy_option_t   phy_opt = HOST_PREFER_S8_CODING;
            switch (phy)
            {
            case 0:
                phy_bit = PHY_1M_BIT;
                break;
            case 1:
                phy_bit = PHY_2M_BIT;
                break;
            case 2:
                phy_opt = HOST_PREFER_S2_CODING; // fall through
            case 3:
                phy_bit = PHY_CODED_BIT;
                break;
            }
            gap_set_phy(mas_conn_handle, 0, phy_bit, phy_bit, phy_opt);
        }
        break;
    case USER_MSG_CHANGE_CONN_PARAM:
        {
            uint16_t interval = conn_param_requst.interval;
            if (sla_conn_handle != INVALID_HANDLE)
            {
                l2cap_request_connection_parameter_update(sla_conn_handle,
                        interval, interval,
                        conn_param_requst.latency,
                        conn_param_requst.timeout);
            }
            if (mas_conn_handle != INVALID_HANDLE)
            {
                uint16_t ce_len = (interval << 1) - 2;
                gap_update_connection_parameters(mas_conn_handle,
                        interval, interval,
                        conn_param_requst.latency,
                        conn_param_requst.timeout,
                        ce_len, ce_len);
            }
        }
        break;
#if (defined TRACE_TO_AIR)
    case USER_MSG_ID_TRACE:
        trace_air_send(&trace_ctx);
        break;
#endif
    case USER_MSG_READ_RSSI:
        if (mas_conn_handle != INVALID_HANDLE)
            gap_read_rssi(mas_conn_handle);
        if (sla_conn_handle != INVALID_HANDLE)
            gap_read_rssi(sla_conn_handle);
        break;
    case USER_MSG_SUBRATE_REQ:
        gap_subrate_request(0, size, size, 1,
                            0, 2000);
        break;
    default:
        if (msg_id >= USER_MSG_SYNC_MSG_START)
        {
            struct gatt_client_synced_runner *runner = (struct gatt_client_synced_runner *)data;
            gatt_client_sync_handle_msg(runner, msg_id - USER_MSG_SYNC_MSG_START);
        }
    }
}

void set_adv_local_name(const char *name, int16_t len)
{
    adv_data[0] = len + 1;
    adv_data[1] = 0x9;              // Complete Local Name
    memcpy(adv_data + 2, name, len);
    btstack_push_user_msg(USER_MSG_UPDATE_ADV_DATA, NULL, 0);
}

void set_bonding(int flag)
{
    btstack_push_user_msg(USER_MSG_SET_BONDING, NULL, flag);
}

void start_adv()
{
    btstack_push_user_msg(USER_MSG_START_ADV, NULL, 0);
}

void stop_adv()
{
    btstack_push_user_msg(USER_MSG_STOP_ADV, NULL, 0);
}

void update_addr()
{
    btstack_push_user_msg(USER_MSG_UPDATE_ADDR, NULL, 0);
}

void conn_to_slave()
{
    btstack_push_user_msg(USER_MSG_CONN_TO_SLAVE, NULL, 0);
}

void start_scan(int targeted)
{
    btstack_push_user_msg(targeted ? USER_MSG_START_SCAN_ADDR : USER_MSG_START_SCAN_ALL, NULL, 0);
}

void start_scan_legacy(int targeted)
{
    btstack_push_user_msg(targeted ? USER_MSG_START_SCAN_OLD_ADDR : USER_MSG_START_SCAN_OLD_ALL, NULL, 0);
}

void cancel_create_conn()
{
    btstack_push_user_msg(USER_MSG_CONN_CANCEL, NULL, 0);
}

void read_value_of_char(int handle)
{
    btstack_push_user_msg(USER_MSG_READ_CHAR, NULL, (uint16_t)handle);
}

void sync_read_value_of_char(int handle)
{
    btstack_push_user_msg(USER_MSG_SYNC_READ_CHAR, NULL, (uint16_t)handle);
}

void write_value_of_char(int handle, block_value_t *value)
{
    btstack_push_user_msg(USER_MSG_WRITE_CHAR, value, (uint16_t)handle);
}

void wor_value_of_char(int handle, block_value_t *value)
{
    btstack_push_user_msg(USER_MSG_WOR_CHAR, value, (uint16_t)handle);
}

void sub_to_char(int handle)
{
    btstack_push_user_msg(USER_MSG_SUB_TO_CHAR, NULL, (uint16_t)handle);
}

void unsub_to_char(int handle)
{
    btstack_push_user_msg(USER_MSG_UNSUB_TO_CHAR, NULL, (uint16_t)handle);
}

void set_phy(int phy)
{
    btstack_push_user_msg(USER_MSG_SET_PHY, NULL, phy);
}

void ble_set_conn_power(int level)
{
    ll_set_conn_tx_power(0, (int16_t)level);
}

void ble_adjust_peer_tx_power(int delta)
{
    if (peer_feature_power_control)
    {
        ll_adjust_conn_peer_tx_power(0, (int8_t)delta);
    }
    else
        iprintf("ERROR: peer does not support power control\n");
}

void ble_set_conn_subrate(int factor)
{
    if (peer_feature_subrate)
    {
        btstack_push_user_msg(USER_MSG_SUBRATE_REQ, NULL, factor);
    }
    else
        iprintf("ERROR: peer does not support power control\n");
}

void ble_set_auto_power_control(int enable)
{
    auto_power_ctrl = enable;
    iprintf("Auto power control %s.\n", enable ? "enabled" : "disabled");
}

void ble_read_rssi(void)
{
    btstack_push_user_msg(USER_MSG_READ_RSSI, NULL, 0);
}

void change_conn_param(int interval, int latency, int timeout)
{
    conn_param_requst.interval = interval >= 6 ? interval : 6;
    conn_param_requst.latency = latency;
    conn_param_requst.timeout = timeout > 0 ? timeout :
                    (conn_param_requst.interval > 10 ? conn_param_requst.interval * (latency + 1) : 10);
    btstack_push_user_msg(USER_MSG_CHANGE_CONN_PARAM, NULL, 0);
}

void synced_push_user_msg(struct gatt_client_synced_runner *runner, uint8_t msg_id)
{
    btstack_push_user_msg(USER_MSG_SYNC_MSG_START + msg_id, runner, 0);
}

int is_new_advertiser(const uint8_t *addr)
{
    int i;
    for (i = 0; i < advertiser_num; i++)
    {
        if (memcmp(scaned_advertisers[i], addr, BD_ADDR_LEN) == 0)
            return 0;
    }
    if (i >= MAX_ADVERTISERS) return 0;
    memcpy(scaned_advertisers[i], addr, BD_ADDR_LEN);
    advertiser_num++;
    return 1;
}

#pragma pack (push, 1)
typedef struct read_remote_version
{
    uint8_t Status;
    uint16_t Connection_Handle;
    uint8_t Version;
    uint16_t Manufacturer_Name;
    uint16_t Subversion;
} read_remote_version_t;
#pragma pack (pop)

static void check_and_print_features(const le_meta_event_read_remote_feature_complete_t * complete)
{
    static const char features[][48] =
    {
        "LE Encryption",
        "Connection Parameters Request",
        "Extended Reject Indication",
        "Slave-initiated Features Exchange",
        "LE Ping",
        "LE Data Packet Length Extension",
        "LL Privacy",
        "Extended Scanner Filter Policies",
        "LE 2M PHY",
        "Stable Modulation Index - Transmitter",
        "Stable Modulation Index - Receiver",
        "LE Coded PHY",
        "LE Extended Advertising",
        "LE Periodic Advertising",
        "Channel Selection Algorithm #2",
        "LE Power Class 1",
        "Minimum Number of Used Channels Procedure",
        "Connection CTE Request",
        "Connection CTE Response",
        "Connectionless CTE Transmitter",
        "Connectionless CTE Receiver",
        "Antenna Switching During CTE Transmission (AoD)",
        "Antenna Switching During CTE Reception (AoA)",
        "Receiving Constant Tone Extensions",
        "Periodic Advertising Sync Transfer - Sender",
        "Periodic Advertising Sync Transfer - Recipient",
        "Sleep Clock Accuracy Updates",
        "Remote Public Key Validation",
        "Connected Isochronous Stream - Master",
        "Connected Isochronous Stream - Slave",
        "Isochronous Broadcaster",
        "Synchronized Receiver",
        "Isochronous Channels (Host Support)",
        "LE Power Control Request",
        "LE Power Control Request",
        "LE Path Loss Monitoring",
        "Periodic Advertising ADI",
        "Connection Subrating",
        "Connection Subrating (Host Support)",
        "Channel Classification",
    };
    int i;
    iprintf("Features of peer #%d (status %d)\n", complete->handle, complete->status);
    for (i = 0; i < sizeof(features) / sizeof(features[0]); i++)
    {
        int B_i = i >> 3;
        int b_i = i & 0x7;
        iprintf("[%c] %s\n", complete->features[B_i] & (1 << b_i) ? '*' : ' ', features[i]);
    }
    iprintf("\n");
    peer_feature_power_control = (complete->features[4] & 0x6) == 0x6;
    peer_feature_subrate = (complete->features[4] & 0x60) == 0x60;
    iprintf("peer_feature_power_control = %d\n", peer_feature_power_control);
    iprintf("peer_feature_subrate = %d\n", peer_feature_subrate);
}

static const char *decode_version(int ver)
{
    switch (ver)
    {
        case 6: return "4.0";
        case 7:  return "4.1";
        case 8:  return "4.2";
        case 9:  return "5.0";
        case 10: return "5.1";
        case 11: return "5.2";
        case 12: return "5.3";
        case 13: return "5.4";
        case 14: return "5.5";
        default: return "??";
    }
}

static const char *decode_zone(le_path_loss_zone_event_t zone_entered)
{
    switch (zone_entered)
    {
    case PATH_LOSS_ZONE_ENTER_LOW:
        return "LOW";
    case PATH_LOSS_ZONE_ENTER_MIDDLE:
        return "MIDDLE";
    default:
        return "HIGH";
    }
}

static const char *decode_unified_phy(unified_phy_type_t phy)
{
    static const char phys[4][5] = {"1M", "2M", "S8", "S2"};
    return phys[phy];
}

static const char *decode_tx_power_reason(le_tx_power_reporting_reason_t reason)
{
    switch (reason)
    {
    case TX_POWER_REPORTING_REASON_LOCAL_CHANGED:
        return "LOCAL CHANGED";
    case TX_POWER_REPORTING_REASON_REMOTE_CHANGED:
        return "REMOTE CHANGED";
    default:
        return "HCI COMPLETE";
    }
}

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;

        synced_runner = gatt_client_create_sync_runner(synced_push_user_msg);

        platform_config(PLATFORM_CFG_LL_LEGACY_ADV_INTERVAL, 1500);
        ll_set_tx_power_range(-30, 10);

        gap_set_random_device_address(sm_persistent.identity_addr);
        gap_set_adv_set_random_addr(0, sm_persistent.identity_addr);
        gap_set_ext_adv_para(0,
                                CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT,
                                0x00a1, 0x00a1,            // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
                                PRIMARY_ADV_ALL_CHANNELS,  // Primary_Advertising_Channel_Map
                                BD_ADDR_TYPE_LE_RANDOM,    // Own_Address_Type
                                BD_ADDR_TYPE_LE_PUBLIC,    // Peer_Address_Type (ignore)
                                NULL,                      // Peer_Address      (ignore)
                                ADV_FILTER_ALLOW_ALL,      // Advertising_Filter_Policy
                                0x00,                      // Advertising_Tx_Power
                                PHY_1M,                    // Primary_Advertising_PHY
                                0,                         // Secondary_Advertising_Max_Skip
                                PHY_1M,                    // Secondary_Advertising_PHY
                                0x00,                      // Advertising_SID
                                0x00);                     // Scan_Request_Notification_Enable
        do_set_data();
        gap_set_ext_scan_response_data(0, sizeof(scan_data), (uint8_t*)scan_data);
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT:
            {
                const le_ext_adv_report_t *report = decode_hci_le_meta_event(packet, le_meta_event_ext_adv_report_t)->reports;
                if (is_targeted_scan)
                {
                    if ((report->evt_type & HCI_EXT_ADV_PROP_SCAN_RSP) == 0)
                    {
                        uint64_t now = platform_get_us_time();
                        if (last_seen != 0)
                            platform_printf("Interval: %d ms\n", (int)((now - last_seen) / 1000));
                        last_seen = now;
                    }

                    platform_printf("ADV %02X:%02X:%02X:%02X:%02X:%02X (%s) %ddBm\n"
                                "Type: 0x%02x\n",
                                report->address[5], report->address[4], report->address[3],
                                report->address[2], report->address[1], report->address[0],
                                report->addr_type ? "RANDOM" : "PUBLIC",
                                report->rssi, report->evt_type);
                }
                else
                {
                    if (!is_new_advertiser(report->address)) break;
                    platform_printf("No. %d:\n"
                                "ADV %02X:%02X:%02X:%02X:%02X:%02X (%s) %ddBm\n"
                                "Type: 0x%02x\n",
                                advertiser_num,
                                report->address[5], report->address[4], report->address[3],
                                report->address[2], report->address[1], report->address[0],
                                report->addr_type ? "RANDOM" : "PUBLIC",
                                report->rssi, report->evt_type);
                }


                print_hex_table(report->data, report->data_len, print_fun);
                platform_printf("\n");
            }
            break;
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            {
                const le_meta_event_enh_create_conn_complete_t * complete =
                    decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);

                if (complete->status != 0) break;

                if (HCI_ROLE_SLAVE == complete->role)
                {
                    sla_conn_handle = complete->handle;
                    att_set_db(sla_conn_handle, profile_data);
#if (defined TRACE_TO_AIR)
                    trace_air_enable(&trace_ctx, 1, sla_conn_handle, HANDLE_TRACE_DATA);
#endif
                }
                else
                {
                    mas_conn_handle = complete->handle;
                }


                gap_read_remote_info(complete->handle);
            }
            break;
        case HCI_SUBEVENT_LE_READ_REMOTE_USED_FEATURES_COMPLETE:
            {
                const le_meta_event_read_remote_feature_complete_t * complete =
                    decode_hci_le_meta_event(packet, le_meta_event_read_remote_feature_complete_t);
                check_and_print_features(complete);

                gap_set_path_loss_reporting_param(complete->handle,
                                                  60, 8,
                                                  30, 8, 50);
                gap_set_path_loss_reporting_enable(complete->handle, 1);
                gap_set_tx_power_reporting_enable(complete->handle, 1,
                                                  peer_feature_power_control);

                if (complete->handle == mas_conn_handle)
                {
                    if (0 == bonding_flag)
                    {
                        iprintf("discovering...\n");
                        discoverer = gatt_client_util_discover_all(mas_conn_handle, gatt_client_util_dump_profile, NULL);
                    }
                    else
                        sm_request_pairing(mas_conn_handle);
                }
            }
            break;
        case HCI_SUBEVENT_LE_PHY_UPDATE_COMPLETE:
            {
                const le_meta_phy_update_complete_t *cmpl = decode_hci_le_meta_event(packet, le_meta_phy_update_complete_t);
                platform_printf("PHY updated: Rx %d, Tx %d\n", cmpl->rx_phy, cmpl->tx_phy);
            }
            break;
        case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE:
            {
                const le_meta_event_conn_update_complete_t *cmpl = decode_hci_le_meta_event(packet, le_meta_event_conn_update_complete_t);
                platform_printf("CONN updated: interval %.2f ms\n", cmpl->interval * 1.25);
            }
            break;
        case HCI_SUBEVENT_LE_PATH_LOSS_THRESHOLD:
            {
                const le_meta_path_loss_threshold_t *rpt = decode_hci_le_meta_event(packet, le_meta_path_loss_threshold_t);

                platform_printf("PATH_LOSS_THRESHOLD:\n"
                                "current_path_loss = %d dB\n"
                                "zone_entered      = %s\n\n",
                                rpt->current_path_loss,
                                decode_zone(rpt->zone_entered));
                if (auto_power_ctrl)
                    gap_read_rssi(rpt->conn_handle);
            }
            break;
        case HCI_SUBEVENT_LE_TRANSMIT_POWER_REPORTING:
            {
                const le_meta_tx_power_reporting_t *rpt = decode_hci_le_meta_event(packet, le_meta_tx_power_reporting_t);
                platform_printf("TRANSMIT_POWER_REPORTING:\n"
                                "status                 = %d\n"
                                "reason                 = %s\n"
                                "phy                    = %s\n"
                                "tx_power_level         = %d dBm\n"
                                "tx_power_level_flag    = %d\n"
                                "delta                  = %d dB\n\n",
                                rpt->status,
                                decode_tx_power_reason(rpt->reason),
                                decode_unified_phy(rpt->phy),
                                rpt->tx_power_level,
                                rpt->tx_power_level_flag,
                                rpt->delta
                                );

            }
            break;
        case HCI_SUBEVENT_LE_SUBRATE_CHANGE:
            {
                const le_meta_subrate_change_t *rpt = decode_hci_le_meta_event(packet, le_meta_subrate_change_t);
                platform_printf("SUBRATE_CHANGE:\n"
                                "status             = %d\n"
                                "subrate_factor     = %d\n"
                                "peripheral_latency = %d\n"
                                "continuation_number= %d\n"
                                "supervision_timeou = %d\n\n",
                                rpt->status,
                                rpt->subrate_factor,
                                rpt->peripheral_latency,
                                rpt->continuation_number,
                                rpt->supervision_timeout
                                );
            }
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_ENCRYPTION_CHANGE:
        {
            const hci_encryption_change_event_t *complete =
                decode_hci_event(packet, hci_encryption_change_event_t);
            if (complete->conn_handle == mas_conn_handle)
            {
                iprintf("discovering...\n");
                discoverer = gatt_client_util_discover_all(mas_conn_handle, gatt_client_util_dump_profile, NULL);
            }
        }
        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        {
            const event_disconn_complete_t *complete = decode_hci_event_disconn_complete(packet);
            iprintf("disconnected\n");
            if (complete->conn_handle == sla_conn_handle)
            {
                sla_conn_handle = INVALID_HANDLE;
#if (defined TRACE_TO_AIR)
                trace_air_enable(&trace_ctx, 0, 0, 0);
                start_adv();
#endif
            }
            if (complete->conn_handle == mas_conn_handle)
            {
                mas_conn_handle = INVALID_HANDLE;
                if (discoverer)
                {
                    gatt_client_util_free(discoverer);
                    discoverer = NULL;
                }
            }
        }
        break;

    case HCI_EVENT_COMMAND_COMPLETE:
        {
            const event_command_complete_return_param_read_rssi_t *
                read_rssi;
            const uint8_t *returns = hci_event_command_complete_get_return_parameters(packet);
            if (*returns != 0)
            {
                platform_printf("COMMAND_COMPLETE: 0x%02x for OPCODE %04X\n",
                    *returns, hci_event_command_complete_get_command_opcode(packet));
                break;
            }

            switch (hci_event_command_complete_get_command_opcode(packet))
            {
            case OPCODE_READ_RSSI:
                read_rssi = (const event_command_complete_return_param_read_rssi_t *)returns;
                iprintf("RSSI: %d dBm", read_rssi->rssi);
                if (auto_power_ctrl)
                {
                    int delta = 0;
                    if (read_rssi->rssi > RX_GOLDEN_RAGE_MAX)
                        delta = RX_GOLDEN_RAGE_MAX - read_rssi->rssi;
                    else if (read_rssi->rssi < RX_GOLDEN_RAGE_MIN)
                        delta = RX_GOLDEN_RAGE_MIN - read_rssi->rssi;
                    if (delta != 0)
                        ll_adjust_conn_peer_tx_power(read_rssi->conn_handle, delta);
                }
                break;
            }
        }
        break;
    case HCI_EVENT_COMMAND_STATUS:
        {
            const uint8_t status = hci_event_command_status_get_status(packet);
            if (status != 0)
                platform_printf("COMMAND_STATUS: 0x%02x for OPCODE %04X\n",
                    status, hci_event_command_status_get_command_opcode(packet));
        }
        break;
    case HCI_EVENT_READ_REMOTE_VERSION_INFORMATION_COMPLETE:
        {
            const read_remote_version_t *version = decode_event_offset(packet, read_remote_version_t, 2);
            if (version->Status == 0)
            {
                platform_printf("Remote version\n"
                "Version          : %d (%s)\n"
                "Manufacturer Name: 0x%04X\n"
                "Subversion       : 0x%04X\n",
                version->Version, decode_version(version->Version),
                version->Manufacturer_Name, version->Subversion);
            }
            gap_read_remote_used_features(version->Connection_Handle);
        }
        break;

    case L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST:
        platform_printf("L2CAP_CONNECTION_PARAMETER_UPDATE_REQUEST:\n"
                        "handle      : %d\n"
                        "interval min: %d\n"
                        "interval max: %d\n"
                        "latency     : %d\n"
                        "timeout     : %d\n",
                        l2cap_event_connection_parameter_update_request_get_handle(packet),
                        l2cap_event_connection_parameter_update_request_get_interval_min(packet),
                        l2cap_event_connection_parameter_update_request_get_interval_max(packet),
                        l2cap_event_connection_parameter_update_request_get_latency(packet),
                        l2cap_event_connection_parameter_update_request_get_timeout_multiplier(packet));
        break;

    case L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE:
        platform_printf("L2CAP_CONNECTION_PARAMETER_UPDATE_RESPONSE:\n"
                        "handle: %d\n"
                        "result: %d\n",
                        l2cap_event_connection_parameter_update_response_get_handle(packet),
                        l2cap_event_connection_parameter_update_response_get_result(packet));
        break;

    case L2CAP_EVENT_COMMAND_REJECT_RESPONSE:
        platform_printf("L2CAP_COMMAND_REJECT_RESPONSE:\n"
                        "handle: %d\n"
                        "reason: %d\n",
                        l2cap_event_command_reject_response_get_handle(packet),
                        l2cap_event_command_reject_response_get_reason(packet));
        break;

    case GATT_EVENT_MTU:
        iprintf("GATT client MTU updated: %d\n", gatt_event_mtu_get_mtu(packet));
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

static void sm_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);

    if (packet_type != HCI_EVENT_PACKET) return;
    if (0 == bonding_flag) return;

    platform_printf("SM: %d\n", event);
    switch (event)
    {
    case SM_EVENT_JUST_WORKS_REQUEST:
        iprintf("JUST_WORKS confirmed\n");
        sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
        break;
    case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
        platform_printf("===================\npasskey: %06d\n===================\n",
            sm_event_passkey_display_number_get_passkey(packet));
        break;
    case SM_EVENT_PASSKEY_DISPLAY_CANCEL:
        platform_printf("TODO: DISPLAY_CANCEL\n");
        break;
    case SM_EVENT_PASSKEY_INPUT_NUMBER:
        // TODO: intput number
        platform_printf("===================\ninput number:\n");
        break;
    case SM_EVENT_PASSKEY_INPUT_CANCEL:
        platform_printf("TODO: INPUT_CANCEL\n");
        break;
    case SM_EVENT_IDENTITY_RESOLVING_FAILED:
        platform_printf("not authourized\n");
        break;
    case SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED:
        discoverer = gatt_client_util_discover_all(mas_conn_handle, gatt_client_util_dump_profile, NULL);
        break;
    default:
        break;
    }
}

static btstack_packet_callback_registration_t sm_event_callback_registration  = {.callback = &sm_packet_handler};

uint32_t setup_profile(void *data, void *user_data)
{
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    gatt_client_register_handler(&user_packet_handler);
    sm_add_event_handler(&sm_event_callback_registration);
    sm_config(1, IO_CAPABILITY_NO_INPUT_NO_OUTPUT,
              0,
              &sm_persistent);
    sm_add_event_handler(&sm_event_callback_registration);
    l2cap_register_packet_handler(&user_packet_handler);
    return 0;
}
