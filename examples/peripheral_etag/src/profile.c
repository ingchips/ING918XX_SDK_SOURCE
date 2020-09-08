#include <string.h>

#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"

#include "screen.h"

#include "FreeRTOS.h"
#include "queue.h"

#define ETAG_STATUS_OK              0
#define ETAG_STATUS_ERR             1

#define ETAG_CMD_IMG_CLEAR          0
#define ETAG_CMD_IMG_USE_DEF1       1
#define ETAG_CMD_IMG_USE_DEF2       2
#define ETAG_CMD_IMG_START_WRITE    10
#define ETAG_CMD_IMG_COMPLETE       11

typedef struct
{
    uint8_t black_white[IMAGE_BYTE_SIZE];   // 0 -> black, 1 -> white
    uint8_t red_white[IMAGE_BYTE_SIZE];     // 0 -> red,   1 -> white
} eink_image_t;

#if (HEIGHT == 128)
const eink_image_t def_image1 = 
{
    .black_white = {
        #include "../data/image1_black.dat" 
    },
    .red_white = {
        #include "../data/image1_red.dat" 
    }
};

const eink_image_t def_image2 = 
{
    .black_white = {
        #include "../data/image2_black.dat" 
    },
    .red_white = {
        #include "../data/image2_red.dat" 
    }
};
#endif

#if (HEIGHT == 152)
const eink_image_t def_image1 = 
{
    .black_white = {
        #include "../data/img152_1_b.dat" 
    },
    .red_white = {
        #include "../data/img152_1_r.dat" 
    }
};

const eink_image_t def_image2 = 
{
    .black_white = {
        #include "../data/img152_2_b.dat" 
    },
    .red_white = {
        #include "../data/img152_2_r.dat" 
    }
};
#endif

static eink_image_t image = {0};

static int8_t  etag_status = ETAG_STATUS_OK;
static int16_t img_write_offset = -1;

// GATT characteristic handles
#define HANDLE_DEVICE_NAME                                   3
#define HANDLE_ETAG_CONTROL                                  6
#define HANDLE_ETAG_DATA                                     8


const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

const static uint8_t profile_data[] = {
#if (HEIGHT == 152)
    #include "../data/gatt2.profile"
#elif (HEIGHT == 128)
    #include "../data/gatt.profile"
#endif
};

void refresh_display(const eink_image_t *image)
{
    queue_msg_t msg = {NULL, NULL};
    extern QueueHandle_t xQueue;
    if (image) 
    {
        msg.black_white = image->black_white;
        msg.red_white = image->red_white;
    }
    else;
    platform_printf("refresh\n");
    xQueueOverwrite(xQueue, &msg);
}

uint32_t calc_check_sum(const uint8_t *data, const uint16_t size)
{
    uint32_t r = 0;
    uint16_t i;
    for (i = 0; i < size; i++) r += data[i];
    return r;
}
 
static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, 
                                  uint8_t * buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {
    case HANDLE_ETAG_CONTROL:
        if (buffer)
        {
            *buffer = etag_status;
            return buffer_size;
        }
        else
            return sizeof(etag_status);
    case HANDLE_ETAG_DATA:
        if (buffer)
        {
            *(uint16_t *)buffer = img_write_offset;
            return buffer_size;
        }
        else
            return sizeof(img_write_offset);
    default:
        return 0;
    }
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, 
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    uint32_t check_sum;
    switch (att_handle)
    {
    case HANDLE_ETAG_CONTROL:
        switch (*buffer)
        {
        case ETAG_CMD_IMG_CLEAR:
            refresh_display(NULL);
            break;
        case ETAG_CMD_IMG_USE_DEF1:
            refresh_display(&def_image1);
            break;
        case ETAG_CMD_IMG_USE_DEF2:
            refresh_display(&def_image2);
            break;
        case ETAG_CMD_IMG_START_WRITE:
            etag_status = ETAG_STATUS_OK;
            img_write_offset = 0;
            break;
        case ETAG_CMD_IMG_COMPLETE:
            if ((img_write_offset != sizeof(image)) || (buffer_size != sizeof(uint32_t) + 1))
                etag_status = ETAG_STATUS_ERR;

            // verify check sum
            check_sum = *(uint32_t *)(buffer + 1);
            if (check_sum != calc_check_sum((uint8_t *)&image, sizeof(image)))
                etag_status = ETAG_STATUS_ERR;

            // refresh now
            if (ETAG_STATUS_OK == etag_status)
                refresh_display(&image);

            break;
        default:
            etag_status = ETAG_STATUS_ERR;
            break;
        }
        return 0;
    case HANDLE_ETAG_DATA:       
        if (img_write_offset + buffer_size > sizeof(image))
            etag_status = ETAG_STATUS_ERR;
        if (ETAG_STATUS_OK != etag_status)
            return 0;
        memcpy((uint8_t *)&image + img_write_offset, buffer, buffer_size);
        img_write_offset += buffer_size;
        return 0;

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

bd_addr_t null_addr = {0xAB, 0x89, 0x67, 0x45, 0x23, 0x01};

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const static ext_adv_set_en_t adv_sets_en[1] = {{.handle = 0, .duration = 0, .max_events = 0}};
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;

        gap_set_ext_adv_para(0, 
                                CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT,
                                0x00a1, 0x00a1,            // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
                                PRIMARY_ADV_ALL_CHANNELS,  // Primary_Advertising_Channel_Map
                                BD_ADDR_TYPE_LE_PUBLIC,    // Own_Address_Type
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
            att_set_db(decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t)->handle,
                       profile_data);
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
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

uint32_t setup_profile(void *data, void *user_data)
{   
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

