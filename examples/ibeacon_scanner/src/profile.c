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

#include "att_db.h"

#include <math.h>

#include "service_ibeacon.h"

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

char str_buffer[80];

int8_t last_power = -50;

void blink(const uint8_t led_id);

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

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);
    uint16_t length;
    ibeacon_adv_t *p_ibeacon;
    uint8_t phy = 0;

    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;

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
                phy = report->s_phy;

                p_ibeacon = (ibeacon_adv_t *)ad_data_from_type(report->data_len, (uint8_t *)report->data, 0xff, &length);

                if ((length != sizeof(ibeacon_adv_t))
                    || (p_ibeacon->apple_id != APPLE_COMPANY_ID)
                    || (p_ibeacon->id != IBEACON_ID))
                    break;
                blink(phy);
                last_power = ((int8_t)report->rssi) / 2 + last_power / 2;
                print_uuid(p_ibeacon->uuid);
                platform_printf(" %04X,%04X, %.1fm, %ddBm\n", 
                        p_ibeacon->major, p_ibeacon->minor,
                        estimate_distance(p_ibeacon->ref_power, last_power), last_power);
            }
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        // add your code
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        // add your code
        break;

    default:
        break;
    }
}

uint32_t setup_profile(void *data, void *user_data)
{   
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

