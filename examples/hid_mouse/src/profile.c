#include <stdio.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "sm.h"
#include "btstack_util.h"
#include "btstack_event.h"
#include "btstack_defines.h"

#include "att_db_util.h"

#include "USBHID_Types.h"
#include <math.h>

// GATT characteristic handles
#define HANDLE_DEVICE_NAME                                   3
#define HANDLE_APPEARANCE                                    5


const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

typedef __packed struct mouse_report
{
    uint8_t buttons;
    int8_t x;
    int8_t y;
} mouse_report_t;

static int notify_enable = 0;
uint16_t att_handle_notify = 0;
hci_con_handle_t handle_send;

uint16_t att_handle_protocol_mode;
uint16_t att_handle_hid_ctrl_point;
uint16_t att_handle_report;
uint16_t att_handle_boot_mouse_input_report;

enum
{
    HID_PROTO_BOOT,
    HID_PROTO_REPORT
};
uint8_t protocol_mode = HID_PROTO_REPORT;

enum
{
    HID_CTRL_SUSPEND,
    HID_CTRL_EXIT_SUSPEND
};
uint8_t suspended = 0;

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, 
                                  uint8_t * buffer, uint16_t buffer_size)
{
    if (att_handle == att_handle_protocol_mode)
    {
        if (buffer)
            *buffer = protocol_mode;
        return sizeof(protocol_mode);
    }
    else;
    return 0;
}

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, 
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    handle_send = connection_handle;
    if (att_handle == att_handle_protocol_mode)
    {
        protocol_mode = *buffer;
    }
    else if (att_handle == att_handle_report + 1)
    {
        if(*(uint16_t *)buffer == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION)
        {
            notify_enable = 1;
            att_handle_notify = att_handle_report;
        }
        return 0;
    }
    else if (att_handle == att_handle_hid_ctrl_point)        
    {
        if (*buffer == HID_CTRL_SUSPEND)
            suspended = 1;
        else if (*buffer == HID_CTRL_EXIT_SUSPEND)
            suspended = 0;
        return 0;
    }        
    else;
    return 0;
}

bd_addr_t null_addr = {0xAB, 0x89, 0x67, 0x45, 0x23, 0x01};

uint8_t adv_type = 0x00;

#define USER_MSG_ID_REQUEST_SEND            1

void app_timer_callback()
{
    if (notify_enable)
        btstack_push_user_msg(USER_MSG_ID_REQUEST_SEND, NULL, 0);
}

mouse_report_t report = 
{ 
    .buttons = 0,
    .x = 10,
    .y = 10
};

// let's draw a circle (radius = 100)
const int8_t delta_xy[][2] = 
{{-1,10},{-1,11},{-3,10},{-4,10},{-4,9},{-6,9},{-7,8},{-7,7},{-8,7},{-9,6},
 {-9,4},{-10,4},{-10,3},{-11,1},{-10,1},{-10,-1},{-11,-1},{-10,-3},{-10,-4},{-9,-4},
 {-9,-6},{-8,-7},{-7,-7},{-7,-8},{-6,-9},{-4,-9},{-4,-10},{-3,-10},{-1,-11},{-1,-10},
 {1,-10},{1,-11},{3,-10},{4,-10},{4,-9},{6,-9},{7,-8},{7,-7},{8,-7},{9,-6},{9,-4},
 {10,-4},{10,-3},{11,-1},{10,-1},{10,1},{11,1},{10,3},{10,4},{9,4},{9,6},{8,7},{7,7},
 {7,8},{6,9},{4,9},{4,10},{3,10},{1,11},{1,10}};

void mouse_report_movement(void)
{
    if (suspended)
        return;
    if (protocol_mode != HID_PROTO_REPORT)
        return;
    if (notify_enable)
    {
        static int index = 0;      
        report.x = delta_xy[index][0];
        report.y = delta_xy[index][1];
        index = index < sizeof(delta_xy) / sizeof(delta_xy[0]) - 1 ? index + 1 : 0;
        att_server_notify(handle_send, att_handle_notify, (uint8_t*)&report, sizeof(report));
    }
}

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MSG_ID_REQUEST_SEND:
        if (att_server_can_send_packet_now(handle_send))
            mouse_report_movement();
        else
            att_server_request_can_send_now_event(handle_send);
        break;
    }
}

const static ext_adv_set_en_t adv_sets_en[] = {{.handle = 0, .duration = 0, .max_events = 0}};

void setup_adv(void)
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

const sm_persistent_t sm_persistent =
{
    .er = {1, 2, 3},
    .ir = {4, 5, 6},
    .identity_addr_type     = BD_ADDR_TYPE_LE_RANDOM,
    .identity_addr          = {0xC3, 2, 3, 4, 5, 6}
};

uint8_t *init_service(void);

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
        sm_private_random_address_generation_set_mode(GAP_RANDOM_ADDRESS_RESOLVABLE);
        //gap_set_adv_set_random_addr(0, sm_persistent.identity_addr);
        //setup_adv();
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
            att_set_db(decode_hci_le_meta_event(packet, le_meta_event_create_conn_complete_t)->handle, init_service());
            break;
        default:
            break;
        }

        break;

     case HCI_EVENT_DISCONNECTION_COMPLETE:
        notify_enable = 0;
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        mouse_report_movement();
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
    static uint8_t addr_ready = 0;

    if (packet_type != HCI_EVENT_PACKET) return;
    switch (event)
    {
    case SM_EVENT_PRIVATE_RANDOM_ADDR_UPDATE:
        gap_set_adv_set_random_addr(0, sm_private_random_addr_update_get_address(packet));
        if (0 == addr_ready)
        {
            addr_ready = 1;
            setup_adv();
        }
        break;
    case SM_EVENT_JUST_WORKS_REQUEST:
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
        // TODO: intput number
        platform_printf("===================\ninput number:\n");
        break;
    case SM_EVENT_PASSKEY_INPUT_CANCEL:
        platform_printf("TODO: INPUT_CANCEL\n");
        break;
    default:
        break;
    }
}

const static uint8_t MOUSE_REPORT_MAP[] = {

    USAGE_PAGE(1),      0x01,         // Generic Desktop
    USAGE(1),           0x02,         // Mouse
    COLLECTION(1),      0x01,         // Application
        REPORT_ID(1),       0x01,         //  Report Id 1    
        USAGE(1),           0x01,         //  Pointer
        COLLECTION(1),      0x00,         //  Physical
            USAGE_PAGE(1),      0x09,         //   Buttons
            USAGE_MINIMUM(1),   0x01,
            USAGE_MAXIMUM(1),   0x03,
            LOGICAL_MINIMUM(1), 0x00,
            LOGICAL_MAXIMUM(1), 0x01,
            REPORT_COUNT(1),    0x03,         //   3 bits (Buttons)
            REPORT_SIZE(1),     0x01,
            INPUT(1),           0x02,         //   Data, Variable, Absolute
            REPORT_COUNT(1),    0x01,         //   5 bits (Padding)
            REPORT_SIZE(1),     0x05,
            INPUT(1),           0x01,         //   Constant
            USAGE_PAGE(1),      0x01,         //   Generic Desktop
            USAGE(1),           0x30,         //   X
            USAGE(1),           0x31,         //   Y
            LOGICAL_MINIMUM(1), 0x81,         //   -127
            LOGICAL_MAXIMUM(1), 0x7f,         //   127
            REPORT_SIZE(1),     0x08,         //   Two bytes
            REPORT_COUNT(1),    0x02,
            INPUT(1),           0x06,         //   Data, Variable, Relative
        END_COLLECTION(0),
    END_COLLECTION(0),
};

typedef __packed struct
{
    uint16_t bcd_hid;
    uint8_t  b_country_code;
    uint8_t  flags;
} hid_info_t;

hid_info_t hid_info = 
{
    .bcd_hid = 0x0101,
    .b_country_code = 0,
    .flags = 0x2        // The device is normally connectable
};

static uint8_t att_db_storage[800];

#define GATT_CLIENT_CHARACTERISTICS_DESC_REPORT_REF     0x2908

#define REPORT_TYPE_INPUT               1
#define REPORT_TYPE_OUTPUT              2
#define REPORT_TYPE_FEATURE             3

typedef __packed struct
{
    uint8_t report_id;
    uint8_t report_type;
} report_ref_t;

const static report_ref_t mouse_report = 
{
    .report_id      = 1,
    .report_type    = REPORT_TYPE_INPUT
};

uint8_t *init_service()
{
    const char dev_name[] = "ING Mouse";
    const uint16_t appearance = 0x03C2;
    
    att_db_util_init(att_db_storage, sizeof(att_db_storage));
    
    att_db_util_add_service_uuid16(GAP_SERVICE_UUID);
    att_db_util_add_characteristic_uuid16(GAP_DEVICE_NAME_UUID, ATT_PROPERTY_READ, (uint8_t *)dev_name, sizeof(dev_name) - 1);
    // Characteristic Appearance: 2A01 
    att_db_util_add_characteristic_uuid16(0x2A01, ATT_PROPERTY_READ, (uint8_t *)&appearance, 2);
    
    // Service Human Interface Device: 1812      
    att_db_util_add_service_uuid16(0x1812);
    // Characteristic Protocol Mode: 2A4E    
    att_handle_protocol_mode = att_db_util_add_characteristic_uuid16(0x2A4E,
        ATT_PROPERTY_READ | ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC,
        &protocol_mode, sizeof(protocol_mode));
    // Characteristic Report: 2A4D
    att_handle_report = att_db_util_add_characteristic_uuid16(0x2A4D,
        ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_AUTHENTICATION_REQUIRED,
        (uint8_t *)&report, sizeof(report));
    att_db_util_add_descriptor_uuid16(GATT_CLIENT_CHARACTERISTICS_DESC_REPORT_REF, ATT_PROPERTY_READ, (uint8_t *)&mouse_report, sizeof(mouse_report));
    // Characteristic Report Map: 2A4B
    att_db_util_add_characteristic_uuid16(0x2A4B, ATT_PROPERTY_READ, (uint8_t *)MOUSE_REPORT_MAP, sizeof(MOUSE_REPORT_MAP));    
    // Characteristic Boot Mouse Input Report: 2A33
    att_handle_boot_mouse_input_report = att_db_util_add_characteristic_uuid16(0x2A33, ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY, (uint8_t *)&report, sizeof(report));
    // Characteristic HID Information: 2A4A
    att_db_util_add_characteristic_uuid16(0x2A4A, ATT_PROPERTY_READ, (uint8_t *)&hid_info, sizeof(hid_info));
    // Characteristic HID Control Point: 2A4C 
    att_handle_hid_ctrl_point = att_handle_protocol_mode = att_db_util_add_characteristic_uuid16(0x2A4C, 
        ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC, NULL, 0);

    return att_db_util_get_address();
}

static btstack_packet_callback_registration_t hci_event_callback_registration = {.callback = &user_packet_handler};
static btstack_packet_callback_registration_t sm_event_callback_registration  = {.callback = &sm_packet_handler};

uint32_t setup_profile(void *data, void *user_data)
{   
    att_server_init(att_read_callback, att_write_callback);
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    sm_add_event_handler(&sm_event_callback_registration);
    sm_config(IO_CAPABILITY_NO_INPUT_NO_OUTPUT,
              0,
              &sm_persistent);
    return 0;
}
