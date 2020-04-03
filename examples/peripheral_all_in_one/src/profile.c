#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"

#include "step_calc.h"

#include "screen.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "bme280.h"

#include <string.h>
#include <stdio.h>

#ifdef SIMULATION
#include <stdlib.h>
#endif

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
        #include "../../peripheral_etag/data/image1_black.dat" 
    },
    .red_white = {
        #include "../../peripheral_etag/data/image1_red.dat" 
    }
};

const eink_image_t def_image2 = 
{
    .black_white = {
        #include "../../peripheral_etag/data/image2_black.dat" 
    },
    .red_white = {
        #include "../../peripheral_etag/data/image2_red.dat" 
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
#define HANDLE_BATTERY_LEVEL                                 6
#define HANDLE_RSC_MEASUREMENT                               9
#define HANDLE_RSC_FEATURE                                   12
#define HANDLE_TEMPERATURE_MEASUREMENT                       15
#define HANDLE_TEMPERATURE_TYPE                              18
#define HANDLE_RGB_LIGHTING_CONTROL                          21
#define HANDLE_ETAG_CONTROL                                  24
#define HANDLE_ETAG_DATA                                     26
#define HANDLE_KEY_INPUT                                     31

#define HANDLE_BATTERY_LEVEL_OFFSET                          76


const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

#pragma pack (push, 1)
typedef struct
{
    uint8_t     flags;
    uint16_t    speed;            // Unit is in m/s with a resolution of 1/256 s
    uint8_t     cadence;          // Unit is in 1/minute (or RPM) with a resolutions of 1 1/min (or 1 RPM) 
    uint16_t    stride_length;    // Unit is in meter with a resolution of 1/100 m (or centimeter).
    uint32_t    total_distance;   // Unit is in meter with a resolution of 1/10 m (or decimeter).
} rsc_meas_t;
#pragma pack (pop)

rsc_meas_t rsc_meas = {0};
uint8_t rsc_notify_enable = 0;
uint8_t rsc_indicate_enable = 0;

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
    printf("refresh\n");
    xQueueOverwrite(xQueue, &msg);
}

uint32_t calc_check_sum(const uint8_t *data, const uint16_t size)
{
    uint32_t r = 0;
    uint16_t i;
    for (i = 0; i < size; i++) r += data[i];
    return r;
}

uint8_t temperature_value[]={0x00,0x00,0x00,0x00,0xFE};

static int temperture_notify_enable = 0;
static int temperture_indicate_enable = 0;

static void read_temperature(void)
{
#ifndef SIMULATION
    int32_t  bme280_temperature;
    bme280_temperature = bme280_compensate_temperature_read() + 1800;
    temperature_value[3]=(uint8_t)(bme280_temperature>>16);
    temperature_value[2]=(uint8_t)(bme280_temperature>>8);
    temperature_value[1]=(uint8_t)bme280_temperature;
#else
    temperature_value[2] = 10;
    temperature_value[1] = (rand() & 0x1f);
#endif
}

void set_led_color(uint8_t r, uint8_t g, uint8_t b);
void set_freq(uint16_t freq);

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
            return 1; // TODO: return required buffer size
    case HANDLE_ETAG_DATA:
        if (buffer)
        {
            *(uint16_t *)buffer = img_write_offset;
            return buffer_size;
        }
        else
            return sizeof(img_write_offset);
    case HANDLE_RGB_LIGHTING_CONTROL:
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

hci_con_handle_t handle_send;
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
            {
                printf("etag: img_write_offset = %d, buffer_size = %d\n", img_write_offset, buffer_size);
                etag_status = ETAG_STATUS_ERR;
            }
            
            // verify check sum
            check_sum = *(uint32_t *)(buffer + 1);
            if (check_sum != calc_check_sum((uint8_t *)&image, sizeof(image)))
            {
                printf("etag check_sum = %d, buffer_size = %d\n", check_sum, calc_check_sum((uint8_t *)&image, sizeof(image)));
                etag_status = ETAG_STATUS_ERR;
            }

            // refresh now
            if (ETAG_STATUS_OK == etag_status)
                refresh_display(&image);

            break;
        default:
            printf("etag unkown command = %d\n", *buffer);
            etag_status = ETAG_STATUS_ERR;
            break;
        }
        return 0;
    case HANDLE_ETAG_DATA:
        if (img_write_offset + buffer_size > sizeof(image))
        {
            printf("etag img_write_offset + buffer_size = %d\n", img_write_offset + buffer_size);
            etag_status = ETAG_STATUS_ERR;
        }
        if (ETAG_STATUS_OK != etag_status)
            return 0;
        memcpy((uint8_t *)&image + img_write_offset, buffer, buffer_size);
        img_write_offset += buffer_size;
        return 0;
    case HANDLE_RGB_LIGHTING_CONTROL:
        set_led_color(buffer[0], buffer[1], buffer[2]);
        return 0;

    case HANDLE_TEMPERATURE_MEASUREMENT + 1:
        temperture_notify_enable = 0;
        temperture_indicate_enable = 0;
        switch (*(uint16_t *)buffer)
        {
        case GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION:
            temperture_indicate_enable = 1;
            att_server_request_can_send_now_event(handle_send);
            break;
        case GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION:
            temperture_notify_enable = 1;
            att_server_request_can_send_now_event(handle_send);
            break;
        }
        return 0;
    case HANDLE_RSC_MEASUREMENT + 1:
        handle_send = connection_handle;
        rsc_indicate_enable = 0;
        rsc_notify_enable = 0;
        switch (*(uint16_t *)buffer)
        {
        case GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION:
            rsc_indicate_enable = 1;
            att_server_request_can_send_now_event(handle_send);
            break;
        case GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION:
            rsc_notify_enable = 1;
            att_server_request_can_send_now_event(handle_send);
            break;
        }
        return 0;
    case HANDLE_KEY_INPUT:
        set_freq(*(uint16_t *)buffer);
        return 0;
    default:
        return 0;
    }
}

#define USER_MSG_ID_REQUEST_SEND            1

void send_rsc_meas(void)
{
    if (rsc_notify_enable)
        att_server_notify(handle_send, HANDLE_RSC_MEASUREMENT, (uint8_t*)&rsc_meas, sizeof(rsc_meas));

    if (rsc_indicate_enable)
        att_server_indicate(handle_send, HANDLE_RSC_MEASUREMENT, (uint8_t*)&rsc_meas, sizeof(rsc_meas));
}

static void send_temperature(void)
{
    if (temperture_notify_enable)
    {
        read_temperature();
        att_server_notify(handle_send, HANDLE_TEMPERATURE_MEASUREMENT, (uint8_t*)temperature_value, sizeof(temperature_value));
    }

    if (temperture_indicate_enable)
    {
        read_temperature();
        att_server_indicate(handle_send, HANDLE_TEMPERATURE_MEASUREMENT, (uint8_t*)temperature_value, sizeof(temperature_value));
    }
}

#define USER_MSG_ID_REQUEST_SEND            1

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MSG_ID_REQUEST_SEND:
        att_server_request_can_send_now_event(handle_send);
        break;
    }
}


bd_addr_t null_addr = {0xAB, 0x89, 0x67, 0x45, 0x23, 0x01};

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
        gap_set_adv_set_random_addr(0, (const uint8_t *)(0x44000));
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
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
            printf("conn: %d\n", decode_hci_le_meta_event(packet, le_meta_event_create_conn_complete_t)->status);
            att_set_db(decode_hci_le_meta_event(packet, le_meta_event_create_conn_complete_t)->handle,
                       profile_data);
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        printf("disc\n");
        temperture_notify_enable = 0;
        temperture_indicate_enable = 0;
        rsc_notify_enable = 0;
        rsc_indicate_enable = 0;
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        send_temperature();
        send_rsc_meas();
        break;

    case BTSTACK_EVENT_USER_MSG:
        p_user_msg = hci_event_packet_get_user_msg(packet);
        user_msg_handler(p_user_msg->msg_id, p_user_msg->data, p_user_msg->len);
        break;

    default:
        break;
    }
}

void print_reg(uint32_t addr)
{
    printf("%08X = %08X\n", addr, *(uint32_t *)(addr));
}

SemaphoreHandle_t sem_battery = NULL;
uint8_t *battery_level = NULL;

static void battery_task(void *pdata)
{
    uint16_t voltage;
    for (;;)
    {
        BaseType_t r = xSemaphoreTake(sem_battery,  portMAX_DELAY);
        if (r != pdTRUE)
            continue;

#ifndef SIMULATION
        ADC_Reset();
        while (ADC_IsChannelDataValid(0) == 0) ;
        voltage = ADC_ReadChannelData(0);
        // printf("U = %d\n", voltage);
#else
        voltage = 800 + rand() % 200;
#endif
        // level is reported by comparing max & min voltage
        // for DEMO only
#define MAX_VOLT       1023
#define MIN_VOLT       800
        if (voltage > MIN_VOLT)
            *battery_level = (uint32_t)(voltage - MIN_VOLT) * 100 / (MAX_VOLT - MIN_VOLT);
        else
            *battery_level = 0;
    }
}

void app_timer_callback()
{
    //printf("t\n");    
    if (temperture_notify_enable | temperture_indicate_enable)
    {
        read_temperature();
        btstack_push_user_msg(USER_MSG_ID_REQUEST_SEND, NULL, 0);
    }
    
    if (rsc_notify_enable | rsc_indicate_enable)
    {
        const pedometer_info_t *pedometer = pedometer_get_info();
        rsc_meas.flags = pedometer->cadence > 130 ? 7 : 3;
        rsc_meas.speed = pedometer->speed;
        rsc_meas.cadence = pedometer->cadence;
        rsc_meas.stride_length = pedometer->stride_length;
        rsc_meas.total_distance = pedometer->total_distance;
        btstack_push_user_msg(USER_MSG_ID_REQUEST_SEND, NULL, 0);
    }
}

uint32_t setup_profile(void *data, void *user_data)
{   
    battery_level = profile_data + HANDLE_BATTERY_LEVEL_OFFSET;
    sem_battery = xSemaphoreCreateBinary();

#ifndef SIMULATION 
    printf("powter_ctrl\n");
    ADC_PowerCtrl(1);
    printf("powter_ctrl\n");
    ADC_SetClkSel(ADC_CLK_EN | ADC_CLK_128);
    ADC_SetMode(ADC_MODE_SINGLE);
    ADC_EnableChannel(0, 1);    
    ADC_Enable(1);
#endif

    xTaskCreate(battery_task,
               "b",
               50,
               NULL,
               (configMAX_PRIORITIES - 1),
               NULL);
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

