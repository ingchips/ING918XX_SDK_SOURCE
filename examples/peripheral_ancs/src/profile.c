#include <stdio.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "bluetooth_hci.h"

#include "sm.h"

#include "FreeRTOS.h"
#include "timers.h"

const sm_persistent_t sm_persistent =
{
    .er = {1, 2, 3},
    .ir = {4, 5, 6},
    .identity_addr_type     = BD_ADDR_TYPE_LE_RANDOM,
    .identity_addr          = {0xC3, 25, 34, 48, 52, 67}
};

#define SECURITY_PERSISTENT_DATA    (&sm_persistent) // ((const sm_persistent_t *)0x44000)
#define PRIVATE_ADDR_MODE           GAP_RANDOM_ADDRESS_OFF

#include "ancs.h"

// GATT characteristic handles
#include "../data/gatt.const"

const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

const static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

uint16_t conn_handle = 0xffff;
int paring = 0;
static TimerHandle_t app_timer = 0;

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

#define USER_MSG_KEY_PRESSED        0x00000001
#define USER_MSG_RSSI_TIMER         0x00000002

static void app_timer_callback(TimerHandle_t xTimer)
{
    btstack_push_user_msg(USER_MSG_RSSI_TIMER, NULL, 0);
}

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MSG_KEY_PRESSED:
        {
            uint32_t mask = (uint32_t)(data);
            if (mask & 1) ancs_user_action(ANCS_ACTION_ID_POSITIVE);
            if (mask & 2) ancs_user_action(ANCS_ACTION_ID_NEGATIVE);
        }
        break;
    case USER_MSG_RSSI_TIMER:
        gap_read_rssi(conn_handle);
        break;
    default:
        ;
    }
}

void key_pressed(uint32_t keys_mask)
{
    btstack_push_user_msg(USER_MSG_KEY_PRESSED, (void *)(keys_mask), 1);
}

const static ext_adv_set_en_t adv_sets_en[] = { {.handle = 0, .duration = 0, .max_events = 0} };

static void setup_adv(void)
{
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

extern void connection_changed(int connected);

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
#if (PRIVATE_ADDR_MODE == GAP_RANDOM_ADDRESS_OFF)
        if (SECURITY_PERSISTENT_DATA->identity_addr_type != BD_ADDR_TYPE_LE_PUBLIC)
            gap_set_adv_set_random_addr(0, SECURITY_PERSISTENT_DATA->identity_addr);
        setup_adv();
#else
        sm_private_random_address_generation_set_mode(PRIVATE_ADDR_MODE);
#endif
        break;

    case HCI_EVENT_COMMAND_COMPLETE:
        switch (hci_event_command_complete_get_command_opcode(packet))
        {
        case HCI_RD_RSSI_CMD_OPCODE:
            {
                const event_command_complete_return_param_read_rssi_t *cmpl =
                    (const event_command_complete_return_param_read_rssi_t *)hci_event_command_complete_get_return_parameters(packet);
                if (cmpl->rssi > -50)
                {
                    connection_changed(1);
                    xTimerStop(app_timer, portMAX_DELAY);
                }
            }
            break;
        default:
            break;
        }
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            conn_handle = decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t)->handle;
            att_set_db(conn_handle, profile_data);
            sm_send_security_request(conn_handle);
            break;
        default:
            break;
        }

        break;
    case HCI_EVENT_ENCRYPTION_CHANGE:
        {
            const hci_encryption_change_event_t *evt = decode_hci_event(packet, hci_encryption_change_event_t);
            if (!evt->enabled) break;
            if (paring)
                ancs_discover(conn_handle);
        }
        break;
    case HCI_EVENT_DISCONNECTION_COMPLETE:
        conn_handle = 0xffff;
        connection_changed(0);
        xTimerStop(app_timer, portMAX_DELAY);
        platform_printf("HCI_EVENT_DISCONNECTION_COMPLETE\n");
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
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

static void sm_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);
#if (PRIVATE_ADDR_MODE != GAP_RANDOM_ADDRESS_OFF)
    static uint8_t addr_ready = 0;
#endif

    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case SM_EVENT_PRIVATE_RANDOM_ADDR_UPDATE:
#if (PRIVATE_ADDR_MODE != GAP_RANDOM_ADDRESS_OFF)
        gap_set_adv_set_random_addr(0, sm_private_random_addr_update_get_address(packet));
        if (0 == addr_ready)
        {
            addr_ready = 1;
            setup_adv();
        }
#endif
        break;
    case SM_EVENT_JUST_WORKS_REQUEST:
        paring = 1;
        sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
        break;
    case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
        platform_printf("===================\npasskey: %06d\n===================\n",
            sm_event_passkey_display_number_get_passkey(packet));
        break;
    case SM_EVENT_PASSKEY_DISPLAY_CANCEL:
        platform_printf("TODO: DISPLAY_CANCEL\n");
        break;
    case SM_EVENT_PASSKEY_INPUT_NUMBER:
        // TODO: ask user to input number & call sm_passkey_input
        platform_printf("===================\ninput number:\n");
        break;
    case SM_EVENT_PASSKEY_INPUT_CANCEL:
        platform_printf("TODO: INPUT_CANCEL\n");
        break;
    case SM_EVENT_IDENTITY_RESOLVING_FAILED:
        platform_printf("not authourized\n");
        gap_disconnect(conn_handle);
        break;
    case SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED:
        xTimerReset(app_timer, portMAX_DELAY);
        ancs_discover(conn_handle);
        break;
    default:
        break;
    }
}

static btstack_packet_callback_registration_t sm_event_callback_registration  = {.callback = &sm_packet_handler};

uint32_t setup_profile(void *data, void *user_data)
{
    app_timer = xTimerCreate("t1",
                            pdMS_TO_TICKS(200),
                            pdTRUE,
                            NULL,
                            app_timer_callback);
    sm_add_event_handler(&sm_event_callback_registration);
    sm_config(1, IO_CAPABILITY_DISPLAY_ONLY,
              0,
              SECURITY_PERSISTENT_DATA);
    sm_set_authentication_requirements(SM_AUTHREQ_BONDING);
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

