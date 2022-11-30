#include <stdio.h>
#include <string.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "l2cap.h"
#include "att_dispatch.h"
#include "btstack_util.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "gatt_client.h"
#include "sig_uuid.h"
#include "profile.h"

#include "ingsoc.h"
#include "peripheral_rtc.h"

#include "ad_parser.h"
#include "uart_console.h"

#include "FreeRTOS.h"
#include "timers.h"
#include "log_util.h"

#define INVALID_HANDLE      (0xffff)
const uint8_t UUID_TPT[]            = {0x24,0x45,0x31,0x4a,0xa1,0xd4,0x48,0x74,0xb4,0xd1,0xfd,0xfb,0x6f,0x50,0x14,0x85};
const uint8_t UUID_CHAR_GEN_IN[]    = {0xbf,0x83,0xf3,0xf1,0x39,0x9a,0x41,0x4d,0x90,0x35,0xce,0x64,0xce,0xb3,0xff,0x67};
const uint8_t UUID_CHAR_GEN_OUT[]   = {0xbf,0x83,0xf3,0xf2,0x39,0x9a,0x41,0x4d,0x90,0x35,0xce,0x64,0xce,0xb3,0xff,0x67};

void print_addr(const uint8_t *addr);

void hw_timer_restart(void);
void hw_timer_clear_int(void);
void hw_timer_stop(void);

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
    uint8_t     loopback;
} slave_info_t;

slave_info_t slave = {.conn_handle = INVALID_HANDLE};
static TimerHandle_t app_timer = 0;

#define LOOPBACK_TEST_SIZE  516
#define LOOPBACK_DATA       ((const uint8_t *)(0x4000))
typedef struct loopback_info
{
    uint32_t counter;
    uint64_t start;
    int rx_len;
} loopback_info_t;

loopback_info_t loopback_info = {0};

void loopback_send(void)
{
    const uint8_t *data = LOOPBACK_DATA;
    uint16_t mtu;
    int remain = LOOPBACK_TEST_SIZE;
    if ((slave.conn_handle == INVALID_HANDLE) || (0 == slave.loopback))
        return;

    loopback_info.start = platform_get_us_time();
    loopback_info.rx_len = 0;
    gatt_client_get_mtu(slave.conn_handle, &mtu);
    mtu -= 3;
    while (remain > 0)
    {
        int len = remain > mtu ? mtu : remain;
        uint8_t r = gatt_client_write_value_of_characteristic_without_response(slave.conn_handle,
                            slave.input_char.value_handle, len, (uint8_t *)data);
        if (r != 0)
        {
            LOG_ERROR("error");
            break;
        }
        remain -= len;
        data += len;
    }
}

void loopback_rx(const uint8_t *buffer, const uint16_t size)
{
    const uint8_t *data = LOOPBACK_DATA + loopback_info.rx_len;
    if (memcmp(data, buffer, size) != 0)
        LOG_ERROR("data mismatch");
    loopback_info.rx_len += size;
    if (loopback_info.rx_len >= LOOPBACK_TEST_SIZE)
    {
        uint32_t duration = platform_get_us_time() - loopback_info.start;
        LOG_MSG("%04d: %.2f", loopback_info.counter, duration / 1000.);
        xTimerStart(app_timer, portMAX_DELAY);
        loopback_info.counter++;
        gap_read_rssi(slave.conn_handle);
    }
}

void reset_info(void)
{
    slave.conn_handle                       = INVALID_HANDLE;
    slave.service_tpt.start_group_handle    = INVALID_HANDLE;
    slave.input_char.value_handle           = INVALID_HANDLE;
    slave.output_char.value_handle          = INVALID_HANDLE;
    slave.output_desc.handle                = INVALID_HANDLE;
    slave.m2s_run = 0;
    slave.s2m_run = 0;
    slave.loopback = 0;
}

uint32_t get_sig_short_uuid(const uint8_t *uuid128)
{
    return uuid_has_bluetooth_prefix(uuid128) ? big_endian_read_32(uuid128, 0) : 0;
}

static void output_notification_handler(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    const gatt_event_value_packet_t *value_packet;
    uint16_t value_size;
    switch (packet[0])
    {
    case GATT_EVENT_NOTIFICATION:
        value_packet = gatt_event_notification_parse(packet, size, &value_size);
        if (0 == slave.loopback)
        {
            slave.s2m_total += value_size;
        }
        else
            loopback_rx(value_packet->value, value_size);
        break;
    }
}

void btstack_callback(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_QUERY_COMPLETE:
        if (gatt_event_query_complete_parse(packet)->status != 0)
            return;
        LOG_INFO("cmpl");
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
        {
            const gatt_event_all_characteristic_descriptors_query_result_t *result =
                gatt_event_all_characteristic_descriptors_query_result_parse(packet);
            if (get_sig_short_uuid(result->descriptor.uuid128) ==
                SIG_UUID_DESCRIP_GATT_CLIENT_CHARACTERISTIC_CONFIGURATION)
            {
                slave.output_desc = result->descriptor;
                LOG_INFO("output desc: %d", slave.output_desc.handle);
            }
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (gatt_event_query_complete_parse(packet)->status != 0)
            break;

        if (slave.output_desc.handle != INVALID_HANDLE)
        {
            gatt_client_listen_for_characteristic_value_updates(&slave.output_notify, output_notification_handler,
                                                                slave.conn_handle, slave.output_char.value_handle);
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
            if (memcmp(result->characteristic.uuid128, UUID_CHAR_GEN_IN, sizeof(UUID_CHAR_GEN_OUT)) == 0)
            {
                slave.input_char = result->characteristic;
                LOG_INFO("input handle: %d", slave.input_char.value_handle);
            }
            else if (memcmp(result->characteristic.uuid128, UUID_CHAR_GEN_OUT, sizeof(UUID_CHAR_GEN_OUT)) == 0)
            {
                slave.output_char = result->characteristic;
                LOG_INFO("output handle: %d", slave.output_char.value_handle);
            }
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (gatt_event_query_complete_parse(packet)->status != 0)
            break;

        if (INVALID_HANDLE == slave.input_char.value_handle)
        {
            LOG_ERROR("characteristic not found, disc");
            gap_disconnect(slave.conn_handle);
        }
        else
        {
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
        {
            slave.service_tpt = gatt_event_service_query_result_parse(packet)->service;
            LOG_INFO("service handle: %d %d",
                    slave.service_tpt.start_group_handle, slave.service_tpt.end_group_handle);
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        if (gatt_event_query_complete_parse(packet)->status != 0)
            break;
        if (slave.service_tpt.start_group_handle != INVALID_HANDLE)
        {
            gatt_client_discover_characteristics_for_service(characteristic_discovery_callback, slave.conn_handle,
                                                           slave.service_tpt.start_group_handle,
                                                           slave.service_tpt.end_group_handle);
        }
        else
        {
            LOG_ERROR("service not found, disc");
            gap_disconnect(slave.conn_handle);
        }
        break;
    }
}

uint16_t send_packet_len = 0;

void send_data(void)
{
    uint8_t r;
    if (0 == slave.m2s_run)
        return;

    slave.m2s_paused = 0;

    gatt_client_get_mtu(slave.conn_handle, &send_packet_len);
    send_packet_len -= 3;

    do
    {
        r = gatt_client_write_value_of_characteristic_without_response(slave.conn_handle,
                                                                       slave.input_char.value_handle,
                                                                       send_packet_len, (uint8_t *)0x4000);

        switch (r)
        {
            case 0:
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
#define USER_MSG_LOOPBACK               6
#define USER_MSG_LOOPBACK_SEND          7
#define USER_MSG_SET_PHY                8
#define USER_MSG_SET_INTERVAL           9

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MSG_START_M2S:
        slave.m2s_run = 1;
        slave.m2s_total = 0;
        send_data();
        hw_timer_restart();
        break;
    case USER_MSG_START_S2M:
        slave.s2m_run = 1;
        slave.s2m_total = 0;
        gatt_client_write_characteristic_descriptor_using_descriptor_handle(btstack_callback, slave.conn_handle,
            slave.output_desc.handle, sizeof(char_config_notification),
            (uint8_t *)&char_config_notification);
        hw_timer_restart();
        break;
    case USER_MSG_STOP_M2S:
        slave.m2s_run = 0;
        if ((slave.m2s_run | slave.m2s_run) == 0)
            hw_timer_stop();
        break;
    case USER_MSG_STOP_S2M:
        slave.s2m_run = 0;
        gatt_client_write_characteristic_descriptor_using_descriptor_handle(btstack_callback, slave.conn_handle,
            slave.output_desc.handle, sizeof(char_config_none),
            (uint8_t *)&char_config_none);
        if ((slave.m2s_run | slave.m2s_run) == 0)
            hw_timer_stop();
        break;
    case USER_MSG_SHOW_TPT:
        if (slave.m2s_run)
        {
            if (slave.m2s_paused) send_data();
            LOG_MSG("M->S: %dbps", slave.m2s_total << 3);
        }
        if (slave.s2m_run)
            LOG_MSG("S->M: %dbps", slave.s2m_total << 3);

#ifdef USE_DISPLAY
        app_status.m2s_bps = slave.m2s_run ? (int)slave.m2s_total << 3 : -1;
        app_status.s2m_bps = slave.s2m_run ? (int)slave.s2m_total << 3 : -1;
        app_state_updated();
#endif

        slave.m2s_total = 0;
        slave.s2m_total = 0;
        break;
    case USER_MSG_LOOPBACK:
        if (slave.m2s_run | slave.m2s_run)
        {
            LOG_ERROR("tpt test is on");
        }
        if (slave.loopback == size)
            break;
        slave.loopback = size;
        if (slave.loopback)
        {
            LOG_MSG("\n+=======================+\n"
                      "| start loopback test.  |\n"
                      "| RTT measured in ms.   |\n"
                      "+=======================+");
            loopback_info.counter = 0;
            gatt_client_write_characteristic_descriptor_using_descriptor_handle(btstack_callback, slave.conn_handle,
                    slave.output_desc.handle, sizeof(char_config_notification),
                    (uint8_t *)&char_config_notification);
            xTimerStart(app_timer, portMAX_DELAY);
        }
        else
        {
            gatt_client_write_characteristic_descriptor_using_descriptor_handle(btstack_callback, slave.conn_handle,
                    slave.output_desc.handle, sizeof(char_config_none),
                    (uint8_t *)&char_config_none);
            xTimerStop(app_timer, portMAX_DELAY);
        }
        break;
    case USER_MSG_LOOPBACK_SEND:
        loopback_send();
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
            gap_set_phy(slave.conn_handle, 0, phy_bit, phy_bit, phy_opt);
        }
        break;
    case USER_MSG_SET_INTERVAL:
        {
            uint16_t interval = size;
            uint16_t ce_len = (interval << 1) - 2;
            gap_update_connection_parameters(slave.conn_handle, interval, interval,
                0, interval > 10 ? interval : 10, // supervisor_timeout = max(100, interval * 8)
                ce_len, ce_len);
        }
        break;
    default:
        ;
    }
}

static void app_timer_callback(TimerHandle_t xTimer)
{
    btstack_push_user_msg(USER_MSG_LOOPBACK_SEND, NULL, 0);
}

uint32_t timer_isr(void *user_data)
{
    hw_timer_clear_int();

    if (INVALID_HANDLE == slave.output_desc.handle)
        return 0;
    btstack_push_user_msg(USER_MSG_SHOW_TPT, NULL, 0);
    return 0;
}

#define CHECK_STATE()   \
    if (INVALID_HANDLE == slave.output_desc.handle) \
    {                                               \
        LOG_ERROR("not ready");                   \
        return;                                     \
    }

void loopback_test(int start1stop0)
{
    CHECK_STATE();
    btstack_push_user_msg(USER_MSG_LOOPBACK, NULL, start1stop0);
}

void set_phy(int phy)
{
    CHECK_STATE();
    btstack_push_user_msg(USER_MSG_SET_PHY, NULL, phy);
}

void set_interval(int interval)
{
    CHECK_STATE();
    btstack_push_user_msg(USER_MSG_SET_INTERVAL, NULL, interval);
}

void start_tpt(tpt_dir_t dir)
{
    CHECK_STATE();

    if (DIR_M_TO_S == dir)
        btstack_push_user_msg(USER_MSG_START_M2S, NULL, 0);
    else if (DIR_S_TO_M == dir)
        btstack_push_user_msg(USER_MSG_START_S2M, NULL, 0);
}

void stop_tpt(tpt_dir_t dir)
{
    CHECK_STATE();

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
            .supervision_timeout = 600,
            .min_ce_len = 90,
            .max_ce_len = 90
        }
    }
};

bd_addr_t rand_addr = {0xC0, 0x00, 0x00, 0x11, 0x11, 0x11};
bd_addr_t peer_addr;

#define OGF_STATUS_PARAMETERS       0x05
#define OPCODE(ogf, ocf)            (ocf | ogf << 10)
#define OPCODE_READ_RSSI            OPCODE(OGF_STATUS_PARAMETERS, 0x05)

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
        gap_set_random_device_address(rand_addr);
        gap_set_ext_scan_para(BD_ADDR_TYPE_LE_RANDOM, SCAN_ACCEPT_ALL_EXCEPT_NOT_DIRECTED,
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
                    LOG_MSG("connecting ...");  print_addr(peer_addr);
#ifdef USE_DISPLAY
                    memcpy(app_status.peer, peer_addr, sizeof(app_status.peer));
#endif

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
            {
                const le_meta_event_enh_create_conn_complete_t *conn_complete
                     = decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);
                if (conn_complete->status)
                    platform_reset();
                LOG_MSG("connected");
                slave.conn_handle = conn_complete->handle;
                gap_set_phy(slave.conn_handle, 0, PHY_2M_BIT, PHY_2M_BIT, HOST_NO_PREFERRED_CODING);
                gatt_client_discover_primary_services_by_uuid128(service_discovery_callback, conn_complete->handle, UUID_TPT);
#ifdef USE_DISPLAY
                app_status.connected = 1;
                app_state_updated();
#endif
            }
            break;
        case HCI_SUBEVENT_LE_PHY_UPDATE_COMPLETE:
            {
                const le_meta_phy_update_complete_t *cmpl = decode_hci_le_meta_event(packet, le_meta_phy_update_complete_t);
                LOG_MSG("PHY updated: Rx %d, Tx %d", cmpl->rx_phy, cmpl->tx_phy);
            }
            break;
        case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE:
            {
                const le_meta_event_conn_update_complete_t *cmpl = decode_hci_le_meta_event(packet, le_meta_event_conn_update_complete_t);
                LOG_MSG("CONN updated: interval %.2f ms", cmpl->interval * 1.25);
            }
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_COMMAND_COMPLETE:
        {
            if (hci_event_command_complete_get_command_opcode(packet) == OPCODE_READ_RSSI)
            {
                const event_command_complete_return_param_read_rssi_t *cmpl =
                    (const event_command_complete_return_param_read_rssi_t *)hci_event_command_complete_get_return_parameters(packet);
                LOG_MSG("RSSI: %ddBm", cmpl->rssi);
            }
        }
        break;

    case HCI_EVENT_NUMBER_OF_COMPLETED_PACKETS:
        if (slave.m2s_run)
        {
            const event_num_of_complete_packets_t *report = decode_hci_event(packet, event_num_of_complete_packets_t);
            slave.m2s_total += send_packet_len * report->complete_packets[0].num_of_packets;
        }
        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        reset_info();
        gap_set_ext_scan_enable(1, 0, 0, 0);   // start continuous scanning
#ifdef USE_DISPLAY
        app_status.connected = 0;
        app_state_updated();
#endif
        break;

    case GATT_EVENT_MTU:
        LOG_MSG("GATT client MTU updated: %d\n", gatt_event_mtu_get_mtu(packet));
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
    app_timer = xTimerCreate("t1",
                            pdMS_TO_TICKS(1000),
                            pdFALSE,
                            NULL,
                            app_timer_callback);
    reset_info();
    hci_event_callback_registration.callback = user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(user_packet_handler);
    gatt_client_register_handler(user_packet_handler);
    return 0;
}
