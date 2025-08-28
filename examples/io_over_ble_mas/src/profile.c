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

#include <string.h>

#include "io_interf.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "ad_parser.h"

#define dbg_printf printf

#define INVALID_HANDLE      (0xffff)

extern const pair_config_t pair_config;
static TimerHandle_t flush_timer = 0;
uint32_t total_rx = 0;

const uint8_t UUID_ING_CONSOLE_SERVICE[]    = {0x43, 0xf4, 0xb1, 0x14, 0xca, 0x67, 0x48, 0xe8, 0xa4, 0x6f, 0x9a, 0x8f, 0xfe, 0xb7, 0x14, 0x6a};
const uint8_t UUID_ING_GENERIC_INPUT[]      = {0xbf, 0x83, 0xf3, 0xf1, 0x39, 0x9a, 0x41, 0x4d, 0x90, 0x35, 0xce, 0x64, 0xce, 0xb3, 0xff, 0x67};
const uint8_t UUID_ING_GENERIC_OUTPUT[]     = {0xbf, 0x83, 0xf3, 0xf2, 0x39, 0x9a, 0x41, 0x4d, 0x90, 0x35, 0xce, 0x64, 0xce, 0xb3, 0xff, 0x67};

static btstack_packet_callback_registration_t hci_event_callback_registration;

typedef struct slave_info
{
    const uint8_t *addr;
    uint16_t     conn_handle;
    gatt_client_service_t                   service_console;
    gatt_client_characteristic_t            input_char;
    gatt_client_characteristic_t            output_char;
    gatt_client_characteristic_descriptor_t output_desc;
    gatt_client_notification_t              output_notify;
} slave_info_t;

slave_info_t slave =
{
    .conn_handle = INVALID_HANDLE,
    .service_console = { .start_group_handle = INVALID_HANDLE},
    .input_char  = { .value_handle = INVALID_HANDLE},
    .output_char = { .value_handle = INVALID_HANDLE},
    .output_desc = { .handle = INVALID_HANDLE}
};

static const scan_phy_config_t configs[] =
{
    {
        .phy = PHY_1M,
        .type = SCAN_PASSIVE,
        .interval = 200,
        .window = 190
    },
    {
        .phy = PHY_CODED,
        .type = SCAN_PASSIVE,
        .interval = 200,
        .window = 190
    }
};

static int cb_ring_buf_peek_data(const void *data, int len, int has_more, void *extra)
{
    int r = 0;
    int flush = extra != NULL;

    uint16_t mtu;
    gatt_client_get_mtu(slave.conn_handle, &mtu);
    mtu -= 3;
    if ((flush == 0) && (has_more == 0) && (len < mtu))
        return r;
    const uint8_t *p = (const uint8_t *)(data);
    while (len)
    {
        int size = len > mtu ? mtu : len;
        uint8_t t = gatt_client_write_value_of_characteristic_without_response(slave.conn_handle, slave.input_char.value_handle,
                                                                       size, p);
        if (t)
        {
            att_dispatch_client_request_can_send_now_event(slave.conn_handle);
            break;
        }

        len -= size;
        p += size;
        r += size;
    }

    return r;
}

#include "buf_io.c"

static void output_notification_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const gatt_event_value_packet_t *value;
    uint16_t value_size;
    switch (packet[0])
    {
    case GATT_EVENT_NOTIFICATION:
        show_state(STATE_RX);
        value = gatt_event_notification_parse(packet, size, &value_size);
        handle_output(value->value, value_size);
        total_rx += value_size;
        break;
    }
}

void btstack_callback(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_QUERY_COMPLETE:
        dbg_printf("cmpl, in %d, output %d, %d\n", slave.input_char.value_handle, slave.output_char.value_handle, slave.output_desc.handle);
        show_state(STATE_CONNECTED);
        break;
    }
}

const uint16_t char_config_notification = GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION;

void descriptor_discovery_callback(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const gatt_event_all_characteristic_descriptors_query_result_t *result;
    switch (packet[0])
    {
    case GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT:
        result = gatt_event_all_characteristic_descriptors_query_result_parse(packet);
        slave.output_desc = result->descriptor;
        dbg_printf("output desc: %d\n", slave.output_desc.handle);
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (slave.output_desc.handle != INVALID_HANDLE)
        {
            gatt_client_listen_for_characteristic_value_updates(&slave.output_notify, output_notification_handler,
                                                                channel, slave.output_char.value_handle);
            gatt_client_write_characteristic_descriptor_using_descriptor_handle(btstack_callback, channel, slave.output_desc.handle, sizeof(char_config_notification),
                                                        (uint8_t *)&char_config_notification);
        }
        break;
    }
}

void characteristic_discovery_callback(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_CHARACTERISTIC_QUERY_RESULT:
        {
            const gatt_event_characteristic_query_result_t *result =
                gatt_event_characteristic_query_result_parse(packet);
            if (INVALID_HANDLE == slave.input_char.value_handle)
                slave.input_char = result->characteristic;
            else
                slave.output_char = result->characteristic;
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (INVALID_HANDLE == slave.output_char.value_handle)
        {
            gatt_client_discover_characteristics_for_handle_range_by_uuid128(characteristic_discovery_callback, channel,
                                                                        slave.service_console.start_group_handle,
                                                                        slave.service_console.end_group_handle,
                                                                        UUID_ING_GENERIC_OUTPUT);
            break;
        }
        else
            gatt_client_discover_characteristic_descriptors(descriptor_discovery_callback, channel, &slave.output_char);
        break;
    }
}

void service_discovery_callback(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_SERVICE_QUERY_RESULT:
        {
            slave.service_console = gatt_event_service_query_result_parse(packet)->service;
            dbg_printf("svc handle: %d %d\n", slave.service_console.start_group_handle, slave.service_console.end_group_handle);
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (slave.service_console.start_group_handle != INVALID_HANDLE)
        {
            gatt_client_discover_characteristics_for_handle_range_by_uuid128(characteristic_discovery_callback, channel,
                                                                        slave.service_console.start_group_handle,
                                                                        slave.service_console.end_group_handle,
                                                                        UUID_ING_GENERIC_INPUT);
            slave.input_char.value_handle  = INVALID_HANDLE;
            slave.output_char.value_handle = INVALID_HANDLE;
        }
        else
        {
            dbg_printf("TODO: service not found, disc\n");
            gap_disconnect(channel);
            slave.conn_handle = INVALID_HANDLE;
        }
        break;
    }
}

void slave_connected(uint16_t conn_handle)
{
    slave.service_console.start_group_handle = INVALID_HANDLE;
    slave.conn_handle = conn_handle;
    gatt_client_discover_primary_services_by_uuid128(service_discovery_callback, slave.conn_handle, UUID_ING_CONSOLE_SERVICE);
    dbg_printf(">> discovering\n");
}

static initiating_phy_config_t phy_configs[] =
{
    {
        .phy = PHY_1M,
        .conn_param =
        {
            .scan_int = 200,
            .scan_win = 200,
            .interval_min = 13,
            .interval_max = 13,
            .latency = 0,
            .supervision_timeout = 200,
            .min_ce_len = 22,
            .max_ce_len = 22
        }
    },
    {
        .phy = PHY_CODED,
        .conn_param =
        {
            .scan_int = 200,
            .scan_win = 200,
            .interval_min = 20,
            .interval_max = 20,
            .latency = 0,
            .supervision_timeout = 200,
            .min_ce_len = 10,
            .max_ce_len = 10
        }
    }
};

#define USER_MSG_WRITE_DATA                 0

int is_triggering = 0;
void trigger_write(int flush)
{
    if (INVALID_HANDLE == slave.input_char.value_handle)
        return;

    if (is_triggering && (0 == flush))
        return;

    is_triggering = 1;
    btstack_push_user_msg(USER_MSG_WRITE_DATA, NULL, flush);
}

static void flush_timer_callback(TimerHandle_t xTimer)
{
    trigger_write(1);
}

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MSG_WRITE_DATA:
        {
            int flush = size;
            xTimerReset(flush_timer, portMAX_DELAY);
            do_write(flush);
            is_triggering = 0;
        }
        break;
    }
}

bd_addr_t peer_addr;

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
        gap_set_random_device_address(pair_config.master.ble_addr);
        gap_set_ext_scan_para(BD_ADDR_TYPE_LE_RANDOM, SCAN_ACCEPT_ALL_EXCEPT_NOT_DIRECTED,
                              sizeof(configs) / sizeof(configs[0]),
                              configs);
        gap_set_ext_scan_enable(1, 0, 0, 0);   // start continuous scanning
        dbg_printf("scanning ...\n");
        show_state(STATE_SCANNING);
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT:
            {
                const le_ext_adv_report_t *report = decode_hci_le_meta_event(packet, le_meta_event_ext_adv_report_t)->reports;

                reverse_bd_addr(report->address, peer_addr);

                if (memcmp(peer_addr, slave.addr, 6) == 0)
                {
                    gap_set_ext_scan_enable(0, 0, 0, 0);

                    dbg_printf(">> connecting to salve ...\n");
                    show_state(STATE_CONNECTING);

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
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:
            if (decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t)->status)
                platform_reset();
            else
                gap_set_phy(0, 0, PHY_2M_BIT, PHY_2M_BIT, HOST_NO_PREFERRED_CODING);
            break;
        case HCI_SUBEVENT_LE_PHY_UPDATE_COMPLETE:
            show_state(STATE_DISCOVERING);
            slave_connected(0);
            xTimerReset(flush_timer, portMAX_DELAY);
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        platform_printf("Total Rx over BLE: %u\n", total_rx);
        vTaskDelay(100);
        platform_reset();
        break;

    case L2CAP_EVENT_CAN_SEND_NOW:
        do_write(0);
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
    platform_printf("setup_profile\n");
    init_buffer();
    flush_timer = xTimerCreate("t1",
                            pdMS_TO_TICKS(800),
                            pdTRUE,
                            NULL,
                            flush_timer_callback);
    slave.addr = pair_config.slave.ble_addr;
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}
