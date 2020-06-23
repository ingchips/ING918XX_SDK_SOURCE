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

#include "ad_parser.h"

#include "../../anti_lost_sla/src/dev_info.h"

// GATT characteristic handles

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
        .type = SCAN_ACTIVE,
        .interval = 200,
        .window = 80
    },
    {
        .phy = PHY_CODED,
        .type = SCAN_ACTIVE,
        .interval = 200,
        .window = 80
    }
};

extern void adv_received(void);

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
        gap_set_random_device_address(dev_info.master);
        gap_add_whitelist(dev_info.slave, BD_ADDR_TYPE_LE_RANDOM);
        gap_set_ext_scan_para(BD_ADDR_TYPE_LE_RANDOM, SCAN_ACCEPT_WLIST_EXCEPT_NOT_DIRECTED,
                              sizeof(configs) / sizeof(configs[0]) - 1,
                              configs);
        gap_set_ext_scan_enable(1, 0, 0, 0);   // start continuous scanning
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT:
            {
                const le_ext_adv_report_t *report = decode_hci_le_meta_event(packet, le_meta_event_ext_adv_report_t)->reports;

                if (report->evt_type & HCI_EXT_ADV_PROP_SCAN_RSP)
                {
                    adv_received();
                    printf("%02X:%02X:%02X:%02X:%02X:%02X: %dBm\n",
                            report->address[5], report->address[4], report->address[3], report->address[2], report->address[1], report->address[0],
                            report->rssi);
                }
            }
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        // add your code
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

uint32_t setup_profile(void *data, void *user_data)
{   
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(user_packet_handler);
    gatt_client_register_handler(user_packet_handler);
    return 0;
}

