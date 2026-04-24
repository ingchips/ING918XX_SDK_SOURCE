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

static uint8_t scan_data[] = { 4, 0xFF, 0xAC, 0x06, 0x01 };

const static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

static uint8_t prd_create_sync = 0;
static int response_counting_down = 10;
static hci_con_handle_t conn_handle = INVALID_HANDLE;
static uint16_t sync_handle = INVALID_HANDLE;

static uint8_t data_counter = 0x01;

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

static const scan_phy_config_t configs[2] =
{
    {
        .phy = PHY_1M,
        .type = SCAN_PASSIVE,
        .interval = 200,
        .window = 100
    },
    {
        .phy = PHY_CODED,
        .type = SCAN_PASSIVE,
        .interval = 200,
        .window = 100
    }
};

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
        gap_set_random_device_address(scanner_addr);
        gap_add_whitelist(advertiser_addr, BD_ADDR_TYPE_LE_RANDOM);
        gap_set_ext_scan_para(BD_ADDR_TYPE_LE_RANDOM, SCAN_ACCEPT_WLIST_EXCEPT_NOT_DIRECTED,
                              sizeof(configs) / sizeof(configs[0]),
                              configs);
        gap_set_ext_scan_enable(1, 0, 0, 0);   // start continuous scanning
        break;

    case HCI_EVENT_COMMAND_COMPLETE:
        switch (hci_event_command_complete_get_command_opcode(packet))
        {
        // add your code to check command complete response
        // case :
        //     break;
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

                platform_printf("%02X:%02X:%02X:%02X:%02X:%02X: %dBm\n",
                        report->address[5], report->address[4], report->address[3], report->address[2], report->address[1], report->address[0],
                        report->rssi);
                if ((0 == prd_create_sync) && (report->prd_adv_interval))
                {
                    platform_printf("create sync...\n");
                    prd_create_sync = 1;
                    gap_periodic_adv_create_sync(PERIODIC_ADVERTISER_FROM_PARAM,
                                                 report->sid,
                                                 BD_ADDR_TYPE_LE_RANDOM, advertiser_addr,
                                                 0,
                                                 1000,  // 10s
                                                 0);
                }
            }
            break;
        case HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED_V2:
            {
                static const uint8_t subevents_to_sync[] = {0, 1};
                const le_meta_event_periodic_adv_sync_established_v2_t *established =
                    decode_hci_le_meta_event(packet, le_meta_event_periodic_adv_sync_established_v2_t);

                prd_create_sync = 0;

                if (established->status != 0)
                {
                    platform_printf("failed to establish sync (%d), restart ........................\n", established->status);
                    platform_reset();
                }

                sync_handle = established->handle;

                platform_printf("ESTABLISHED: num_subevents = %d\n",
                                established->num_subevents);
                gap_set_ext_scan_enable(0, 0, 0, 0);
                if (established->num_subevents >= sizeof(subevents_to_sync))
                {
                    gap_set_periodic_sync_subevent(established->handle,
                                                    PERIODIC_ADV_BIT_INC_TX,
                                                    sizeof(subevents_to_sync),
                                                    subevents_to_sync);
                }
            }
            break;
        case HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_REPORT_V2:
            {
                const le_meta_event_periodic_adv_report_v2_t *report =
                    decode_hci_le_meta_event(packet, le_meta_event_periodic_adv_report_v2_t);
                platform_printf("SUBEVT REPORT (#%d)\n"
                                "   subevent = %d\n"
                                "   data     : ",
                                report->periodic_event_counter,
                                report->subevent);
                printf_hexdump(report->data, report->data_length);

                scan_data[DATA_COUNTER_OFFSET] = ++data_counter;
                if (0 == report->subevent)
                {
                    gap_set_periodic_adv_rsp_data(sync_handle,
                                                    report->periodic_event_counter,
                                                    report->subevent,
                                                    1,
                                                    0,
                                                    sizeof(scan_data),
                                                    scan_data);
                }
            }
            break;
        case HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_LOST:
            platform_printf("SYNC LOST, restart ........................\n");
            platform_reset();
            break;
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:
            {
                const le_meta_event_enh_create_conn_complete_v2_t *complete =
                    decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_v2_t);
                if (0 == complete->status)
                {
                    platform_printf("CONNECTED\n");
                    att_set_db(complete->handle, profile_data);
                }
                else
                {
                    platform_printf("Initiating from PAwR failed\n");
                    response_counting_down = 10;
                }
            }

            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
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
    platform_printf("PAwR SCANNER started\n");
    // Note: security has not been enabled.
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

