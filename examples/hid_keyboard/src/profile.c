#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "sm.h"
#include "btstack_util.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "kv_storage.h"
#include "gatt_client.h"
#include "sig_uuid.h"
#include "btstack_sync.h"
#include "btstack_util.h"
#include "gatt_client_util.h"

#include "att_db_util.h"
#include "ecc_driver.h"

#include "USBHID_Types.h"
#include "USBKeyboard.h"

#include "profile.h"

enum
{
    KV_USER_PEER_OS = KV_USER_KEY_START,
};

#define USER_MSG_ID_OS_FOUND              100

enum os_type
{
    OS_TYPE_UNKNOWN,
    OS_TYPE_iOS,
    OS_TYPE_macOS,
    OS_TYPE_Windows,
    OS_TYPE_Android,
    OS_TYPE_NUM,
} os_type_t;

#define OS_TYPE_DEFAULT     OS_TYPE_Android

const static char os_names[OS_TYPE_NUM][10] = {
    [OS_TYPE_UNKNOWN]   = "Unknown",
    [OS_TYPE_iOS]       = "iOS",
    [OS_TYPE_macOS]     = "macOS",
    [OS_TYPE_Windows]   = "Windows",
    [OS_TYPE_Android]   = "Android",
};

const sm_persistent_t sm_persistent =
{
    .er = {1, 2, 3},
    .ir = {4, 5, 6},
    .identity_addr_type     = BD_ADDR_TYPE_LE_RANDOM,
    .identity_addr          = {0xC3, 0x82, 0x63, 0xc4, 0x35, 0x7b}
};

const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

#define INVALID_HANDLE  (0xffff)
uint16_t att_handle_notify = 0;
hci_con_handle_t handle_send = INVALID_HANDLE;

uint16_t att_handle_protocol_mode;
uint16_t att_handle_hid_ctrl_point;
uint16_t att_handle_report;
uint16_t att_handle_boot_kb_input_report;
uint16_t att_handle_boot_kb_output_report;

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

struct
{
    int flag;
    int cnt;
    char value[6];
    char zero;
} input_number = { 0 };

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

static void set_led_state(const uint8_t state)
{
    const static char names[][7] =
    {
        "NUM", "CAPS", "SCROLL"
    };
    int i;
    for (i = 0; i < sizeof(names) / sizeof(names[0]); i++)
    {
        platform_printf(names[i]);
        platform_printf(state & (1 << i) ? ": ON\n" : ": OFF\n");
    }
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

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
    else if ((att_handle == att_handle_boot_kb_output_report) || (att_handle == att_handle_report))
    {
        set_led_state(*buffer);
        return 0;
    }

    return 0;
}

static int read_charact_value(struct btstack_synced_runner * runner,
    struct gatt_client_discoverer * discoverer,
    hci_con_handle_t con_handle, uint16_t value_handle,
    uint8_t *buffer, int length)
{
    char_node_t *ch = gatt_client_util_find_char_uuid16(discoverer, value_handle);
    if (ch)
    {
        uint16_t len = length;
        int r = gatt_client_sync_read_value_of_characteristic(runner, con_handle, ch->chara.value_handle,
                    buffer, &len);
        return r == 0 ? len : -1;
    }
    else
        return -1;
}

static char *read_str_charact(struct btstack_synced_runner * runner,
    struct gatt_client_discoverer * discoverer,
    hci_con_handle_t con_handle, uint16_t value_handle,
    char *buffer, int buffer_size)
{
    char_node_t *ch = gatt_client_util_find_char_uuid16(discoverer, value_handle);
    if (ch)
    {
        uint16_t length = buffer_size - 1;
        int r = gatt_client_sync_read_value_of_characteristic(runner, con_handle, ch->chara.value_handle,
                    (uint8_t *)buffer, &length);
        if (0 == r)
        {
            buffer[length] = 0;
            return buffer;
        }
    }
    return 0;
}

#define READ_STR_CHARACT(uuid)  read_str_charact(runner, discoverer, con_handle, uuid, (char *)buffer, sizeof(buffer))
#define READ_CHARACT_VAL(uuid)  read_charact_value(runner, discoverer, con_handle, uuid, buffer, sizeof(buffer))

static void detect_peer_os_type(struct btstack_synced_runner * runner, void *user_data)
{
    int err_code = 0;
    uint8_t buffer[200];
    int len;
    char *str;

    printf("detecting peer os type...\n");
    hci_con_handle_t con_handle = (hci_con_handle_t)(uintptr_t)user_data;

    struct gatt_client_discoverer * discoverer = gatt_client_sync_discover_all(runner,
                con_handle, &err_code);

    if (err_code != 0)
        goto quit;

    str = READ_STR_CHARACT(SIG_UUID_CHARACT_MODEL_NUMBER_STRING);
    if (str)
    {
        platform_printf("Model: %s\n", str);
        if ((strstr(str, "iPhone") == str) || (strstr(str, "iPad") == str))
        {
            btstack_push_user_msg(USER_MSG_ID_OS_FOUND, NULL, OS_TYPE_iOS);
            goto quit;
        }
    }

    str = READ_STR_CHARACT(SIG_UUID_CHARACT_MANUFACTURER_NAME_STRING);
    if (str)
    {
        platform_printf("Manufacturer: %s\n", str);
        if (strstr(str, "Apple") == str)
        {
            btstack_push_user_msg(USER_MSG_ID_OS_FOUND, NULL, OS_TYPE_macOS);
            goto quit;
        }
    }

    len = READ_CHARACT_VAL(SIG_UUID_CHARACT_PNP_ID);
    if (len == 7)
    {
        platform_printf("Vendor ID Source: 0x%02X\n", buffer[0]);
        if (buffer[0] == 0x01)
        {
            uint16_t vid = little_endian_read_16(buffer, 1);
            switch (vid)
            {
            case 0x0006: // Microsoft
                btstack_push_user_msg(USER_MSG_ID_OS_FOUND, NULL, OS_TYPE_Windows);
                goto quit;
            case 0x00E0: // Google
                btstack_push_user_msg(USER_MSG_ID_OS_FOUND, NULL, OS_TYPE_Android);
                goto quit;
            }
        }
    }

    btstack_push_user_msg(USER_MSG_ID_OS_FOUND, NULL, OS_TYPE_DEFAULT);

quit:
    printf("complete\n");
    gatt_client_util_free(discoverer);
    return;
}

static void start_detect_peer_os_type(struct btstack_synced_runner * runner, hci_con_handle_t conn_handle)
{
    gatt_client_is_ready(conn_handle);
    btstack_sync_run(runner, detect_peer_os_type, (void *)(uintptr_t)conn_handle);
}

kb_report_t report =
{
    .modifier = 0,
    .reserved = 0,
    .codes = {0}
};

#define USER_MSG_ID_INPUT_HW_KEYS           2
#define USER_MSG_ID_INPUT_ASCII             3

void kb_state_changed(uint16_t key_state)
{
    if(handle_send != INVALID_HANDLE)
        btstack_push_user_msg(USER_MSG_ID_INPUT_HW_KEYS, NULL, key_state);
}

void kb_input_char(char c)
{
    btstack_push_user_msg(USER_MSG_ID_INPUT_ASCII, NULL, c);
}

void kb_send_report(void)
{
    if (protocol_mode != HID_PROTO_REPORT)
        return;

    if (handle_send == INVALID_HANDLE)
        return;
    if (0 == att_handle_notify)
        return;

    att_server_notify(handle_send, att_handle_notify, (uint8_t*)&report, sizeof(report));
}

struct
{
    uint8_t modifier;
    uint8_t code;
} ascii_to_usb[255] =
{
    ['~'] = { .code = KEY_GRAVE, .modifier = KEY_MOD_LSHIFT },
    ['`'] = { .code = KEY_GRAVE },
    ['!'] = { .code = KEY_1, .modifier = KEY_MOD_LSHIFT },
    ['@'] = { .code = KEY_2, .modifier = KEY_MOD_LSHIFT },
    ['#'] = { .code = KEY_3, .modifier = KEY_MOD_LSHIFT },
    ['$'] = { .code = KEY_4, .modifier = KEY_MOD_LSHIFT },
    ['%'] = { .code = KEY_5, .modifier = KEY_MOD_LSHIFT },
    ['^'] = { .code = KEY_6, .modifier = KEY_MOD_LSHIFT },
    ['&'] = { .code = KEY_7, .modifier = KEY_MOD_LSHIFT },
    ['*'] = { .code = KEY_8, .modifier = KEY_MOD_LSHIFT },
    ['('] = { .code = KEY_9, .modifier = KEY_MOD_LSHIFT },
    [')'] = { .code = KEY_0, .modifier = KEY_MOD_LSHIFT },
    ['0'] = { .code = KEY_0, },

    ['_'] = { .code = KEY_MINUS, .modifier = KEY_MOD_LSHIFT },
    ['+'] = { .code = KEY_EQUAL, .modifier = KEY_MOD_LSHIFT },
    ['-'] = { .code = KEY_MINUS, },
    ['='] = { .code = KEY_EQUAL, },

    ['{'] = { .code = KEY_LEFTBRACE, .modifier = KEY_MOD_LSHIFT },
    ['}'] = { .code = KEY_RIGHTBRACE, .modifier = KEY_MOD_LSHIFT },
    [':'] = { .code = KEY_SEMICOLON, .modifier = KEY_MOD_LSHIFT },
    ['"'] = { .code = KEY_APOSTROPHE, .modifier = KEY_MOD_LSHIFT },
    ['|'] = { .code = KEY_BACKSLASH, .modifier = KEY_MOD_LSHIFT },
    ['<'] = { .code = KEY_COMMA, .modifier = KEY_MOD_LSHIFT },
    ['>'] = { .code = KEY_DOT, .modifier = KEY_MOD_LSHIFT },
    ['?'] = { .code = KEY_SLASH, .modifier = KEY_MOD_LSHIFT },
    ['['] = { .code = KEY_LEFTBRACE },
    [']'] = { .code = KEY_RIGHTBRACE },
    [';'] = { .code = KEY_SEMICOLON },
    ['\''] = { .code = KEY_APOSTROPHE },
    ['\\'] = { .code = KEY_BACKSLASH },
    [','] = { .code = KEY_COMMA },
    ['.'] = { .code = KEY_DOT },
    ['/'] = { .code = KEY_SLASH },

    ['\r'] = { .code = KEY_ENTER },
    ['\n'] = { .code = KEY_ENTER },

    ['\t'] = { .code = KEY_TAB },

    [0x1b] = { .code = KEY_ESC },
    [' ']  = { .code = KEY_SPACE },

    [0x8]  = { .code = KEY_BACKSPACE },
    [0x7f] = { .code = KEY_BACKSPACE },
};

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MSG_ID_OS_FOUND:
        platform_printf("Peer OS may be %s.\n", os_names[size]);
        kv_put(KV_USER_PEER_OS, (uint8_t *)&size, 2);
        break;
    case USER_MSG_ID_INPUT_HW_KEYS:
        {
            int i = 0;
            uint16_t current = size;
            report.modifier = 0;
            // report which keys are pressed
            if (current & 1)
                report.codes[i++] = KEY_1;
            if (current & 2)
                report.codes[i++] = KEY_2;
            if (current & 4)
                report.codes[i++] = KEY_3;
            while (i < sizeof(report.codes))
                report.codes[i++] = 0;
            platform_printf("%02X-%02X-%02X\n", report.codes[0], report.codes[1], report.codes[2]);
        }
        kb_send_report();
        break;
    case USER_MSG_ID_INPUT_ASCII:
        if (input_number.flag)
        {
            char c = (char)size;
            if (('0' <= c) && (c <= '9'))
            {
                input_number.value[input_number.cnt++] = c;
                if (input_number.cnt >= sizeof(input_number.value))
                {
                    input_number.flag = 0;

                    int a = atoi(input_number.value);
                    if (handle_send != INVALID_HANDLE)
                        sm_passkey_input(handle_send, a);
                }
            }
        }
        else
        {
            char c = (char)size;
            report.modifier = 0;
            memset(report.codes, 0, sizeof(report.codes));
            if (('a' <= c) && (c <= 'z'))
            {
                report.codes[0] = c - 'a' + KEY_A;
            }
            else if (('1' <= c) && (c <= '9'))
            {
                report.codes[0] = c - '1' + KEY_1;
            }
            else if (('A' <= c) && (c <= 'Z'))
            {
                report.modifier = KEY_MOD_LSHIFT;
                report.codes[0] = c - 'A' + KEY_A;
            }
            else
            {
                report.modifier = ascii_to_usb[c].modifier;
                report.codes[0] = ascii_to_usb[c].code;
                if (report.codes[0] == 0)
                    break;
            }
            kb_send_report();
            report.modifier = 0;
            report.codes[0] = 0;
            kb_send_report();
        }
        break;
    }
}

uint8_t *init_service(void);

static struct btstack_synced_runner *synced_runner = NULL;

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const static ext_adv_set_en_t adv_sets_en[] = {{.handle = 0, .duration = 0, .max_events = 0}};
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;

        synced_runner = btstack_create_sync_runner(0);

        if (BD_ADDR_TYPE_LE_RANDOM ==  sm_persistent.identity_addr_type)
            gap_set_adv_set_random_addr(0, sm_persistent.identity_addr);
        gap_set_ext_adv_para(0,
                                CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT,
                                0x00a1, 0x00a1,            // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
                                PRIMARY_ADV_ALL_CHANNELS,  // Primary_Advertising_Channel_Map
                                sm_persistent.identity_addr_type,    // Own_Address_Type
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
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            att_set_db(decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t)->handle, att_db_util_get_address());
            handle_send = decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t)->handle;
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        handle_send = INVALID_HANDLE;
        att_handle_notify = 0;
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        kb_send_report();
        break;

    case BTSTACK_EVENT_USER_MSG:
        p_user_msg = hci_event_packet_get_user_msg(packet);
        user_msg_handler(p_user_msg->msg_id, p_user_msg->data, p_user_msg->len);
        break;
    default:
        break;
    }
}

const static uint8_t KB_REPORT_MAP[] = {

    USAGE_PAGE(1),      0x01,         // Generic Desktop
    USAGE(1),           0x06,         // Keyboard
    COLLECTION(1),      0x01,         // Application
        REPORT_ID(1),       0x01,         //  Report Id 1
        USAGE_PAGE(1),      0x07,         //  Key Codes
        USAGE_MINIMUM(1),   0xe0,
        USAGE_MAXIMUM(1),   0xe7,
        LOGICAL_MINIMUM(1), 0x00,
        LOGICAL_MAXIMUM(1), 0x01,
        REPORT_SIZE(1),     0x01,
        REPORT_COUNT(1),    0x08,         //   8 bits for MODIFIER
        INPUT(1),           0x02,         //   Data, Variable, Absolute

        REPORT_COUNT(1),    0x01,         //   Reserved byte
        REPORT_SIZE(1),     0x08,
        INPUT(1),           0x01,         //   Data, Variable, Constant

        REPORT_COUNT(1),    0x05,         //   5 bits (Padding)
        REPORT_SIZE(1),     0x01,
        USAGE_PAGE(1),      0x08,         //   LEDs  (same as boot)
        USAGE_MINIMUM(1),   0x01,
        USAGE_MAXIMUM(1),   0x05,
        OUTPUT(1),          0x02,         //   Data, Variable, Absolute
        REPORT_COUNT(1),    0x01,         //   5 bits (Padding)
        REPORT_SIZE(1),     0x03,
        OUTPUT(1),          0x01,         //   Padding

        REPORT_COUNT(1),    INPUT_REPORT_KEYS_MAX,  //   Max. number of key codes
        REPORT_SIZE(1),     0x08,
        LOGICAL_MINIMUM(1), 0x00,
        LOGICAL_MAXIMUM(1), 0x65,
        USAGE_PAGE(1),      0x07,         //  Key Codes
        USAGE_MINIMUM(1),   0x00,
        USAGE_MAXIMUM(1),   0x65,
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

#define GATT_CLIENT_CHARACTERISTICS_DESC_REPORT_REF     0x2908

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

const static report_ref_t kb_desc_feature_report =
{
    .report_id      = 2,
    .report_type    = REPORT_TYPE_FEATURE
};
static uint8_t att_db_storage[800];

uint8_t *init_service()
{
    const char dev_name[] = "ING Keyboard";
    const uint16_t appearance = 0x03C1;

    att_db_util_init(att_db_storage, sizeof(att_db_storage));

    att_db_util_add_service_uuid16(GAP_SERVICE_UUID);
    att_db_util_add_characteristic_uuid16(GAP_DEVICE_NAME_UUID, ATT_PROPERTY_READ, (uint8_t *)dev_name, sizeof(dev_name) - 1);
    // Characteristic Appearance: 2A01
    att_db_util_add_characteristic_uuid16(0x2A01, ATT_PROPERTY_READ, (uint8_t *)&appearance, 2);

    // Service Human Interface Device: 1812
    att_db_util_add_service_uuid16(0x1812);

    // Characteristic HID Information: 2A4A
    att_db_util_add_characteristic_uuid16(0x2A4A, ATT_PROPERTY_READ, (uint8_t *)&hid_info, sizeof(hid_info));

    // Characteristic Protocol Mode: 2A4E
    att_handle_protocol_mode = att_db_util_add_characteristic_uuid16(0x2A4E,
        ATT_PROPERTY_READ | ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC,
        &protocol_mode, sizeof(protocol_mode));

    // Characteristic HID Control Point: 2A4C
    att_handle_hid_ctrl_point = att_handle_protocol_mode = att_db_util_add_characteristic_uuid16(0x2A4C, ATT_PROPERTY_WRITE_WITHOUT_RESPONSE,
        NULL, 0);

    // Characteristic Report Map: 2A4B
    att_db_util_add_characteristic_uuid16(0x2A4B, ATT_PROPERTY_READ, (uint8_t *)KB_REPORT_MAP, sizeof(KB_REPORT_MAP));

    // Characteristic Report: 2A4D
    att_handle_report = att_db_util_add_characteristic_uuid16(0x2A4D, ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_AUTHENTICATION_REQUIRED,
        (uint8_t *)&report, sizeof(report));
    att_db_util_add_descriptor_uuid16(GATT_CLIENT_CHARACTERISTICS_DESC_REPORT_REF, ATT_PROPERTY_READ ,
        (uint8_t *)&kb_desc_input_report, sizeof(kb_desc_input_report));
    att_db_util_add_characteristic_uuid16(0x2A4D, 
		ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_WRITE | ATT_PROPERTY_READ | ATT_PROPERTY_DYNAMIC | ATT_PROPERTY_AUTHENTICATION_REQUIRED,
        (uint8_t *)&report, sizeof(report));
    att_db_util_add_descriptor_uuid16(GATT_CLIENT_CHARACTERISTICS_DESC_REPORT_REF, ATT_PROPERTY_READ ,
        (uint8_t *)&kb_desc_output_report, sizeof(kb_desc_output_report));
    att_db_util_add_characteristic_uuid16(0x2A4D, ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_AUTHENTICATION_REQUIRED,
        (uint8_t *)&report, sizeof(report));
    att_db_util_add_descriptor_uuid16(GATT_CLIENT_CHARACTERISTICS_DESC_REPORT_REF, ATT_PROPERTY_READ,
        (uint8_t *)&kb_desc_feature_report, sizeof(kb_desc_feature_report));

    // Characteristic Boot Keyboard Input Report: 2A22
    att_handle_boot_kb_input_report = att_db_util_add_characteristic_uuid16(0x2A22, ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY,
        (uint8_t *)&report, sizeof(report));
    // Characteristic Boot Keyboard Output Report: 2A32
    att_handle_boot_kb_output_report = att_db_util_add_characteristic_uuid16(0x2A32, ATT_PROPERTY_READ | ATT_PROPERTY_WRITE
        | ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, (uint8_t *)&report, sizeof(report));

    return att_db_util_get_address();
}

static void sm_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);

    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case SM_EVENT_JUST_WORKS_REQUEST:
        sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
        break;
    case SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED:
        att_handle_notify = att_handle_report;
        break;
    case SM_EVENT_PASSKEY_INPUT_NUMBER:
        input_number.flag = 1;
        input_number.cnt = 0;
        break;
    case SM_EVENT_NUMERIC_COMPARISON_REQUEST:
        platform_printf("NUMERIC_COMPARISON: %06d\n",
            sm_event_numeric_comparison_get_compare_value(packet));
        sm_numeric_comparison_confirm(sm_event_numeric_comparison_get_handle(packet));
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
                    start_detect_peer_os_type(synced_runner, state_changed->conn_handle);
                    break;
                case SM_FINAL_REESTABLISHED:
                    platform_printf("SM: REESTABLISHED");
                    if (kv_get(KV_USER_PEER_OS, NULL) == NULL)
                        start_detect_peer_os_type(synced_runner,state_changed->conn_handle);
                    if (0 == att_handle_notify)
                    {
                        platform_printf(" BUT LOCAL INFO DELETED");
                    }
                    platform_printf("\n");
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

uint32_t setup_profile(void *data, void *user_data)
{
    platform_printf("setup profile\n");
    init_service();
    install_ecc_driver();
    sm_add_event_handler(&sm_event_callback_registration);
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    sm_config(1, IO_CAPABILITY_KEYBOARD_DISPLAY,
              0,
              &sm_persistent);
    sm_set_authentication_requirements(SM_AUTHREQ_BONDING | SM_AUTHREQ_SC);
    return 0;
}
