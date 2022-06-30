#include <stdio.h>
#include <string.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "att_dispatch.h"
#include "btstack_util.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "gatt_client.h"
#include "kv_storage.h"
#include "ll_api.h"
#include "ad_parser.h"
#include "sig_uuid.h"
#include "gatt_client_util.h"

#include "ant_id_mapping_4x4.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "service_console.h"

#include "profile.h"

// GATT characteristic handles
#include "../data/gatt.const"

#ifdef PRO_MODE
#define iprintf(...)
#else
#define iprintf         platform_printf
#endif

#define INVALID_HANDLE 0xffff

#define KEY_SETTINGS KV_USER_KEY_START

const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

const static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

hci_con_handle_t handle_send = INVALID_HANDLE;
static uint8_t notify_enable = 0;
struct gatt_client_discoverer *discoverer = NULL;

extern void set_sample_offset(int n);

int current_pattern = -1;
hci_con_handle_t conn_handle = INVALID_HANDLE;
settings_t *settings = NULL;

uint8_t scanner_configured = 0;

char *base64_encode(const uint8_t *data, int data_len,
                    char *res, int buffer_size);

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {
    case HANDLE_GENERIC_OUTPUT_CLIENT_CHAR_CONFIG:
        {
            uint8_t buffer[2];
            if (notify_enable == 0){
                buffer[0] = ((uint16_t)GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NONE) >> 0;
                buffer[1] = ((uint16_t)GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NONE) >> 8;
            } else if (notify_enable == 1){
                buffer[0] = ((uint16_t)GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION) >> 0;
                buffer[1] = ((uint16_t)GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION) >> 8;
            }
            att_server_deferred_read_response(  connection_handle,
                                                HANDLE_GENERIC_OUTPUT_CLIENT_CHAR_CONFIG,
                                                buffer, 2);
            return buffer_size;
        }

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
    case HANDLE_GENERIC_OUTPUT + 1:
        if(*(uint16_t *)buffer == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION)
            notify_enable = 1;
        else
            notify_enable = 0;
        return 0;
    case HANDLE_GENERIC_INPUT:
        console_rx_data((const char *)buffer, buffer_size);
        return 0;
    default:
        return 0;
    }
}

void recv_iq_report(const le_meta_conn_iq_report_t *report)
{
    static char iq_str_buffer[500];
    int len;
    sprintf(iq_str_buffer, "CTE%d:", current_pattern);
    len = strlen(iq_str_buffer);
    base64_encode((const uint8_t *)report,
        sizeof(*report) + report->sample_count * sizeof(report->samples[0]),
        iq_str_buffer + len, sizeof(iq_str_buffer) - len - 1);
    if (notify_enable)
        att_server_notify(handle_send, HANDLE_GENERIC_OUTPUT, (uint8_t *)iq_str_buffer, strlen(iq_str_buffer) + 1);
    else
        platform_printf("%s\n", iq_str_buffer);
}

void recv_pro_connless_iq_report(const le_meta_pro_connless_iq_report_t *report)
{
    static char iq_str_buffer[500];
    int len;
    sprintf(iq_str_buffer, "EXT%d:", current_pattern);
    len = strlen(iq_str_buffer);
    base64_encode((const uint8_t *)report,
        sizeof(*report) + report->sample_count * sizeof(report->samples[0]),
        iq_str_buffer + len, sizeof(iq_str_buffer) - len - 1);
    if (notify_enable)
        att_server_notify(handle_send, HANDLE_GENERIC_OUTPUT, (uint8_t *)iq_str_buffer, strlen(iq_str_buffer) + 1);
    else
        platform_printf("%s\n", iq_str_buffer);
}

void stack_notify_tx_data()
{
    if (notify_enable)
    {
        uint16_t len;
        uint8_t *data = console_get_clear_tx_data(&len);
        att_server_notify(handle_send, HANDLE_GENERIC_OUTPUT, data, len);
    }
}

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    default:
        ;
    }
}

static initiating_phy_config_t phy_configs[] =
{
    {
        .phy = PHY_1M,
        .conn_param =
        {
            .scan_int = 200,
            .scan_win = 150,
            .interval_min = 30,
            .interval_max = 30,
            .latency = 0,
            .supervision_timeout = 200,
            .min_ce_len = 20,
            .max_ce_len = 20
        }
    }
};

void config_switching_pattern(void)
{
    current_pattern++;
    if (current_pattern >= PAT_NUMBER)
        current_pattern = 0;
    if (settings->patterns[current_pattern].len <= 0)
        current_pattern = 0;

    if (settings->patterns[current_pattern].len <= 2)
        settings->patterns[current_pattern].len = 2;
    
    const uint8_t *mapped = switch_pattern_mapping(settings->patterns[current_pattern].len, settings->patterns[current_pattern].ant_ids);

#ifdef PRO_MODE
    ll_scanner_enable_iq_sampling(CTE_AOA, 
                                  settings->slot_duration,
                                  settings->patterns[current_pattern].len,
                                  mapped,
                                  12, 1);
#else
    ll_set_def_antenna(settings->patterns[current_pattern].def);
    gap_set_connection_cte_rx_param(conn_handle,
                                    1,
                                    (cte_slot_duration_type_t)settings->slot_duration,
                                    settings->patterns[current_pattern].len,
                                    mapped);
    gap_set_connection_cte_request_enable(conn_handle,
                                    1,
                                    0,
                                    20, CTE_AOA);

#endif
}

void set_channel(void)
{
    uint8_t channel = settings->channel;
    uint32_t low = channel <= 31 ? 1ul << channel : 0;
    uint8_t high = channel >= 32 ? 1 << (channel - 32) : 0;
    gap_set_host_channel_classification(low, high);
}

void set_responder_led(int r, int g, int b)
{
    #define HANDLE_RGB_LIGHTING_CONTROL                          6
    uint8_t v[3] = { (uint8_t)r, (uint8_t)g, (uint8_t)b };
    if (conn_handle != INVALID_HANDLE)
        gatt_client_write_value_of_characteristic_without_response(conn_handle,
                                                                   HANDLE_RGB_LIGHTING_CONTROL,
                                                                   sizeof(v),
                                                                   v);
}

static void setup_adv()
{
    gap_set_ext_adv_para(0,
                            CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT,
                            800, 800,                  // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
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
}

void setup_ll_param(void)
{
    if (settings)
    {
        set_sample_offset(settings->iq_select);
    }
}

static const scan_phy_config_t configs[] =
{
    {
        .phy = PHY_1M,
        .type = SCAN_PASSIVE,
        .interval = 200,
        .window = 90
    },
};

static bd_addr_t peer_addr = { 0 };
static bd_addr_type_t peer_addr_type = BD_ADDR_TYPE_LE_PUBLIC;
static int peer_found = 0;

static void try_connect_to_peer_with_cte_service(const le_ext_adv_report_t *rpt)
{
    if (peer_found) return;
    if (ad_data_contains_uuid16(rpt->data_len, rpt->data, SIG_UUID_SERVICE_CONSTANT_TONE_EXTENSION) == 0)
        return;
    peer_found = 1;
    gap_set_ext_scan_enable(0, 0, 0, 0);   
    peer_addr_type = rpt->addr_type;
    reverse_bd_addr(rpt->address, peer_addr);
    iprintf("connecting to %02X:%02X:%02X:%02X:%02X:%02X:...\n", 
        peer_addr[0],peer_addr[1],peer_addr[2],
        peer_addr[3],peer_addr[4],peer_addr[5]);
    gap_ext_create_connection(    INITIATING_ADVERTISER_FROM_PARAM, // Initiator_Filter_Policy,
                              BD_ADDR_TYPE_LE_RANDOM,           // Own_Address_Type,
                              peer_addr_type,                   // Peer_Address_Type,
                              peer_addr,                        // Peer_Address,
                              sizeof(phy_configs) / sizeof(phy_configs[0]),
                              phy_configs);
}

static void write_characteristic_value_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_QUERY_COMPLETE:
        iprintf("peer cte enabled\n");
        config_switching_pattern();
        set_channel();
        break;
    }
}

static void enable_peer_cte(service_node_t *first, void *user_data, int err_code)
{
    if (err_code) platform_reset();
    char_node_t *ch = gatt_client_util_find_char_uuid16(discoverer, SIG_UUID_CHARACT_CONSTANT_TONE_EXTENSION_ENABLE);
    if (ch == NULL) platform_reset();
    uint8_t enable = 1;
    gatt_client_write_value_of_characteristic(write_characteristic_value_callback, conn_handle, ch->chara.value_handle, sizeof(enable), &enable);
    gatt_client_util_free(discoverer);
    discoverer = NULL;
}

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const static ext_adv_set_en_t adv_sets_en[1] = {{.handle = 0, .duration = 0, .max_events = 0}};
    static const bd_addr_t rand_addr = { 0xD4, 0x29, 0xF6, 0xBE, 0xF3, 0x26 };    
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    const event_disconn_complete_t *disconn_event;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
        gap_set_random_device_address(rand_addr);

        gap_set_adv_set_random_addr(0, rand_addr);
        setup_adv();
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);

        gap_set_ext_scan_para(BD_ADDR_TYPE_LE_RANDOM, SCAN_ACCEPT_ALL_EXCEPT_NOT_DIRECTED,
                              sizeof(configs) / sizeof(configs[0]),
                              configs);
        gap_set_ext_scan_enable(1, 0, 0, 0);   
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            {
                const le_meta_event_enh_create_conn_complete_t *conn_complete
                    = decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);
                iprintf("connected,%d,%d\n", conn_complete->status,conn_complete->role);
                if (conn_complete->status != 0)
                {
                    gap_ext_create_connection(    INITIATING_ADVERTISER_FROM_PARAM, // Initiator_Filter_Policy,
                                          BD_ADDR_TYPE_LE_RANDOM,           // Own_Address_Type,
                                          peer_addr_type,                   // Peer_Address_Type,
                                          peer_addr,                        // Peer_Address,
                                          sizeof(phy_configs) / sizeof(phy_configs[0]),
                                          phy_configs);
                    break;
                }

                if (HCI_ROLE_SLAVE == conn_complete->role)
                {
                    handle_send = conn_complete->handle;
                    att_set_db(handle_send, profile_data);
                }
                else
                {
                    conn_handle = conn_complete->handle;
                    gatt_client_is_ready(conn_handle);
                    current_pattern = -1;
                    discoverer = gatt_client_util_discover_all(conn_handle, enable_peer_cte, NULL);
                }
            }
            break;
        case HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT:
#ifdef PRO_MODE
            if (0 == scanner_configured)
            {
                scanner_configured = 1;
                config_switching_pattern();
            }
#else
            try_connect_to_peer_with_cte_service(decode_hci_le_meta_event(packet, le_meta_event_ext_adv_report_t)->reports);
#endif
            break;
        case HCI_SUBEVENT_LE_VENDOR_PRO_CONNECTIONLESS_IQ_REPORT:
            recv_pro_connless_iq_report(decode_hci_le_meta_event(packet, le_meta_pro_connless_iq_report_t));
            break;
        case HCI_SUBEVENT_LE_CONNECTION_IQ_REPORT:
            recv_iq_report(decode_hci_le_meta_event(packet, le_meta_conn_iq_report_t));
            config_switching_pattern();
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        disconn_event = decode_hci_event_disconn_complete(packet);
        iprintf("disc:%d,%d,%d\n",disconn_event->conn_handle,handle_send,conn_handle);
        if (disconn_event->conn_handle == handle_send)
        {
            handle_send = INVALID_HANDLE;
            notify_enable = 0;
            gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        }
        else
        {
            conn_handle = INVALID_HANDLE;
            gap_ext_create_connection(    INITIATING_ADVERTISER_FROM_PARAM, // Initiator_Filter_Policy,
                                          BD_ADDR_TYPE_LE_RANDOM,           // Own_Address_Type,
                                          peer_addr_type,                   // Peer_Address_Type,
                                          peer_addr,                        // Peer_Address,
                                          sizeof(phy_configs) / sizeof(phy_configs[0]),
                                          phy_configs);
        }
        break;

    case L2CAP_EVENT_CAN_SEND_NOW:
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

#define ANTENNA_ARRAY_4x4   0
#define ANTENNA_ARRAY_1x4   1
#define ANTENNA_ARRAY_1x3   2
#define ANTENNA_ARRAY_2x2   3
#define ANTENNA_ARRAY_3x3   4
#define ANTENNA_ARRAY_1100  5
#define ANTENNA_ARRAY_1010  6
#define ANTENNA_ARRAY_1001  7

#ifndef CURRENT_ARRAY
#define CURRENT_ARRAY       ANTENNA_ARRAY_4x4
#endif

const static uint8_t pattern[] =
#if (CURRENT_ARRAY == ANTENNA_ARRAY_4x4)
{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }
#elif (CURRENT_ARRAY == ANTENNA_ARRAY_3x3)
{ 0, 1, 2, 4, 5, 6, 8, 9, 10 }
#elif (CURRENT_ARRAY == ANTENNA_ARRAY_2x2)
{ 0, 1, 4, 5 }
#elif (CURRENT_ARRAY == ANTENNA_ARRAY_1x4)
{ 0, 1, 2, 3 }
#elif (CURRENT_ARRAY == ANTENNA_ARRAY_1x3)
{ 0, 1, 2 }
#elif (CURRENT_ARRAY == ANTENNA_ARRAY_1100)
{ 0, 1 }
#elif (CURRENT_ARRAY == ANTENNA_ARRAY_1010)
{ 0, 2 }
#elif (CURRENT_ARRAY == ANTENNA_ARRAY_1001)
{ 0, 3 }
#else
#error unknown array: CURRENT_ARRAY
#endif
;

uint32_t setup_profile(void *data, void *user_data)
{
    iprintf("setup profile\n");
    if (kv_get(KEY_SETTINGS, NULL) == NULL)
    {
        int i;
        settings_t *p = pvPortMalloc(sizeof(settings_t));
        memset(p, 0, sizeof(settings_t));
        p->slot_duration = 1;
        p->patterns[0].len = sizeof(pattern);
        memcpy(p->patterns[0].ant_ids, pattern, sizeof(pattern));
        kv_put(KEY_SETTINGS, (const uint8_t *)p, sizeof(settings_t));
        vPortFree(p);
    }
    settings = (settings_t *)kv_get(KEY_SETTINGS, NULL);

    setup_ll_param();
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(user_packet_handler);
    gatt_client_register_handler(user_packet_handler);
    return 0;
}
