#include <stdio.h>
#include <string.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"

#include "sig_uuid.h"
#include "att_db_util.h"
#include "dev_info.h"

#include "ble_qiot_export.h"

#include "hal.h"

const bd_addr_t rand_addr = { 0xE4, 0x75, 0x03, 0xE4, 0xEE, 0x89 };

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
    if (att_handle == qcloud_char_defs[Q_CHAR_DEVICE_INFO].handle)
    {
        qcloud_char_defs[Q_CHAR_DEVICE_INFO].on_write(buffer, buffer_size);
        return 0;
    }
    else if (att_handle == qcloud_char_defs[Q_CHAR_DATA].handle)
    {
        qcloud_char_defs[Q_CHAR_DATA].on_write(buffer, buffer_size);
        return 0;
    }
    else if (att_handle == qcloud_char_defs[Q_CHAR_EVENT].handle + 1)
    {
        qcloud_char_defs[Q_CHAR_EVENT].config = *(uint16_t *)buffer;
        return 0;
    }
    
    return 0;
}

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MESSAGE_STACK_TIMER:
        invoke_stack_timer_cb(size); // size is timer ID
        break;
    default:
        ;
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
        gap_set_adv_set_random_addr(0, rand_addr);
        ble_qiot_advertising_start();
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
            att_set_db(decode_hci_le_meta_event(packet, le_meta_event_create_conn_complete_t)->handle,
                       att_db_util_get_address());
            ble_gap_connect_cb();
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        ble_gap_disconnect_cb();
        ble_qiot_advertising_start();
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

static uint8_t att_db_storage[300];

void init_service()
{
    att_db_util_init(att_db_storage, sizeof(att_db_storage));

    att_db_util_add_service_uuid16(GAP_SERVICE_UUID);
    att_db_util_add_characteristic_uuid16(GAP_DEVICE_NAME_UUID, ATT_PROPERTY_READ, (uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME));
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

uint32_t setup_profile(void *data, void *user_data)
{
    platform_printf("setup profile\n");

    init_service();
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);

    ble_qiot_explorer_init();
    return 0;
}

