#include <stddef.h>
#include "platform_api.h"
#include "att_db.h"
#include "sig_uuid.h"

#include "att_db_util.h"

#include "../../hid_keyboard/src/USBHID_Types.h"
#include "../../hid_keyboard/src/USBKeyboard.h"

#include "kb_service.h"

uint16_t att_handle_kb_notify = 0;

uint16_t att_handle_protocol_mode;
uint16_t att_handle_hid_ctrl_point;
uint16_t att_handle_report;
uint16_t att_handle_boot_kb_input_report;
uint16_t att_handle_boot_kb_output_report;

uint8_t suspended = 0;
uint8_t kb_notify_enable = 0;
static hci_con_handle_t handle_send = 0;

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

my_kb_report_t report = 
{ 
    .flags = 1
};

void kb_send_report(void)
{
    if (suspended)
        return;
    if (protocol_mode != HID_PROTO_REPORT)
        return;
    if (kb_notify_enable)
    {
        att_server_notify(handle_send, att_handle_kb_notify, (uint8_t*)&report, sizeof(report));
    }
}

int kb_att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, 
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    if (att_handle == att_handle_protocol_mode)
    {
        protocol_mode = *buffer;
        return 0;
    }
    else if (att_handle == att_handle_report + 1)
    {
        if(*(uint16_t *)buffer == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION)
        {
            handle_send = connection_handle;
            kb_notify_enable = 1;
            att_handle_kb_notify = att_handle_report;
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
    else
        return -1;
}

uint16_t kb_att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, 
                                  uint8_t * buffer, uint16_t buffer_size)
{
    if (att_handle == att_handle_protocol_mode)
    {
        if (buffer)
            *buffer = protocol_mode;
        return sizeof(protocol_mode);
    }
    else
        return 0xffff;
}

const static uint8_t KB_REPORT_MAP[] = {

    USAGE_PAGE(1),      0x01,         // Generic Desktop
    USAGE(1),           0x06,         // Keyboard
    COLLECTION(1),      0x01,         // Application
        REPORT_ID(1),       0x01,         //  Report Id 1    

        REPORT_COUNT(1),    0x01,         //   Max. 1 key codes
        REPORT_SIZE(1),     0x08,
        LOGICAL_MINIMUM(1), 0x00,
        LOGICAL_MAXIMUM(1), 0xff,
        USAGE_PAGE(1),      0x07,         //  Key Codes
        USAGE_MINIMUM(1),   0x00,
        USAGE_MAXIMUM(1),   0xff,
        INPUT(1),           0x00,         //  Input (Data, Array) Key array(3 bytes)
    END_COLLECTION(0),
};

#pragma pack (push, 1)

typedef struct
{
    uint8_t report_id;
    uint8_t report_type;
} report_ref_t;

typedef struct
{
    uint16_t bcd_hid;
    uint8_t  b_country_code;
    uint8_t  flags;
} hid_info_t;

#pragma pack (pop)

hid_info_t hid_info = 
{
    .bcd_hid = 0x0101,
    .b_country_code = 0,
    .flags = 0x2        // The device is normally connectable
};

#define REPORT_TYPE_INPUT               1
#define REPORT_TYPE_OUTPUT              2
#define REPORT_TYPE_FEATURE             3

const static report_ref_t kb_desc_input_report = 
{
    .report_id      = 1,
    .report_type    = REPORT_TYPE_INPUT
};

const static report_ref_t kb_desc_output_report = 
{
    .report_id      = 1,
    .report_type    = REPORT_TYPE_OUTPUT
};

void init_kb_service(void)
{
    // Service Human Interface Device: 1812      
    att_db_util_add_service_uuid16(SIG_UUID_SERVICE_HUMAN_INTERFACE_DEVICE);

    // Characteristic Protocol Mode: 2A4E    
    att_handle_protocol_mode = att_db_util_add_characteristic_uuid16(SIG_UUID_CHARACT_PROTOCOL_MODE,
        ATT_PROPERTY_READ | ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC,
        &protocol_mode, sizeof(protocol_mode));
    // Characteristic Report: 2A4D
    att_handle_report = att_db_util_add_characteristic_uuid16(SIG_UUID_CHARACT_REPORT,
        ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_AUTHENTICATION_REQUIRED,
        (uint8_t *)&report, sizeof(report));
    att_db_util_add_descriptor_uuid16(SIG_UUID_DESCRIP_REPORT_REFERENCE, ATT_PROPERTY_READ, 
        (uint8_t *)&kb_desc_input_report, sizeof(kb_desc_input_report));
    att_db_util_add_descriptor_uuid16(SIG_UUID_DESCRIP_REPORT_REFERENCE, ATT_PROPERTY_READ, 
        (uint8_t *)&kb_desc_output_report, sizeof(kb_desc_output_report));

    // Characteristic Report Map: 2A4B
    att_db_util_add_characteristic_uuid16(SIG_UUID_CHARACT_REPORT_MAP, ATT_PROPERTY_READ, (uint8_t *)KB_REPORT_MAP, sizeof(KB_REPORT_MAP));    

    // Characteristic Boot Keyboard Input Report: 2A22
    att_handle_boot_kb_input_report = att_db_util_add_characteristic_uuid16(SIG_UUID_CHARACT_BOOT_KEYBOARD_INPUT_REPORT,
        ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY, (uint8_t *)&report, sizeof(report));
    // Characteristic Boot Keyboard Output Report: 2A32
    att_handle_boot_kb_output_report = att_db_util_add_characteristic_uuid16(SIG_UUID_CHARACT_BOOT_KEYBOARD_OUTPUT_REPORT, 
        ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_WRITE_WITHOUT_RESPONSE,
        (uint8_t *)&report, sizeof(report));

    // Characteristic HID Information: 2A4A
    att_db_util_add_characteristic_uuid16(SIG_UUID_CHARACT_HID_INFORMATION,
        ATT_PROPERTY_READ, (uint8_t *)&hid_info, sizeof(hid_info));
    // Characteristic HID Control Point: 2A4C 
    att_handle_hid_ctrl_point = att_handle_protocol_mode =
        att_db_util_add_characteristic_uuid16(SIG_UUID_CHARACT_HID_CONTROL_POINT, 
            ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, NULL, 0);
}
