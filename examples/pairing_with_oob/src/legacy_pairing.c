#include <stdio.h>
#include <string.h>
#include "profile.h"
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "sm.h"
#include "gatt_client_util.h"

#define SLAVE_ADDR          {0xFA, 0x69, 0x77, 0xCC, 0xC2, 0x7E}

#if (ROLE == HCI_ROLE_SLAVE)

// GATT characteristic handles
#include "../data/gatt.const"

const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

#include "../data/advertising.const"

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

#include "../data/scan_response.const"

const static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

const sm_persistent_t sm_persistent =
{
    .er = {9, 1, 8},
    .ir = {9, 1, 6},
    .identity_addr_type     = BD_ADDR_TYPE_LE_RANDOM,
    .identity_addr          = SLAVE_ADDR
};

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

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {
    case HANDLE_GENERIC_INPUT:
        // add your code
        return 0;

    default:
        return 0;
    }
}

#else

const sm_persistent_t sm_persistent =
{
    .er = {9, 1, 8},
    .ir = {9, 1, 6},
    .identity_addr_type     = BD_ADDR_TYPE_LE_RANDOM,
    .identity_addr          = {0xFA, 0x69, 0x77, 0xCC, 0xC2, 0x7F}
};

static const bd_addr_t peer_addr = SLAVE_ADDR;

static struct gatt_client_discoverer *discoverer = NULL;

#define iprintf platform_printf

#include "gatt_client_util.c"

#endif

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

#if (ROLE == HCI_ROLE_SLAVE)

const static ext_adv_set_en_t adv_sets_en[] = { {.handle = 0, .duration = 0, .max_events = 0} };

static void setup_adv(void)
{
    gap_set_adv_set_random_addr(0, sm_persistent.identity_addr);
    gap_set_ext_adv_para(0,
                            CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT,
                            0x00a1, 0x00a1,            // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
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
    gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
}

#else

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

static void setup_initiating(void)
{
     gap_set_random_device_address(sm_persistent.identity_addr);
     gap_ext_create_connection(INITIATING_ADVERTISER_FROM_PARAM, // Initiator_Filter_Policy,
                              BD_ADDR_TYPE_LE_RANDOM,           // Own_Address_Type,
                              BD_ADDR_TYPE_LE_RANDOM,           // Peer_Address_Type,
                              peer_addr,                        // Peer_Address,
                              sizeof(phy_configs) / sizeof(phy_configs[0]),
                              phy_configs);
}
#endif

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
#if (ROLE == HCI_ROLE_SLAVE)
        setup_adv();
#else
        setup_initiating();
#endif
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
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:
            {
                const le_meta_event_enh_create_conn_complete_t *complete =
                    decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);
                platform_printf("connected: %d\n", complete->status);
#if (ROLE == HCI_ROLE_SLAVE)
                att_set_db(complete->handle, profile_data);
                sm_request_pairing(complete->handle);
#else
                if (complete->status == 0)
                {
                    if (discoverer) gatt_client_util_free(discoverer);
                    discoverer = gatt_client_util_discover_all(complete->handle,
                        gatt_client_util_dump_profile,
                        NULL);
                }
                else
                {
                    setup_initiating();
                }
#endif
            }
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
#if (ROLE == HCI_ROLE_SLAVE)
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
#else
        setup_initiating();
#endif
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

static void sm_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);

    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case SM_EVENT_JUST_WORKS_REQUEST:
        platform_printf("JUST_WORKS?\n");
        sm_bonding_decline(sm_event_just_works_request_get_handle(packet));
        break;
    case SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED:
        break;
    case SM_EVENT_PASSKEY_INPUT_NUMBER:
        break;
    case SM_EVENT_STATE_CHANGED:
        {
            const sm_event_state_changed_t *state_changed = decode_hci_event(packet, sm_event_state_changed_t);
            switch (state_changed->reason)
            {
                case SM_STARTED:
                    platform_printf("SM: STARTED\n");
                    break;
                case SM_FINAL_PAIRED:
                    platform_printf("SM: PAIRED\n");
                    break;
                case SM_FINAL_REESTABLISHED:
                    platform_printf("SM: REESTABLISHED\n");
                    break;
                default:
                    platform_printf("SM: FINAL ERROR: %d\n", state_changed->reason);
                    break;
            }
        }
        break;
    default:
        break;
    }
}

static btstack_packet_callback_registration_t sm_event_callback_registration  = {.callback = &sm_packet_handler};
static btstack_packet_callback_registration_t hci_event_callback_registration;

static int get_oob_data_callback(uint8_t addres_type, bd_addr_t addr, uint8_t * oob_data)
{
    // random OOB data
    const static sm_key_t _oob_data = {
        52, 29, 200, 119, 171, 76, 33, 1, 56, 153, 91, 155, 26, 42, 243, 57
    };
    platform_printf("get_oob\n");
    memcpy(oob_data, _oob_data, sizeof(_oob_data));
    return 1;
}

uint32_t setup_profile(void *data, void *user_data)
{
    platform_printf("setup profile\n");
    sm_add_event_handler(&sm_event_callback_registration);
#if (ROLE == HCI_ROLE_SLAVE)
    att_server_init(att_read_callback, att_write_callback);
#endif
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    sm_config(1, IO_CAPABILITY_NO_INPUT_NO_OUTPUT,
              0,
              &sm_persistent);
    sm_set_authentication_requirements(SM_AUTHREQ_BONDING);
    sm_register_oob_data_callback(get_oob_data_callback);
    return 0;
}
