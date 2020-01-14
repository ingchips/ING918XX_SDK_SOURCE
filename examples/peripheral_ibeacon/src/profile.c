#include <stddef.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"

// GATT characteristic handles


const static uint8_t adv_data_1m[] = {
    #include "../data/advertising0.adv"
};

const static uint8_t adv_data_2m[] = {
    #include "../data/advertising_2m.adv"
};

const static uint8_t adv_data_coded125[] = {
    #include "../data/advertising_coded125.adv"
};

const static uint8_t adv_data_coded500[] = {
    #include "../data/advertising_coded500.adv"
};

const static uint8_t eddystone_url[] = {
    #include "../data/eddystone_url.adv"
};

/* // for weixin iBeacon
const static uint8_t adv_data_weixin[] = {
    #include "../data/advertising_2.adv"
};
*/

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

bd_addr_t null_addr = {0};

static void setup_adv_set(uint8_t adv_handle, 
                          phy_type_t primary_phy,
                          phy_type_t secondary_phy, 
                          const uint8_t *adv_data, const uint8_t adv_data_len,
                          uint16_t properties)
{
    gap_set_ext_adv_para(adv_handle, 
                            properties,
                            0x003a1, 0x003a1,          // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
                            PRIMARY_ADV_ALL_CHANNELS,  // Primary_Advertising_Channel_Map
                            BD_ADDR_TYPE_LE_RANDOM,    // Own_Address_Type
                            BD_ADDR_TYPE_LE_PUBLIC,    // Peer_Address_Type (ignore)
                            NULL,                      // Peer_Address      (ignore)
                            ADV_FILTER_ALLOW_ALL,      // Advertising_Filter_Policy
                            100,                       // Advertising_Tx_Power
                            primary_phy,               // Primary_Advertising_PHY
                            0,                         // Secondary_Advertising_Max_Skip
                            secondary_phy,             // Secondary_Advertising_PHY
                            adv_handle,                // Advertising_SID
                            0x00);                     // Scan_Request_Notification_Enable
    gap_set_ext_adv_data(adv_handle, adv_data_len, adv_data);
}

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const static ext_adv_set_en_t adv_sets_en[] = {
        {.handle = 0, .duration = 0, .max_events = 0},
        {.handle = 1, .duration = 0, .max_events = 0},
        {.handle = 2, .duration = 0, .max_events = 0},
        {.handle = 3, .duration = 0, .max_events = 0},
    };
    const static bd_addr_t rand_addr1 = {1,1,1,1,1,1};    // TODO: random address generation
    const static bd_addr_t rand_addr2 = {2,2,2,2,2,2};    // TODO: random address generation
    const static bd_addr_t rand_addr3 = {3,3,3,3,3,3};    // TODO: random address generation
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;

        gap_set_adv_set_random_addr(0, rand_addr1);
        gap_set_adv_set_random_addr(1, rand_addr2);
        gap_set_adv_set_random_addr(2, rand_addr1);
        gap_set_adv_set_random_addr(3, rand_addr2);
        gap_set_adv_set_random_addr(4, rand_addr3);
        
        setup_adv_set(0, PHY_1M, PHY_1M,
                         adv_data_1m, sizeof(adv_data_1m), 0);
        setup_adv_set(1, PHY_1M, PHY_2M,
                         adv_data_2m, sizeof(adv_data_2m), 0);        
        setup_adv_set(2, PHY_CODED, PHY_CODED,
                         adv_data_coded125, sizeof(adv_data_coded125), 0);
        setup_adv_set(3, PHY_CODED, PHY_CODED,
                         adv_data_coded500, sizeof(adv_data_coded500), 0);
        setup_adv_set(4, PHY_1M, PHY_1M,
                         eddystone_url, sizeof(eddystone_url), LEGACY_PDU_BIT);
        ll_set_adv_coded_scheme(3, BLE_CODED_S2);
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
            // add your code
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
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

