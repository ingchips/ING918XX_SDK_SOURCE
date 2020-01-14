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

#include "cm32gpm3.h"

#include "ad_parser.h"
#include "uart_console.h"

#define INVALID_HANDLE      (0xffff)
const uint8_t UUID_TPT[]            = {0x24,0x45,0x31,0x4a,0xa1,0xd4,0x48,0x74,0xb4,0xd1,0xfd,0xfb,0x6f,0x50,0x14,0x85};
const uint8_t UUID_CHAR_GEN_IN[]    = {0xbf,0x83,0xf3,0xf1,0x39,0x9a,0x41,0x4d,0x90,0x35,0xce,0x64,0xce,0xb3,0xff,0x67};
const uint8_t UUID_CHAR_GEN_OUT[]   = {0xbf,0x83,0xf3,0xf2,0x39,0x9a,0x41,0x4d,0x90,0x35,0xce,0x64,0xce,0xb3,0xff,0x67};

void print_addr(const uint8_t *addr);

typedef struct slave_info
{    
    uint32_t    s2m_total;
    uint32_t    m2s_total;
    gatt_client_service_t                   service_tpt;
    gatt_client_characteristic_t            input_char;
    gatt_client_characteristic_t            output_char;
    gatt_client_characteristic_descriptor_t output_desc;
    gatt_client_notification_t              output_notify;
    uint16_t    conn_handle;
    uint8_t     m2s_run;
    uint8_t     m2s_paused;
    uint8_t     s2m_run;
    
} slave_info_t;

slave_info_t slave = {.conn_handle = INVALID_HANDLE};

void reset_info(void)
{
    slave.conn_handle                       = INVALID_HANDLE;
    slave.service_tpt.start_group_handle    = INVALID_HANDLE;
    slave.input_char.value_handle           = INVALID_HANDLE;
    slave.output_char.value_handle          = INVALID_HANDLE;
    slave.output_desc.handle                = INVALID_HANDLE;
    slave.m2s_run = 0;
    slave.s2m_run = 0;
}

static void output_notification_handler(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    uint16_t value_size;
    switch (packet[0])
    {
    case GATT_EVENT_NOTIFICATION:
        gatt_event_notification_parse(packet, size, &value_size);
        slave.s2m_total += value_size;
        break;
    }
}

void btstack_callback(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_QUERY_COMPLETE:
        if (gatt_event_query_complete_get_status(packet) != 0)
            return;
        printf("cmpl\n");
        break;
    }
}

static uint16_t char_config_notification = GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION;
static uint16_t char_config_none = 0;

void descriptor_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT:
        gatt_event_all_characteristic_descriptors_query_result_get_characteristic_descriptor(packet, &slave.output_desc);
        printf("output desc: %d\n", slave.output_desc.handle);               
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (gatt_event_query_complete_get_status(packet) != 0)
            break;
        
        if (slave.output_desc.handle != INVALID_HANDLE)
        {            
            gatt_client_listen_for_characteristic_value_updates(&slave.output_notify, output_notification_handler, 
                                                                slave.conn_handle, &slave.output_char);            
        }
        break;
    }
}

void characteristic_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_CHARACTERISTIC_QUERY_RESULT:
        if (INVALID_HANDLE == slave.input_char.value_handle)
        {
            gatt_event_characteristic_query_result_get_characteristic(packet, &slave.input_char);
            printf("input handle: %d\n", slave.input_char.value_handle);
        }
        else
        {
            gatt_event_characteristic_query_result_get_characteristic(packet, &slave.output_char);
            printf("output handle: %d\n", slave.output_char.value_handle);
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (gatt_event_query_complete_get_status(packet) != 0)
            break;
        if (INVALID_HANDLE == slave.input_char.value_handle)
        {
            printf("characteristic not found, disc\n");
            gap_disconnect(slave.conn_handle);
        }
        else
        {
            if (INVALID_HANDLE == slave.output_char.value_handle)
                gatt_client_discover_characteristics_for_service_by_uuid128(characteristic_discovery_callback, slave.conn_handle, 
                                                                       &slave.service_tpt, UUID_CHAR_GEN_OUT);
            else
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
        gatt_event_service_query_result_get_service(packet, &slave.service_tpt);
        printf("service handle: %d %d\n", 
                slave.service_tpt.start_group_handle, slave.service_tpt.end_group_handle);               
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (gatt_event_query_complete_get_status(packet) != 0)
            break;
        if (slave.service_tpt.start_group_handle != INVALID_HANDLE)
        {
            gatt_client_discover_characteristics_for_service_by_uuid128(characteristic_discovery_callback, slave.conn_handle, 
                                                                       &slave.service_tpt, UUID_CHAR_GEN_IN);
        }
        else
        {
            printf("service not found, disc\n");
            gap_disconnect(slave.conn_handle);
        }
        break;
    }
}

void send_data(void)
{
    uint16_t len;
    uint8_t r;
    if (0 == slave.m2s_run)
        return;
    
    slave.m2s_paused = 0;
    
    gatt_client_get_mtu(slave.conn_handle, &len);
    len -= 3;

    do
    {
        r = gatt_client_write_value_of_characteristic_without_response(slave.conn_handle, 
                                                                       slave.input_char.value_handle, len, (uint8_t *)0x4000);
        
        switch (r)
        {
            case 0:
                slave.m2s_total += len;
                break;
            case BTSTACK_ACL_BUFFERS_FULL:
                att_dispatch_client_request_can_send_now_event(slave.conn_handle);
                break;
            default:
                slave.m2s_paused = 1;   // gatt client is busy, thoughput is paused
        }
    } while (0 == r);
}

#define USER_MSG_START_M2S              1
#define USER_MSG_START_S2M              2
#define USER_MSG_STOP_M2S               3
#define USER_MSG_STOP_S2M               4
#define USER_MSG_SHOW_TPT               5

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MSG_START_M2S:
        slave.m2s_run = 1;
        slave.m2s_total = 0;
        send_data();
        TMR_Reload(APB_TMR1);
        TMR_Enable(APB_TMR1);
        break;
    case USER_MSG_START_S2M:
        slave.s2m_run = 1;
        slave.s2m_total = 0;
        gatt_client_write_characteristic_descriptor(btstack_callback, slave.conn_handle, &slave.output_desc, sizeof(char_config_notification),
                                                        (uint8_t *)&char_config_notification);
        TMR_Reload(APB_TMR1);
        TMR_Enable(APB_TMR1);
        break;
    case USER_MSG_STOP_M2S:
        slave.m2s_run = 0;
        if ((slave.m2s_run | slave.m2s_run) == 0)
            TMR_Disable(APB_TMR1);
        break;
    case USER_MSG_STOP_S2M:
        slave.s2m_run = 0;
        gatt_client_write_characteristic_descriptor(btstack_callback, slave.conn_handle, &slave.output_desc, sizeof(char_config_none),
                                                        (uint8_t *)&char_config_none);
        if ((slave.m2s_run | slave.m2s_run) == 0)
            TMR_Disable(APB_TMR1);
        break;
    case USER_MSG_SHOW_TPT:
        if (slave.m2s_run)
        {
            if (slave.m2s_paused) send_data();
            printf("M->S: %dbps\n", slave.m2s_total << 3);
        }
        if (slave.s2m_run)
            printf("S->M: %dbps\n", slave.s2m_total << 3);
        slave.m2s_total = 0;
        slave.s2m_total = 0;
    default:
        ;
    }
}

uint32_t timer_isr(void *user_data)
{
    TMR_IntClr(APB_TMR1);
    if (INVALID_HANDLE == slave.output_desc.handle)
        return 0;
    btstack_push_user_msg(USER_MSG_SHOW_TPT, NULL, 0);
    return 0;
}

void start_tpt(tpt_dir_t dir)
{
    if (INVALID_HANDLE == slave.output_desc.handle)
    {
        printf("ERROR: not ready\n");
        return;
    }
    
    if (DIR_M_TO_S == dir)
        btstack_push_user_msg(USER_MSG_START_M2S, NULL, 0);
    else if (DIR_S_TO_M == dir)
        btstack_push_user_msg(USER_MSG_START_S2M, NULL, 0);
}

void stop_tpt(tpt_dir_t dir)
{
    if (INVALID_HANDLE == slave.output_desc.handle)
    {
        printf("ERROR: not ready\n");
        return;
    }
    
    if (DIR_M_TO_S == dir)
        btstack_push_user_msg(USER_MSG_STOP_M2S, NULL, 0);
    else if (DIR_S_TO_M == dir)
        btstack_push_user_msg(USER_MSG_STOP_S2M, NULL, 0);
}

static const scan_phy_config_t configs[2] =
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

static initiating_phy_config_t phy_configs[] =
{
    {
        .phy = PHY_1M,
        .conn_param =
        {
            .scan_int = 200,
            .scan_win = 180,
            .interval_min = 50,
            .interval_max = 50,
            .latency = 0,
            .supervision_timeout = 200,
            .min_ce_len = 90,
            .max_ce_len = 90
        }
    }
};

bd_addr_t rand_addr = {0xC0, 0x00, 0x00, 0x11, 0x11, 0x11};
bd_addr_t peer_addr;

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const le_meta_event_create_conn_complete_t *conn_complete;
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
        gap_set_random_device_address(rand_addr);
        gap_set_ext_scan_para(BD_ADDR_TYPE_LE_PUBLIC, SCAN_ACCEPT_ALL_EXCEPT_NOT_DIRECTED,
                              sizeof(configs) / sizeof(configs[0]),
                              configs);
        gap_set_ext_scan_enable(1, 0, 0, 0);   // start continuous scanning
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT:
            {
                const le_ext_adv_report_t *report = decode_hci_le_meta_event(packet, le_meta_event_ext_adv_report_t)->reports;

                if (ad_data_contains_uuid128(report->data_len, report->data, UUID_TPT))
                {
                    gap_set_ext_scan_enable(0, 0, 0, 0);
                    reverse_bd_addr(report->address, peer_addr);
                    printf("connecting ... ");  print_addr(peer_addr);

                    if (report->evt_type & HCI_EXT_ADV_PROP_USE_LEGACY)
                        phy_configs[0].phy = PHY_1M;
                    else
                        phy_configs[0].phy = (phy_type_t)(report->s_phy != 0 ? report->s_phy : report->p_phy); 
                    gap_ext_create_connection(    INITIATING_ADVERTISER_FROM_PARAM, // Initiator_Filter_Policy,
                                                  BD_ADDR_TYPE_LE_RANDOM,           // Own_Address_Type,
                                                  report->addr_type,                // Peer_Address_Type,
                                                  peer_addr,                        // Peer_Address,
                                                  sizeof(phy_configs) / sizeof(phy_configs[0]),
                                                  phy_configs);
                }
            }
            break;
        case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
            conn_complete = decode_hci_le_meta_event(packet, le_meta_event_create_conn_complete_t);
            slave.conn_handle = conn_complete->handle;
            gap_set_phy(slave.conn_handle, 0, PHY_2M_BIT, PHY_2M_BIT, HOST_NO_PREFERRED_CODING);
            gatt_client_discover_primary_services_by_uuid128(service_discovery_callback, conn_complete->handle, UUID_TPT);            
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        reset_info();
        gap_set_ext_scan_enable(1, 0, 0, 0);   // start continuous scanning
        break;

    case L2CAP_EVENT_CAN_SEND_NOW:
        send_data();
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
    reset_info();
    hci_event_callback_registration.callback = user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(user_packet_handler);
    gatt_client_register_handler(user_packet_handler);
    return 0;
}
