#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "profile.h"
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "gatt_client.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "att_db_util.h"
#include "sig_uuid.h"
#include "btstack_util.h"
#include "gatt_client_util.h"

struct gatt_client_discoverer *discoverer = NULL;

static uint8_t adv_data[31] = { 0 };

static uint8_t scan_data[31] = { 0 };

uint8_t handle_map_to_slave[256] = {0};
uint8_t handle_map_from_slave[256] = {0};

void show_app_status(app_status_t status);

int adv_data_len = 0;
int scan_data_len = 0;
int adv_run = 0;

#define INVALIDE_HANDLE         0xffff

hci_con_handle_t handle_to_slave = INVALIDE_HANDLE;
hci_con_handle_t handle_to_master = INVALIDE_HANDLE;

void setup_adv(void);

void gatt_read_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT:
        {
            uint16_t value_size;
            const gatt_event_value_packet_t *value =
                gatt_event_characteristic_value_query_result_parse(packet, size, &value_size);
            att_server_deferred_read_response(handle_to_master,
                                              handle_map_from_slave[value->handle],
                                              value->value, value_size);
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        break;
    }
}

static void output_notification_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const gatt_event_value_packet_t *value;
    uint16_t value_size;
    if (handle_to_master == INVALIDE_HANDLE) return;

    switch (packet[0])
    {
    case GATT_EVENT_NOTIFICATION:
        value = gatt_event_notification_parse(packet, size, &value_size);
        if (value_size)
        {
            att_server_notify(handle_to_master, handle_map_from_slave[value->handle], (uint8_t *)value->value, value_size);
        }
        break;
    case GATT_EVENT_INDICATION:
        value = gatt_event_indication_parse(packet, size, &value_size);
        if (value_size)
        {
            att_server_indicate(handle_to_master, handle_map_from_slave[value->handle], (uint8_t *)value->value, value_size);
        }
        break;
    }
}

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size)
{
    if (buffer == NULL)
    {
        uint16_t handle = handle_map_to_slave[att_handle];
        gatt_client_read_value_of_characteristic_using_value_handle(gatt_read_callback,
            handle_to_slave,
            handle);
        return ATT_DEFERRED_READ;
    }
    else
        platform_raise_assertion(__FILE__, __LINE__);
    return 0;
}

void gatt_dummy_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_QUERY_COMPLETE:
        break;
    }
}

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{    
    uint16_t handle = handle_map_to_slave[att_handle];
    char_node_t *c = gatt_client_util_find_char(discoverer, handle);

    if (c && (c->chara.properties & ATT_PROPERTY_WRITE_WITHOUT_RESPONSE))
        gatt_client_write_value_of_characteristic_without_response(handle_to_slave, handle, buffer_size, buffer);
    else
        gatt_client_write_value_of_characteristic(gatt_dummy_callback, handle_to_slave, handle, buffer_size, buffer);

    return 0;
}

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

static const uint8_t *parse_uuid(const uint8_t *uuid, uint16_t *uuid16)
{
    if (uuid_has_bluetooth_prefix(uuid))
    {
        *uuid16 = (uuid[2] << 8) | uuid[3];
        return NULL;
    }
    else
    {
        *uuid16 = 0xffff;
        return uuid;
    }
}

void clone_gatt_profile(service_node_t *first)
{
    service_node_t *s = first;
    const uint8_t *uuid128;
    uint16_t uuid16;
    uint16_t handle;

    iprintf("========== CLONING GATT PROFILE ==========\n\n");

    for (s = first; s; s = s->next)
    {
        iprintf("SERVICE: ");
        gatt_client_util_print_uuid(s->service.uuid128);
        iprintf("\nHANDLE RANGE: %d - %d\n\n", s->service.start_group_handle, s->service.end_group_handle);

        uuid128 = parse_uuid(s->service.uuid128, &uuid16);
        if (uuid128)
        {
            att_db_util_add_service_uuid128(uuid128);
        }
        else
            att_db_util_add_service_uuid16(uuid16);
        
        char_node_t *c;
        for (c = s->chars; c; c = c->next)
        {
            iprintf("    CHARACTERISTIC: ");
            gatt_client_util_print_uuid(c->chara.uuid128);
            iprintf("\n        HANDLE RANGE: %d - %d\n", c->chara.start_handle, c->chara.end_handle);
            iprintf(  "        VALUE HANDLE: %d\n", c->chara.value_handle);
            iprintf(  "        PROPERTIES  : "); gatt_client_util_print_properties(c->chara.properties); iprintf("\n\n");
            
            if (c->chara.value_handle > 255)
                platform_raise_assertion(__FILE__, __LINE__);
            
            uuid128 = parse_uuid(c->chara.uuid128, &uuid16);
            if (uuid128)
            {
                handle = att_db_util_add_characteristic_uuid128(uuid128,
                    c->chara.properties | ATT_PROPERTY_DYNAMIC, NULL, 0);
            }
            else
                handle = att_db_util_add_characteristic_uuid16(uuid16,
                    c->chara.properties | ATT_PROPERTY_DYNAMIC, NULL, 0);
            
            handle_map_to_slave[handle] = c->chara.value_handle;
            handle_map_from_slave[c->chara.value_handle] = handle;
            
            int has_config = 0;
            if (c->chara.properties & (ATT_PROPERTY_NOTIFY | ATT_PROPERTY_INDICATE))
            {
                has_config = 1;
                
                c->notification = (gatt_client_notification_t *)malloc(sizeof(gatt_client_notification_t));
                gatt_client_listen_for_characteristic_value_updates(
                    c->notification, output_notification_handler,
                    handle_to_slave, c->chara.value_handle);
            }

            desc_node_t *d;
            for (d = c->descs; d; d = d->next)
            {
                iprintf("        DESCRIPTOR: ");
                gatt_client_util_print_uuid(d->desc.uuid128);
                iprintf("\n        HANDLE: %d\n\n", d->desc.handle);
                
                if (d->desc.handle > 255)
                    platform_raise_assertion(__FILE__, __LINE__);

                uuid128 = parse_uuid(d->desc.uuid128, &uuid16);
                
                if (has_config && (GATT_CLIENT_CHARACTERISTICS_CONFIGURATION == uuid16))
                {
                    handle = handle_map_from_slave[c->chara.value_handle] + 1;
                    handle_map_to_slave[handle] = d->desc.handle;
                    handle_map_from_slave[d->desc.handle] = handle;
                    continue;
                }

                handle = att_db_util_add_descriptor_uuid16(uuid16,
                        ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC,
                        NULL, 0);

                handle_map_to_slave[handle] = d->desc.handle;
                handle_map_from_slave[d->desc.handle] = handle;
            }
        }
    }
    
    setup_adv();
}

const static ext_adv_set_en_t adv_sets_en[] = { {.handle = 0, .duration = 0, .max_events = 0} };

void setup_adv(void)
{
    show_app_status(APP_ADVERTISING);
    gap_set_ext_adv_para(0,
                            CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT,
                            0x00a1, 0x00a1,            // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
                            PRIMARY_ADV_ALL_CHANNELS,  // Primary_Advertising_Channel_Map
                            BD_ADDR_TYPE_LE_RANDOM,    // Own_Address_Type
                            BD_ADDR_TYPE_LE_PUBLIC,    // Peer_Address_Type (ignore)
                            NULL,                      // Peer_Address      (ignore)
                            ADV_FILTER_ALLOW_ALL,      // Advertising_Filter_Policy
                            100,                       // Advertising_Tx_Power
                            PHY_1M,                    // Primary_Advertising_PHY
                            0,                         // Secondary_Advertising_Max_Skip
                            PHY_1M,                    // Secondary_Advertising_PHY
                            0x00,                      // Advertising_SID
                            0x00);                     // Scan_Request_Notification_Enable
    gap_set_ext_adv_data(0, adv_data_len, (uint8_t*)adv_data);
    gap_set_ext_scan_response_data(0, scan_data_len, (uint8_t*)scan_data);
    gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
}

static uint8_t att_db_storage[1000];

uint8_t *init_service()
{
    att_db_util_init(att_db_storage, sizeof(att_db_storage));

    return att_db_util_get_address();
}

static scan_phy_config_t scan_configs[] =
{
    {
        .phy = PHY_1M,
        .type = SCAN_ACTIVE,
        .interval = 200,
        .window = 90
    },
    {
        .phy = PHY_CODED,
        .type = SCAN_ACTIVE,
        .interval = 200,
        .window = 90
    }
};

#define CONN_PARAM  {                   \
            .scan_int = 200,            \
            .scan_win = 100,            \
            .interval_min = 30,         \
            .interval_max = 30,         \
            .latency = 0,               \
            .supervision_timeout = 200, \
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

static bd_addr_t rand_addr = {0xC6, 0x2A, 0x35, 0xAD, 0xFC, 0x68};

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    static const bd_addr_t peer_addr = { 0xCD, 0xA3, 0x28, 0x11, 0x89, 0x3e };
    const bd_addr_type_t peer_addr_type = BD_ADDR_TYPE_LE_RANDOM;
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
        show_app_status(APP_SCANNING);
#ifndef DEBUG
        {
            iprintf("\nADDR ");
            int i;
            for (i = 0; i < sizeof(rand_addr); i++)
            {
                rand_addr[i] = platform_rand() & 0xff;
                if (i == 0) rand_addr[i] |= 0xc0;
                iprintf(":%02X", rand_addr[i]);
            }
            iprintf("\n");
        }
#endif
        gap_set_adv_set_random_addr(0, rand_addr);
        gap_set_random_device_address(rand_addr);
        gap_add_whitelist(peer_addr, peer_addr_type);
        gap_set_ext_scan_para(BD_ADDR_TYPE_LE_RANDOM, SCAN_ACCEPT_WLIST_EXCEPT_NOT_DIRECTED,
                              sizeof(scan_configs) / sizeof(scan_configs[0]),
                              scan_configs);
        gap_set_ext_scan_enable(1, 0, 0, 0);   // start continuous scanning
        break;

    case HCI_EVENT_COMMAND_COMPLETE:
        switch (hci_event_command_complete_get_command_opcode(packet))
        {
        
        default:
            break;
        }
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT:
            {
                const le_ext_adv_report_t *report = decode_hci_le_meta_event(packet, le_meta_event_ext_adv_report_t)->reports;
                if (scan_data_len) break;
                
                if ((report->evt_type & HCI_EXT_ADV_PROP_SCAN_RSP) == 0)
                {
                    memcpy(adv_data, report->data, report->data_len);
                    adv_data_len = report->data_len;
                }
                else
                {
                    memcpy(scan_data, report->data, report->data_len);
                    scan_data_len = report->data_len;
                    gap_set_ext_scan_enable(0, 0, 0, 0);
                    
                    show_app_status(APP_CONNECTING);
                    iprintf("create connection...\n");
                    gap_ext_create_connection(INITIATING_ADVERTISER_FROM_LIST,
                                              BD_ADDR_TYPE_LE_RANDOM,           // Own_Address_Type,
                                              BD_ADDR_TYPE_LE_RANDOM,
                                              NULL,
                                              sizeof(phy_configs) / sizeof(phy_configs[0]),
                                              phy_configs);
                }
            }
            break;

        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            {
                const le_meta_event_enh_create_conn_complete_t * complete =
                    decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);
                if (HCI_ROLE_SLAVE == complete->role)
                {
                    show_app_status(APP_CONNECTED);
                    handle_to_master = complete->handle;
                    att_set_db(handle_to_master, att_db_util_get_address());
                }
                else
                {
                    handle_to_slave = complete->handle;
                    show_app_status(APP_CLONING);
                    iprintf("discovering...\n");
                    discoverer = gatt_client_util_discover_all(handle_to_slave, clone_gatt_profile);
                }
            }
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        {
            const event_disconn_complete_t *disc = decode_hci_event_disconn_complete(packet);
            if (disc->conn_handle == handle_to_slave)
                platform_reset();
            else
            {
                show_app_status(APP_ADVERTISING);
                handle_to_master = INVALIDE_HANDLE;
                gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
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

static btstack_packet_callback_registration_t hci_event_callback_registration;

uint32_t setup_profile(void *data, void *user_data)
{
    platform_printf("setup profile\n");
    // Note: security has not been enabled.
    init_service();
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

