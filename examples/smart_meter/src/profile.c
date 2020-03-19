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

#include "uart_console.h"

#include <string.h>

#include "FreeRTOS.h"
#include "timers.h"

// GATT characteristic handles
#define HANDLE_DEVICE_NAME                                  3
#define HANDLE_GENERIC_INPUT                                6
#define HANDLE_GENERIC_OUTPUT                               8
#define HANDLE_GENERIC_OUTPUT_CLIENT_CHAR_CONFIG			9
#define HANDLE_GENERIC_INPUT_1			                    12
#define HANDLE_GENERIC_OUTPUT_1			                    14
#define HANDLE_GENERIC_OUTPUT_1_CLIENT_CHAR_CONFIG			15

const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

const static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

#define GATT_SERVICE_HEALTH_THERMOMETER                 0x1809
#define GATT_CHARACTERISTIC_TEMPERATURE_MEASUREMENT     0x2a1c

#define INVALID_HANDLE      (0xffff)

static char console_output[200];
static int  output_len = 0;

static uint8_t is_initiating = 0;
static TimerHandle_t app_timer = 0;

bd_addr_t rand_addr = {0xC0,0xAA,0xAA,0xAA,0xAA,0xAA};

typedef struct
{
    uint8_t  notify_enable;
    uint8_t  tpt_enable;
    uint16_t conn_handle;
} peripheral_cfg_t;

peripheral_cfg_t peripheral_cfgs[] = {
    {.notify_enable = 0, .conn_handle = INVALID_HANDLE},
    {.notify_enable = 0, .conn_handle = INVALID_HANDLE},
    {.notify_enable = 0, .conn_handle = INVALID_HANDLE},
    {.notify_enable = 0, .conn_handle = INVALID_HANDLE}
};

#define PERIPHERAL_NUMBER (sizeof(peripheral_cfgs) / sizeof(peripheral_cfgs[0]))

uint8_t conn_handle_to_peripheral_index(const uint16_t conn_handle)
{
    uint8_t index;
    for (index = 0; index < PERIPHERAL_NUMBER; index++)
    {
        if (conn_handle == peripheral_cfgs[index].conn_handle)
        {
            return index;
        }
    }
    return 0xff;
}

void disable_timer_if_no_tpt(void)
{
    uint8_t index;
    for (index = 0; index < PERIPHERAL_NUMBER; index++)
    {
        if ((peripheral_cfgs[index].conn_handle != INVALID_HANDLE) &&
            (peripheral_cfgs[index].tpt_enable))
            return;
    }
    TMR_Disable(APB_TMR1);
}

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, 
                                  uint8_t * buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {
    case HANDLE_GENERIC_INPUT:
        if (buffer)
        {
            // add your code
            return buffer_size;
        }
        else
            return 1; // TODO: return required buffer size

    default:
        return 0;
    }
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, 
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    uint8_t index;
    switch (att_handle)
    {
    case HANDLE_GENERIC_OUTPUT_CLIENT_CHAR_CONFIG:
        index = conn_handle_to_peripheral_index(connection_handle);
        if (index >= PERIPHERAL_NUMBER) return 0;
        if(*(uint16_t *)buffer == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION)
            peripheral_cfgs[index].notify_enable = 1;
        else
            peripheral_cfgs[index].notify_enable = 0;
        return 0;
    case HANDLE_GENERIC_OUTPUT_1_CLIENT_CHAR_CONFIG:
        index = conn_handle_to_peripheral_index(connection_handle);
        if (index >= PERIPHERAL_NUMBER) return 0;
        if(*(uint16_t *)buffer == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION)
        {
            ll_hint_on_ce_len(connection_handle, 30, 30);
            peripheral_cfgs[index].tpt_enable = 1;
            TMR_Reload(APB_TMR1);
            TMR_Enable(APB_TMR1);
        }
        else
        {
            peripheral_cfgs[index].tpt_enable = 0;
            disable_timer_if_no_tpt();
        }
        return 0;
    default:
        return 0;
    }
}

typedef struct slave_info
{
    uint8_t     id;
    bd_addr_t   addr;
    uint16_t    conn_handle;
    gatt_client_service_t                   service_thermo;
    gatt_client_characteristic_t            temp_char;
    gatt_client_characteristic_descriptor_t temp_desc;
    gatt_client_notification_t              temp_notify;
} slave_info_t;

#define INIT_FIELDS .conn_handle = INVALID_HANDLE, \
                    .service_thermo = { .start_group_handle = INVALID_HANDLE}, \
                    .temp_char      = { .value_handle = INVALID_HANDLE}, \
                    .temp_desc      = { .handle = INVALID_HANDLE}

slave_info_t slave_addr_lst[] = 
{
    {.id = 0, .addr = {0xC2, 0x12, 0x35, 0x98, 0x67, 0x00}, INIT_FIELDS},
    {.id = 1, .addr = {0xC2, 0x12, 0x35, 0x98, 0x67, 0x01}, INIT_FIELDS}, 
    {.id = 2, .addr = {0xC2, 0x12, 0x35, 0x98, 0x67, 0x02}, INIT_FIELDS},
#ifdef THREE_SLAVE
    {.id = 3, .addr = {0xC2, 0x12, 0x35, 0x98, 0x67, 0x03}, INIT_FIELDS}
#endif
};

void set_slave_addr(const uint8_t index, const uint8_t *addr)
{
    memcpy(slave_addr_lst[index].addr, addr, sizeof(slave_addr_lst[index].addr));
}

slave_info_t *get_slave_by_addr(const uint8_t *addr)
{
    int i;
    for (i = 0; i < sizeof(slave_addr_lst) / sizeof(slave_addr_lst[0]); i++)
    {
        if (memcmp(slave_addr_lst[i].addr, addr, sizeof(slave_addr_lst[i].addr)) == 0)
            return &slave_addr_lst[i];
    }
    return NULL;
}

slave_info_t *get_slave_by_addr2(const uint8_t *addr)
{
    static bd_addr_t rev;
    reverse_bd_addr(addr, rev);
    return get_slave_by_addr(rev);
}

void start_scan_if_needed(void)
{
    int i;
    for (i = 0; i < sizeof(slave_addr_lst) / sizeof(slave_addr_lst[0]); i++)
    {
        if (INVALID_HANDLE == slave_addr_lst[i].conn_handle)
        {
            // start continuous scanning
            gap_set_ext_scan_enable(1, 0, 0, 0);
            return;
        }
    }
}

slave_info_t *get_slave_by_conn(const uint16_t conn_handle)
{
    int i;
    for (i = 0; i < sizeof(slave_addr_lst) / sizeof(slave_addr_lst[0]); i++)
    {
        if (slave_addr_lst[i].conn_handle == conn_handle)
            return &slave_addr_lst[i];
    }
    return NULL;
}

uint8_t *get_slave_addr(const uint8_t index)
{
    return slave_addr_lst[index].addr;
}

uint8_t is_pending_slave(const uint8_t *addr)
{
    slave_info_t * slave = get_slave_by_addr(addr);
    if (NULL != slave)
        slave = slave;
    if ((NULL == slave) || (slave->conn_handle != INVALID_HANDLE))
        return 0;
    else
        return 1;
}

extern const char *float_ieee_11073_val_to_repr(uint32_t val);
void request_output(void);

static void temperature_notification_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    static char info[25];
    slave_info_t *slave;
    switch (packet[0])
    {
    case GATT_EVENT_NOTIFICATION:
        slave = get_slave_by_conn(channel);
        if (slave)
        {
            int len;
            uint16_t value_size;
            const gatt_event_value_packet_t *value = gatt_event_notification_parse(packet, size, &value_size);
            sprintf(info, "[%d] %s deg\n", slave->id, float_ieee_11073_val_to_repr(*(const uint32_t *)(value->value + 1)));
            len = strlen(info);
            iprintf(info);
            if (sizeof(console_output) - output_len < len)
                output_len = 0;
            memcpy(console_output + output_len, info, len + 1);
            output_len += len;
            request_output();
        }
        else
            dprintf("ERROR: ch %d\n", channel);
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
        iprintf("cmpl\n");
        break;
    }
}

static uint16_t char_config_notification = GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION;

void descriptor_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    hci_con_handle_t channel;
    slave_info_t *slave;
    switch (packet[0])
    {
    case GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT:
        channel = gatt_event_all_characteristic_descriptors_query_result_get_handle(packet);
        slave = get_slave_by_conn(channel);
        gatt_event_all_characteristic_descriptors_query_result_get_characteristic_descriptor(packet, &slave->temp_desc);
        iprintf("[%d] temp desc: %d\n", slave->id, slave->temp_desc.handle);               
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (gatt_event_query_complete_get_status(packet) != 0)
            break;
        channel = gatt_event_query_complete_get_handle(packet);
        slave = get_slave_by_conn(channel);
        if (slave->temp_desc.handle != INVALID_HANDLE)
        {            
            gatt_client_listen_for_characteristic_value_updates(&slave->temp_notify, temperature_notification_handler, 
                                                                channel, &slave->temp_char);
            gatt_client_write_characteristic_descriptor(btstack_callback, channel, &slave->temp_desc, sizeof(char_config_notification),
                                                        (uint8_t *)&char_config_notification);
        }
        break;
    }
}

void characteristic_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    hci_con_handle_t channel;
    slave_info_t *slave;

    switch (packet[0])
    {
    case GATT_EVENT_CHARACTERISTIC_QUERY_RESULT:
        channel = gatt_event_characteristic_query_result_get_handle(packet);
        slave = get_slave_by_conn(channel);
        gatt_event_characteristic_query_result_get_characteristic(packet, &slave->temp_char);
        iprintf("[%d] temp handle: %d\n", slave->id, slave->temp_char.value_handle);               
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (gatt_event_query_complete_get_status(packet) != 0)
            break;
        channel = gatt_event_query_complete_get_handle(packet);
        slave = get_slave_by_conn(channel);
        if (slave->temp_char.value_handle != INVALID_HANDLE)
            gatt_client_discover_characteristic_descriptors(descriptor_discovery_callback, channel, &slave->temp_char);
        break;
    }
}

void service_discovery_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    hci_con_handle_t channel;
    slave_info_t *slave;
    
    switch (packet[0])
    {
    case GATT_EVENT_SERVICE_QUERY_RESULT:
        channel = gatt_event_service_query_result_get_handle(packet);
        slave = get_slave_by_conn(channel);
        gatt_event_service_query_result_get_service(packet, &slave->service_thermo);
        iprintf("[%d] svc handle: %d %d\n", slave->id, 
                slave->service_thermo.start_group_handle, slave->service_thermo.end_group_handle);               
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (gatt_event_query_complete_get_status(packet) != 0)
            break;
        channel = gatt_event_query_complete_get_handle(packet);
        slave = get_slave_by_conn(channel);
        if (slave->service_thermo.start_group_handle != INVALID_HANDLE)
        {
            gatt_client_discover_characteristics_for_service_by_uuid16(characteristic_discovery_callback, channel, 
                                                                       &slave->service_thermo, GATT_CHARACTERISTIC_TEMPERATURE_MEASUREMENT);
        }
        else
        {
            iprintf("service not found, disc\n");
            gap_disconnect(channel);
        }
        break;
    }
}

void slave_connected(const le_meta_event_create_conn_complete_t *conn_complete)
{
    slave_info_t *slave = get_slave_by_addr2(conn_complete->peer_addr);
    if (slave)
    {
        slave->conn_handle = conn_complete->handle;
        gatt_client_discover_primary_services_by_uuid16(service_discovery_callback, conn_complete->handle, GATT_SERVICE_HEALTH_THERMOMETER);
        iprintf("[%d] conn %d\n", slave->id, conn_complete->handle);
    }
    else
    {
        iprintf("error: unknown slave ");
        print_addr(conn_complete->peer_addr);
    }
}

#define USER_MSG_START              1
#define USER_MSG_UPDATE_ADV_DATA    2
#define USER_MSG_UPDATE_ADDR        3
#define USER_MSG_SEND_OUTPUT        4
#define USER_MSG_INITIATE_TIMOUT    5
#define USER_MSG_SEND_TPT           6

void trigger_tpt(void)
{
    btstack_push_user_msg(USER_MSG_SEND_TPT, NULL, 0);
}

static void app_timer_callback(TimerHandle_t xTimer)
{
    btstack_push_user_msg(USER_MSG_INITIATE_TIMOUT, NULL, 0);
}

void peripheral_tpt(void)
{
    int i;
    for (i = 0; i < PERIPHERAL_NUMBER; i++)
    {
        if ((INVALID_HANDLE != peripheral_cfgs[i].conn_handle)
            && peripheral_cfgs[i].tpt_enable)
        {
            int16_t size = 220000 / 1000 * 20 / 8;   // for 220kb throughput
            uint16_t mtu = att_server_get_mtu(peripheral_cfgs[i].conn_handle) - 3;
            while (size > 0)
            {
                att_server_notify(peripheral_cfgs[i].conn_handle, 
                                  HANDLE_GENERIC_OUTPUT_1, NULL, mtu);
                size -= mtu;
            }
        }
    }
}

void start_adv_if_needed(void)
{
    const static ext_adv_set_en_t adv_sets_en[] = {{.handle = 0, .duration = 0, .max_events = 0}};
    int i;
    for (i = 0; i < PERIPHERAL_NUMBER; i++)
    {
        if (INVALID_HANDLE == peripheral_cfgs[i].conn_handle)
        {
            gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
            return;
        }
    }
}

void peripheral_disc(const hci_con_handle_t conn_handle)
{
    int i;
    for (i = 0; i < PERIPHERAL_NUMBER; i++)
    {
        if (conn_handle == peripheral_cfgs[i].conn_handle)
        {
            peripheral_cfgs[i].conn_handle = INVALID_HANDLE;
            peripheral_cfgs[i].notify_enable = 0;
            peripheral_cfgs[i].tpt_enable = 0;
        }
    }
    start_adv_if_needed();
}

uint8_t peripheral_connect(const hci_con_handle_t conn_handle)
{
    int i;
    for (i = 0; i < PERIPHERAL_NUMBER; i++)
    {
        if (INVALID_HANDLE == peripheral_cfgs[i].conn_handle)
        {
            peripheral_cfgs[i].conn_handle = conn_handle;
            peripheral_cfgs[i].notify_enable = 0;
            return i;
        }
    }
    
    return 0xff;
}

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    
    switch (msg_id)
    {
    case USER_MSG_START:
        start_adv_if_needed();
        start_scan_if_needed();
        break;
    case USER_MSG_UPDATE_ADDR:
        gap_set_adv_set_random_addr(0, rand_addr);
        iprintf("addr changed: ");
        print_addr(rand_addr);
        break;
    case USER_MSG_SEND_OUTPUT:
        if (output_len > 0)
        {
            uint8_t handle;
            for (handle = 0; handle < sizeof(peripheral_cfgs) / sizeof(peripheral_cfgs[0]); handle++)
            {
                if (peripheral_cfgs[handle].notify_enable)
                {
                    att_server_notify(peripheral_cfgs[handle].conn_handle, 
                                      HANDLE_GENERIC_OUTPUT, (uint8_t*)console_output, output_len + 1);
                }
            } 

            output_len = 0;
        }
        break;
    case USER_MSG_INITIATE_TIMOUT:
        if (is_initiating)
        {
            iprintf("initiate timout\n");
            is_initiating = 0;
            gap_create_connection_cancel();
            start_scan_if_needed();
        }
        break;
    case USER_MSG_SEND_TPT:
        peripheral_tpt();
        break;
    default:
        ;
    }
}

void request_output(void)
{
    uint8_t output_flags = 0;
    uint8_t handle;
    for (handle = 0; handle < PERIPHERAL_NUMBER; handle++)
        output_flags |= peripheral_cfgs[handle].notify_enable;

    if (output_flags != 0)
        btstack_push_user_msg(USER_MSG_SEND_OUTPUT, NULL, 0);
}

void update_addr()
{
    btstack_push_user_msg(USER_MSG_UPDATE_ADDR, NULL, 0);
}

void start_run()
{
    btstack_push_user_msg(USER_MSG_START, NULL, 0);
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
            .scan_win = 50,
            .interval_min = 100,
            .interval_max = 200,
            .latency = 2,
            .supervision_timeout = 200,
            .min_ce_len = 5,
            .max_ce_len = 15
        }
    }
};

bd_addr_t peer_addr;

static void setup_adv(void)
{    
    gap_set_adv_set_random_addr(0, rand_addr);
    gap_set_ext_adv_para(0, 
                        CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT,
                        0x0500, 0x0500,            // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
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

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{    
    const le_meta_event_create_conn_complete_t *conn_complete;
    const event_disconn_complete_t *disconn_event;
    slave_info_t *slave;
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
        gap_set_random_device_address(rand_addr);
        setup_adv();

        gap_set_ext_scan_para(BD_ADDR_TYPE_LE_PUBLIC, SCAN_ACCEPT_ALL_EXCEPT_NOT_DIRECTED,
                              sizeof(configs) / sizeof(configs[0]),
                              configs);
#ifndef AUTO_START        
        btstack_push_user_msg(USER_MSG_START, NULL, 0);
#endif
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT:
            {
                const le_ext_adv_report_t *report = decode_hci_le_meta_event(packet, le_meta_event_ext_adv_report_t)->reports;

                reverse_bd_addr(report->address, peer_addr);
                // print_addr(peer_addr);
                if (is_pending_slave(peer_addr))
                {
                    gap_set_ext_scan_enable(0, 0, 0, 0);
                    iprintf("connecting ... ");  print_addr(peer_addr);

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
                    is_initiating = 1;
                    xTimerReset(app_timer, portMAX_DELAY);
                }
            }
            break;
        case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
            conn_complete = decode_hci_le_meta_event(packet, le_meta_event_create_conn_complete_t);
            iprintf("role = %d, handle = %d\n", conn_complete->role, conn_complete->handle);
            
            if (HCI_ROLE_SLAVE == conn_complete->role)
            {
                if (0 == conn_complete->status)
                {
                    att_set_db(conn_complete->handle, profile_data);
                    ll_hint_on_ce_len(conn_complete->handle, 10, 15);
                }
            }
            else
            {
                print_addr(conn_complete->peer_addr);
                is_initiating = 0;
                xTimerStop(app_timer, portMAX_DELAY);
                if (0 == conn_complete->status)
                    slave_connected(conn_complete);
                start_scan_if_needed();
            }
            break;
        case HCI_SUBEVENT_LE_ADVERTISING_SET_TERMINATED:
            {
                const le_meta_adv_set_terminated_t *adv_term = decode_hci_le_meta_event(packet, le_meta_adv_set_terminated_t);
                if (adv_term->status == 0)
                    peripheral_connect(adv_term->conn_handle);
                start_adv_if_needed();
            }
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        disconn_event = decode_hci_event_disconn_complete(packet);
        slave = get_slave_by_conn(disconn_event->conn_handle);
        if (slave)
        {
            iprintf("[%d] disc\n", slave->id);
            slave->conn_handle = INVALID_HANDLE;
            start_scan_if_needed();
        }
        else
            peripheral_disc(disconn_event->conn_handle);

        break;

    case ATT_EVENT_CAN_SEND_NOW:        
        break;

    case BTSTACK_EVENT_USER_MSG:
        p_user_msg = hci_event_packet_get_user_msg(packet);
        user_msg_handler(p_user_msg->msg_id, p_user_msg->data, p_user_msg->len);
        break;

    default:
        break;
    }
}

uint32_t setup_profile(void *data, void *user_data)
{
    app_timer = xTimerCreate("a",
                            pdMS_TO_TICKS(5000),
                            pdFALSE,
                            NULL,
                            app_timer_callback);
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

