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
#include "btstack_mt.h"
#include "ll_api.h"
#include "bluetooth_hci.h"

#include "eatt_uart_console.h"
#include "gatt_client_util.h"
#include "btstack_sync.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "../data/gatt.const"
#include "../data/advertising.const"

#if (defined TRACE_TO_AIR)
extern trace_air_t trace_ctx;
#endif
#define CONST_MASTER    0
#define CONST_SLAVE     1
#ifndef APP_ROLE
#define APP_ROLE        CONST_MASTER
// #define APP_ROLE        CONST_SLAVE
#endif

#if(APP_ROLE == CONST_MASTER)
sm_persistent_t sm_persistent =
{
    .er = {1, 2, 3},
    .ir = {4, 5, 6},
    .identity_addr_type     = BD_ADDR_TYPE_LE_RANDOM,
    .identity_addr          = {0xA5, 0xA5, 0x5C, 0x20, 0x87, 0xA7}
};
#else
sm_persistent_t sm_persistent =
{
    .er = {1, 2, 3},
    .ir = {4, 5, 6},
    .identity_addr_type     = BD_ADDR_TYPE_LE_RANDOM,
    .identity_addr          = {0xA6, 0xA6, 0x5C, 0x20, 0x87, 0xA7}
};
#endif

#define MAX_ADVERTISERS     50
#define STORAGE_SIZE 1024*4
uint8_t num_eatt_bearers = 5;
uint8_t test_storage_buffer[STORAGE_SIZE] = {0};
uint8_t gatt_client_storage[STORAGE_SIZE] = {0};
bd_addr_t scaned_advertisers[MAX_ADVERTISERS] = {0};
int advertiser_num = 0;
static uint16_t test_value_handle;

struct gatt_client_discoverer *discoverer = NULL;
struct btstack_synced_runner *synced_runner = NULL;

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
static int bonding_flag = 1;
uint8_t peer_feature_power_control = 0;
uint8_t peer_feature_subrate = 0;
uint8_t auto_power_ctrl = 0;

static le_meta_event_vendor_connection_aborted_t aborted = {0};
uint8_t peer_addr_type = 0;
bd_addr_t peer_addr;
static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size)
{
    platform_printf("ATT read: handle = %d, size = %d\n", att_handle, buffer_size);
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
    platform_printf("ATT Write: handle = %d, size = %d\n", att_handle, buffer_size);
    switch (att_handle)
    {
    default:
        return 0;
    }
}

uint8_t slave_addr[] = {0,0,0,0,0,0};
bd_addr_type_t slave_addr_type = BD_ADDR_TYPE_LE_RANDOM;
void do_set_data()
{
    mt_gap_set_ext_adv_data(0, adv_data[ADVERTISING_ITEM_OFFSET_COMPLETE_LOCAL_NAME - 2]
                            + ADVERTISING_ITEM_OFFSET_COMPLETE_LOCAL_NAME - 1, (uint8_t *)(adv_data));
}

#define iprintf platform_printf
void discover_services(void) {
    discoverer = gatt_client_util_discover_all(mas_conn_handle, gatt_client_util_dump_profile, NULL);
}
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

#define USER_MSG_READ_CHAR          6
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
    platform_printf("user_msg_handler msg_id %d, size %d\n", msg_id, size);
    int i;
    for (i = 0; i < size; i++)
    {
        platform_printf("%02x ", ((uint8_t *)data)[i]);
    }
    platform_printf("\n");
    switch (msg_id)
    {
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
    }

}


void start_adv()
{
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
    gap_set_ext_adv_data(0, sizeof(adv_data), (uint8_t*)adv_data);
    gap_set_ext_scan_response_data(0, sizeof(scan_data), (uint8_t*)scan_data);
    mt_gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
    printf("adv started\n");
}
void demo_synced_create_conn(struct btstack_synced_runner *runner, void *_)
{
    printf("synced create connection (timeout 5s)...");
    le_meta_event_enh_create_conn_complete_t complete = {0};
    int r = gap_sync_ext_create_connection(synced_runner,
                                    INITIATING_ADVERTISER_FROM_PARAM,
                                    BD_ADDR_TYPE_LE_RANDOM,           // Own_Address_Type,
                                    slave_addr_type,                  // Peer_Address_Type,
                                    slave_addr,                       // Peer_Address,
                                    sizeof(phy_configs) / sizeof(phy_configs[0]),
                                    phy_configs,
                                    5000,
                                    &complete);
    printf(r == 0 ? "Succeed\n" : "Failed\n");
}

void sync_conn_to_slave()
{
    peer_feature_power_control = 0;
    peer_feature_subrate = 0;
    btstack_sync_run(synced_runner, demo_synced_create_conn, NULL);
}


void read_value_of_char(int handle)
{
    btstack_push_user_msg(USER_MSG_READ_CHAR, NULL, (uint16_t)handle);
}

void gatt_event_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size) {
    platform_printf("my_gatt_callback: packet_type=%d, channel=0x%04X, size=%d\n", packet_type, channel, size);
}

void start_enhanced_gatt(hci_con_handle_t con_handle){
    uint8_t status = gatt_client_le_enhanced_connect(
        gatt_event_handler,
        con_handle,
        num_eatt_bearers,
        gatt_client_storage,
        sizeof(gatt_client_storage)
    );

    if (status != 0){
        printf("Enhanced GATT connect failed: %d\n", status);
    } else {
        printf("Enhanced GATT connect started\n");
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
            synced_runner = btstack_create_sync_runner(1);
            
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
                gap_read_white_lists_size();
#if (APP_ROLE == CONST_SLAVE)
            start_adv();
#endif
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {

        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:
            {
                const le_meta_event_enh_create_conn_complete_t * complete =
                    decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);

                if (complete->status != 0) break;

                peer_addr_type = complete->peer_addr_type;
                memcpy(peer_addr, complete->peer_addr, sizeof(peer_addr));

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
                gatt_client_is_ready(complete->handle);
                sm_request_pairing(mas_conn_handle);
            }
            break;
        default:
            break;
        }

        break;
    case HCI_EVENT_DISCONNECTION_COMPLETE:
        {
            const event_disconn_complete_t *complete = decode_hci_event_disconn_complete(packet);
            iprintf("disconnected\n");
            if (complete->conn_handle == sla_conn_handle)
            {
                sla_conn_handle = INVALID_HANDLE;
                start_adv();
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
        platform_printf("authourized\n");
        platform_printf("discovering...\n");
        discoverer = gatt_client_util_discover_all(mas_conn_handle, gatt_client_util_dump_profile, NULL);
        break;
    case SM_EVENT_STATE_CHANGED:
        {
            const sm_event_state_changed_t *state_changed = decode_hci_event(packet, sm_event_state_changed_t);
            switch (state_changed->reason)
            {
                case SM_STARTED:
                    platform_printf("SM: STARTED\n");
                    break;
                case SM_FINAL_PAIRED:
                    platform_printf("SM: PAIRED\n");
                    #if APP_ROLE == CONST_MASTER
                    start_enhanced_gatt(state_changed->conn_handle);
                    #endif
                    // start_detect_peer_os_type(synced_runner, state_changed->conn_handle);
                    break;
                case SM_FINAL_REESTABLISHED:
                    platform_printf("SM: REESTABLISHED\n");
                    break;
                default:
                    platform_printf("SM: FINAL ERROR: %d\n", state_changed->reason);
                    break;
            }
        }

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
    platform_printf("role: %d\n", APP_ROLE);
#if (APP_ROLE == CONST_SLAVE)
    int ret = att_server_eatt_init(num_eatt_bearers, test_storage_buffer, STORAGE_SIZE);
    platform_printf("att_server_eatt_init: %d\n", ret);
#endif
    return 0;
}
