#include <stdio.h>
#include "profile.h"
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"

// GATT characteristic handles
#include "../data/gatt.const"

#include "addr.c"

static hci_con_handle_t conn_handle = INVALID_HANDLE;
static uint8_t is_initiating = 0;

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {

    default:
        return 0;
    }
}

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {

    default:
        return 0;
    }
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

const static ext_adv_set_en_t adv_sets_en[] = { {.handle = 0, .duration = 0, .max_events = 0} };

static uint8_t data0[] = {4, 0xFF, 0xAC, 0x06, 0x01 };
static uint8_t data1[] = {4, 0xFF, 0xAC, 0x06, 0x02 };

static uint8_t data_counter = 0x02;

static const gap_prd_adv_subevent_data_t subevents_data[] =
{
    {
        .subevent = 0,
        .rsp_slot_start = 0,
        .rsp_slot_count = 1,
        .data_len = sizeof(data0),
        .data = data0,
    },
    {
        .subevent = 1,
        .rsp_slot_start = 0,
        .rsp_slot_count = 1,
        .data_len = sizeof(data1),
        .data = data1,
    }
};

static const initiating_phy_config_t phy_config =
{
    .phy = PHY_1M,
    .conn_param =
    {
        .scan_int = 100,    // ignored
        .scan_win = 20,     // ignored
        .interval_min = 100,
        .interval_max = 100,
        .latency = 0,
        .supervision_timeout = 300,
        .min_ce_len = 3,
        .max_ce_len = 3,
    }
};

static void setup_adv(void)
{
    gap_set_ext_adv_para(0,
                            0,
                            0x0100, 0x0120,            // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
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

    gap_set_periodic_adv_para_v2(0,
                            800, 800,                   // interval
                            PERIODIC_ADV_BIT_INC_TX,    // properties
                            2,                          // num_subevents
                            100,                        // subevent_interval
                            1,                          // response_slot_delay
                            2,                          // response_slot_spacing
                            4);                         // num_response_slots

    gap_set_periodic_adv_subevent_data(0, 2, subevents_data);

    gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
    gap_set_periodic_adv_enable(1, 0);
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
        gap_set_adv_set_random_addr(0, advertiser_addr);
        setup_adv();
        break;

    case HCI_EVENT_COMMAND_COMPLETE:
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:
            {
                is_initiating = 0;
                const le_meta_event_enh_create_conn_complete_v2_t *complete =
                    decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_v2_t);
                if (0 == complete->status)
                {
                    conn_handle = complete->handle;
                    platform_printf("CONNECTED\n");
                }
                else
                    platform_printf("Initiating from PAwR failed\n");
            }
            break;
        case HCI_SUBEVENT_PRD_ADV_SUBEVT_DATA_REQ:
            {
                const le_mete_event_prd_adv_subevent_data_req_t *req =
                    decode_hci_le_meta_event(packet, le_mete_event_prd_adv_subevent_data_req_t);

                platform_printf("SUBEVT_DATA_REQ:\n"
                                "   adv_handle          = %d\n"
                                "   subevent_start      = %d\n"
                                "   subevent_data_count = %d\n",
                    req->adv_handle,
                    req->subevent_start,
                    req->subevent_data_count);

                int i;
                for (i = 0; i < req->subevent_data_count; i++)
                {
                    const gap_prd_adv_subevent_data_t *p = subevents_data + req->subevent_start + i;

                    ((uint8_t *)p->data)[DATA_COUNTER_OFFSET] = ++data_counter;
                    gap_set_periodic_adv_subevent_data(0, 1, p);
                }
            }
            break;
        case HCI_SUBEVENT_PRD_ADV_RSP_REPORT:
            {
                const le_mete_event_prd_adv_rsp_report_t *report =
                    decode_hci_le_meta_event(packet, le_mete_event_prd_adv_rsp_report_t);
                platform_printf("RSP_REPORT:\n"
                                "   subevent      = %d\n"
                                "   response_slot = %d\n"
                                "   data      : ",
                    report->subevent,
                    report->reports[0].response_slot);
                printf_hexdump(report->reports[0].data, report->reports[0].data_length);

                if ((INVALID_HANDLE == conn_handle) && (0 == is_initiating))
                {
                    uint8_t r = gap_ext_create_connection_v2(0,
                        report->subevent,
                        INITIATING_ADVERTISER_FROM_PARAM,
                        BD_ADDR_TYPE_LE_RANDOM,
                        BD_ADDR_TYPE_LE_RANDOM,
                        scanner_addr,
                        1,
                        &phy_config);

                    if (r == 0)
                        is_initiating = 1;
                }
            }
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        conn_handle = INVALID_HANDLE;
        platform_printf("DISCONNECTED\n");
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
    platform_printf("PAwR Advertiser started\n");
    // Note: security has not been enabled.
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

