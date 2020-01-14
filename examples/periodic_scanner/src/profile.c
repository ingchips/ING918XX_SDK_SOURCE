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

#include "service_ibeacon.h"

// GATT characteristic handles

uint8_t prd_create_sync = 0;
uint8_t prd_adv_data_acc[1650] = {0};
uint8_t prd_adv_data_offset = 0;

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

static const bd_addr_t target_addr = {0xC0, 0xFD, 0xDB, 0x54, 0x2A, 0xD2};

static int8_t last_power = -50;

#pragma pack (push, 1)
typedef struct temperature_service_data_adv
{
    uint16_t service_uuid;
    uint8_t  meas_type;
    uint32_t meas_value;
} temperature_service_data_adv_t;
#pragma pack (pop)

const char *float_ieee_11073_val_to_repr(uint32_t val);

void show_adv(int8_t rssi)
{
    ad_context_t context;
    uint16_t length;
    ibeacon_adv_t *p_ibeacon;
    const temperature_service_data_adv_t *p_temperature;
    
    ad_iterator_init(&context, prd_adv_data_offset, prd_adv_data_acc);
    while (ad_iterator_has_more(&context))
    {
        switch (ad_iterator_get_data_type(&context))
        {
        case 0x16: // service data 16bit uuid
            length = ad_iterator_get_data_len(&context);
            if (length != sizeof(temperature_service_data_adv_t))
                break;
            p_temperature = (const temperature_service_data_adv_t *)ad_iterator_get_data(&context);
            if (p_temperature->service_uuid != 0x1809)
                break;
            platform_printf("temperature = %s Deg\n", float_ieee_11073_val_to_repr(p_temperature->meas_value));
            break;
        case 0xff:
            p_ibeacon = (ibeacon_adv_t *)ad_data_from_type(prd_adv_data_offset, (uint8_t *)prd_adv_data_acc, 0xff, &length);

            if ((length != sizeof(ibeacon_adv_t))
                || (p_ibeacon->apple_id != APPLE_COMPANY_ID)
                || (p_ibeacon->id != IBEACON_ID))
                return;

            last_power = rssi / 2 + last_power / 2;
            print_uuid(p_ibeacon->uuid);
            platform_printf(" %04X,%04X, %.1fm, %ddBm\n", 
                    p_ibeacon->major, p_ibeacon->minor,
                    estimate_distance(p_ibeacon->ref_power, last_power), last_power);
            break;
        }
        ad_iterator_next(&context);
    }
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
        gap_add_whitelist(target_addr, BD_ADDR_TYPE_LE_RANDOM);
        gap_set_ext_scan_para(BD_ADDR_TYPE_LE_PUBLIC, SCAN_ACCEPT_WLIST_EXCEPT_NOT_DIRECTED,
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

                printf("%02X:%02X:%02X:%02X:%02X:%02X: %dBm\n",
                        report->address[5], report->address[4], report->address[3], report->address[2], report->address[1], report->address[0],
                        report->rssi);
                if ((0 == prd_create_sync) && (report->prd_adv_interval))
                {
                    printf("create sync...\n");
                    prd_create_sync = 1;
                    gap_periodic_adv_create_sync(PERIODIC_ADVERTISER_FROM_PARAM,
                                                 report->sid,
                                                 BD_ADDR_TYPE_LE_RANDOM, target_addr,
                                                 0,
                                                 1000,  // 10s
                                                 0);
                }
            }
            break;
        case HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED:
            printf("established\n");
            gap_set_ext_scan_enable(0, 0, 0, 0); 
            prd_adv_data_offset = 0;
            break;
        
        case HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_REPORT:
            {
                const le_meta_event_periodic_adv_report_t *report = 
                    decode_hci_le_meta_event(packet, le_meta_event_periodic_adv_report_t);
                if (prd_adv_data_offset + report->data_length <= sizeof(prd_adv_data_acc))
                {
                    memcpy(prd_adv_data_acc + prd_adv_data_offset, report->data, report->data_length);
                    prd_adv_data_offset += report->data_length;
                }
                switch (report->data_status)
                {
                case HCI_PRD_ADV_DATA_STATUS_CML:
                    show_adv(report->rssi);
                    prd_adv_data_offset = 0;
                    break;
                case HCI_PRD_ADV_DATA_STATUS_HAS_MORE:
                    break;
                case HCI_PRD_ADV_DATA_STATUS_TRUNCED:
                    prd_adv_data_offset = 0;
                    break;
                }                
            }
            break;
        
        case HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_LOST:
            prd_create_sync = 0;
            printf("sync lost\n");
            gap_set_ext_scan_enable(1, 0, 0, 0);   // start continuous scanning
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

