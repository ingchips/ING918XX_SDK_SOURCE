#include "btstack_helper.h"

#include "platform_api.h"
#include "btstack_defines.h"
#include "btstack_event.h"
#include "gap.h"

uint8_t bt_helper_set_random_address(const uint8_t *address)
{
    uint8_t r = 0;
    r |= gap_set_random_device_address(address);
    r |= gap_set_adv_set_random_addr(BT_HELPER_DEFAULT_ADV_SET, address);
    return r;
}

void bt_helper_set_public_address(const uint8_t *address)
{
    bd_addr_t addr;
    reverse_bd_addr(address, addr);
    sysSetPublicDeviceAddr(addr);
}

static btstack_packet_handler_t hci_callback = NULL;
static uint8_t hci_event_copy[300] = {0};

#define copy_data()     do { flag = 1; memcpy(hci_event_copy, packet, size); } while (0)

#define reverse_addr(type, field)       do { \
        const type *_original = decode_hci_le_meta_event(packet, type);         \
        type *_copied = (type *)decode_hci_le_meta_event(hci_event_copy, type);   \
        reverse_bd_addr(_original->field, _copied->field);                      \
    } while (0)

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t flag = 0;
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    if (size > sizeof(hci_event_copy))
        platform_raise_assertion("bt_helper", __LINE__);

    switch (event)
    {
    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:
            copy_data();
            reverse_addr(le_meta_event_enh_create_conn_complete_t, peer_addr);
            reverse_addr(le_meta_event_enh_create_conn_complete_t, local_resolv_priv_addr);
            reverse_addr(le_meta_event_enh_create_conn_complete_t, peer_resolv_priv_addr);
            break;

        case HCI_SUBEVENT_LE_DIRECT_ADVERTISING_REPORT:
            copy_data();
            reverse_addr(le_meta_event_directed_adv_report_t, reports[0].address);
            reverse_addr(le_meta_event_directed_adv_report_t, reports[0].direct_addr);
            break;

        case HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT:
            copy_data();
            reverse_addr(le_meta_event_ext_adv_report_t, reports[0].address);
            reverse_addr(le_meta_event_ext_adv_report_t, reports[0].direct_addr);
            break;

        case HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED:
        case HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED_V2:
            copy_data();
            reverse_addr(le_meta_event_periodic_adv_sync_established_t, address);
            break;

        case HCI_SUBEVENT_LE_SCAN_REQUEST_RECEIVED:
            copy_data();
            reverse_addr(le_meta_event_scan_req_received_t, scanner_addr);
            break;

        case HCI_SUBEVENT_LE_PRD_ADV_SYNC_TRANSFER_RCVD:
        case HCI_SUBEVENT_LE_PRD_ADV_SYNC_TRANSFER_RCVD_V2:
            copy_data();
            reverse_addr(le_meta_event_prd_adv_sync_transfer_recv_t, addr);
            break;

        case HCI_SUBEVENT_LE_VENDOR_PRO_CONNECTIONLESS_IQ_REPORT:
            copy_data();
            reverse_addr(le_meta_event_pro_connless_iq_report_t, addr);
            break;

        default:
            break;
        }
        break;
    default:
        break;
    }

    hci_callback(HCI_EVENT_PACKET, BT_HELPER_HCI_CHANNEL, flag ? hci_event_copy : packet, size);
}

void bt_helper_hci_add_event_handler(btstack_packet_callback_registration_t *callback_handler)
{
    hci_callback = callback_handler->callback;
    callback_handler->callback = user_packet_handler;
    hci_add_event_handler(callback_handler);
}

int bt_helper_adv_start(const struct bt_le_adv_param *param,
		    const uint8_t *ad, size_t ad_len,
		    const uint8_t *sd, size_t sd_len)
{
    ext_adv_set_en_t adv_sets_en[] = { {.handle = param->id, .duration = 0, .max_events = 0} };
    adv_filter_policy_t policy = ADV_FILTER_ALLOW_ALL;
    if (param->options & BT_LE_ADV_OPT_FILTER_CONN)
    {
        policy = param->options & BT_LE_ADV_OPT_FILTER_SCAN_REQ ? ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST
                                                                : ADV_FILTER_ALLOW_SCAN_ALL_CON_WLST;
    }
    else
    {
        policy = param->options & BT_LE_ADV_OPT_FILTER_SCAN_REQ ? ADV_FILTER_ALLOW_SCAN_WLST_CON_ALL
                                                                : ADV_FILTER_ALLOW_ALL;
    }
    adv_event_properties_t properties = 0;
    if (param->peer)
        properties |= DIRECT_ADV_BIT;
    if (param->options & BT_LE_ADV_OPT_EXT_ADV)
    {
        if (param->options & BT_LE_ADV_OPT_CONNECTABLE)
            properties |= CONNECTABLE_ADV_BIT;
        if (param->options & BT_LE_ADV_OPT_SCANNABLE)
            properties |= SCANNABLE_ADV_BIT;
    }
    else
    {
        properties = LEGACY_PDU_BIT;

        if (param->options & BT_LE_ADV_OPT_SCANNABLE)
            properties |= SCANNABLE_ADV_BIT;
        if (param->options & BT_LE_ADV_OPT_CONNECTABLE)
        {
            properties |= CONNECTABLE_ADV_BIT;
            if (param->peer == NULL)
                properties |= SCANNABLE_ADV_BIT;
        }
    }

    gap_set_ext_adv_para(param->id,
                            properties,
                            param->interval_min, param->interval_max,
                            PRIMARY_ADV_ALL_CHANNELS,  // Primary_Advertising_Channel_Map
                            BD_ADDR_TYPE_LE_RANDOM,    // Own_Address_Type
                            param->peer ? param->peer->type : BD_ADDR_TYPE_LE_PUBLIC, // Peer_Address_Type (ignore)
                            param->peer ? param->peer->a : NULL,                      // Peer_Address      (ignore)
                            policy,                    // Advertising_Filter_Policy
                            0x00,                      // Advertising_Tx_Power
                            PHY_1M,                    // Primary_Advertising_PHY
                            0,                         // Secondary_Advertising_Max_Skip
                            PHY_1M,                    // Secondary_Advertising_PHY
                            0x00,                      // Advertising_SID
                            0x00);                     // Scan_Request_Notification_Enable
    gap_set_ext_adv_data(param->id, ad_len, (uint8_t*)ad);
    gap_set_ext_scan_response_data(param->id, sd_len, (uint8_t*)sd);
    gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);

    return 0;
}