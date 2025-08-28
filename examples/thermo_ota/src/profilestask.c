#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"

#include "ota_service.h"

#include "att_db_util.h"
#include "iic.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "board.h"

extern void ota_connected(void);

uint16_t att_temp_value_handle = 0;
uint16_t att_client_desc_value_handle = 0;

uint8_t temperature_value[]={0x00,0x00,0x00,0x00,0xFE};

static int temperture_notify_enable=0;
static int temperture_indicate_enable=0;

#ifndef SIMULATION
#define I2C_PORT        I2C_PORT_0
#endif

static void read_temperature(void)
{
    float temp;
#ifndef SIMULATION
    if ((temp = get_temperature()) < 0)
        return;
#ifdef PRINT_ALL
    platform_printf("T: %04d * 0.01 Deg\n", temp);
    platform_printf("H: %04d / 1024 %%\n", get_humidity());
    platform_printf("P: %08d Pascal \n", get_pressure());
#endif
    int32_t sensor_temperature = temp;
    temperature_value[3]=(uint8_t)(sensor_temperature>>16);
    temperature_value[2]=(uint8_t)(sensor_temperature>>8);
    temperature_value[1]=(uint8_t)sensor_temperature;
#else
    temperature_value[2] = 10;
    temperature_value[1] = (rand() & 0x1f);
#endif
}

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    if (att_handle == att_temp_value_handle)
    {
        if (buffer)
        {
            read_temperature();
            memcpy(buffer, &temperature_value[offset], buffer_size);
            //invoke the tmpreature measurement API
            return buffer_size;
        }
        else
            return sizeof(temperature_value);
    }
    else
        return ota_read_callback(att_handle, offset, buffer, buffer_size);
}

hci_con_handle_t handle_send;
static btstack_packet_callback_registration_t hci_event_callback_registration;

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    handle_send = connection_handle;
    if (att_handle == att_client_desc_value_handle)
    {
        if(*(uint16_t *)buffer == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION)
        {
            temperture_indicate_enable = 1;
        }
        else if(*(uint16_t *)buffer == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION)
        {
            temperture_notify_enable = 1;
        }
        return 0;
    }
    else
        return ota_write_callback(att_handle, transaction_mode, offset, buffer, buffer_size);
}

const uint8_t adv_data[]={

    // Flags general discoverable
    0x02,0x01,0x06,

    // Name
    0x10, 0x09, 'I', 'N', 'G', ' ', 'T', 'h', 'e', 'r', 'm', 'o', 'm', 'e', 't', 'e', 'r',

    //slave connection Interval range
    0x05, 0x12, 0x12,0x00,0x20,0x00,
};

const uint8_t adv_data_len = sizeof(adv_data);

const uint8_t scan_data[] = {
    // Flags general discoverable
    0x02, 0x01, 0x06,

    //Tx Power
    0x02, 0x0a,0x08,

    //32bits complete service UUIDs
    0x03, 0x03,0x09,0x18,
};

const uint8_t scan_data_len = sizeof(scan_data);

bd_addr_t null_addr={0xAB,0x89,0x67,0x45,0x23,0x01};

uint16_t adv_int_min = 0x00a1;
uint16_t adv_int_max = 0x00a1;

uint8_t adv_type = 0x00;

static void send_temperature(void)
{
    if (temperture_notify_enable)
        att_server_notify(handle_send, att_temp_value_handle, (uint8_t*)temperature_value, sizeof(temperature_value));

    if (temperture_indicate_enable)
        att_server_indicate(handle_send, att_temp_value_handle, (uint8_t*)temperature_value, sizeof(temperature_value));
}

static TimerHandle_t app_timer = 0;

#define USER_MSG_ID_REQUEST_SEND            1

static void app_timer_callback(TimerHandle_t xTimer)
{
    btstack_push_user_msg(USER_MSG_ID_REQUEST_SEND, NULL, 0);
}

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MSG_ID_REQUEST_SEND:
        if (temperture_notify_enable | temperture_indicate_enable)
        {
            read_temperature();
            send_temperature();
        }
        break;
    }
}

uint8_t *init_service(void);

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const static ext_adv_set_en_t adv_sets_en[] = {{.handle = 0, .duration = 0, .max_events = 0}};
    const static bd_addr_t rand_addr = {0xCD, 0xA3, 0x28, 0x11, 0x89, 0x3f};    // TODO: random address generation
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;

        gap_set_adv_set_random_addr(0, (uint8_t *)rand_addr);
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
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:
            ota_connected();
            att_set_db(decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t)->handle,
                       att_db_util_get_address());
            xTimerStart(app_timer, portMAX_DELAY);
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        temperture_notify_enable = 0;
        temperture_indicate_enable = 0;
        xTimerStop(app_timer,  portMAX_DELAY);
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        break;

    case BTSTACK_EVENT_USER_MSG:
        p_user_msg = hci_event_packet_get_user_msg(packet);
        user_msg_handler(p_user_msg->msg_id, p_user_msg->data, p_user_msg->len);
        break;
    default:
        break;
    }
}

#define gatt_service_generic_attribute                  0x1801
#define gatt_service_health_thermometer                 0x1809

#define gatt_characteristic_temperature_measurement     0x2a1c
#define gatt_characteristic_temperature_type            0x2a1d

#define temperature_type_body                           2
#define temperature_type_toe                            8

static uint8_t att_db_storage[800];

//#define V2

prog_ver_t prog_ver =
#ifdef V2
    { .major = 1, .minor = 2, .patch = 0 }
#else
    { .major = 1, .minor = 1, .patch = 0 }
#endif
;

uint8_t *init_service()
{
    static char dev_name[] = "Thermometer";
    static uint8_t service_changed[] = {0x00,0x00,0x00,0x00};

#ifdef V2
    static uint8_t temp_value_type = temperature_type_toe;
#else
    static uint8_t temp_value_type = temperature_type_body;
#endif

    att_db_util_init(att_db_storage, sizeof(att_db_storage));

    att_db_util_add_service_uuid16(GAP_SERVICE_UUID);
    att_db_util_add_characteristic_uuid16(GAP_DEVICE_NAME_UUID, ATT_PROPERTY_READ, (uint8_t *)dev_name, sizeof(dev_name) - 1);

    att_db_util_add_service_uuid16(gatt_service_generic_attribute);
    att_db_util_add_characteristic_uuid16(GAP_SERVICE_CHANGED, ATT_PROPERTY_READ, service_changed, sizeof(service_changed));

    att_db_util_add_service_uuid16(gatt_service_health_thermometer);

    att_temp_value_handle = att_db_util_add_characteristic_uuid16(gatt_characteristic_temperature_measurement,
        ATT_PROPERTY_INDICATE | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_READ | ATT_PROPERTY_DYNAMIC,
        temperature_value, sizeof(temperature_value));
    att_db_util_add_characteristic_uuid16(gatt_characteristic_temperature_type,
        ATT_PROPERTY_READ, &temp_value_type, sizeof(temp_value_type));

    att_client_desc_value_handle = att_temp_value_handle + 1;

    printf("att_temp_value_handle         = %d\n"
           "att_client_desc_value_handle  = %d\n", att_temp_value_handle, att_client_desc_value_handle);

    ota_init_service();

    return att_db_util_get_address();
}

#ifndef SIMULATION

#define I2C_SCL         GIO_GPIO_10
#define I2C_SDA         GIO_GPIO_11

void setup_peripherals_i2c_pin(void)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    SYSCTRL_ClearClkGateMulti( (1 << SYSCTRL_ClkGate_APB_I2C0)
                              | (1 << SYSCTRL_ClkGate_APB_GPIO0)
                                 | (1 << SYSCTRL_ClkGate_APB_GPIO1)
                              |(1 << SYSCTRL_ClkGate_APB_PinCtrl));
    PINCTRL_SetPadMux(10, IO_SOURCE_I2C0_SCL_O);
    PINCTRL_SetPadMux(11, IO_SOURCE_I2C0_SDO);
    PINCTRL_SelI2cSclIn(I2C_PORT, 10);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    SYSCTRL_ClearClkGateMulti(    (1 << SYSCTRL_ITEM_APB_I2C0)
                                  | (1 << SYSCTRL_ITEM_APB_SysCtrl)
                                  | (1 << SYSCTRL_ITEM_APB_PinCtrl)
                                  | (1 << SYSCTRL_ITEM_APB_GPIO1)
                                  | (1 << SYSCTRL_ITEM_APB_GPIO0));

    PINCTRL_SelI2cIn(I2C_PORT_0, I2C_SCL, I2C_SDA);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
    SYSCTRL_ClearClkGateMulti(    (1 << SYSCTRL_ITEM_APB_I2C0)
                                  | (1 << SYSCTRL_ITEM_APB_SysCtrl)
                                  | (1 << SYSCTRL_ITEM_APB_PinCtrl)
                                  | (1 << SYSCTRL_ITEM_APB_GPIO1)
                                  | (1 << SYSCTRL_ITEM_APB_GPIO0));

    PINCTRL_SelI2cIn(I2C_PORT_0, I2C_SCL, I2C_SDA);
#else
    #error unknown or unsupported chip family
#endif
}

void setup_peripherals_i2c(void)
{
    setup_peripherals_i2c_pin();
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    //init I2C module
    I2C_Config(APB_I2C0,I2C_ROLE_MASTER,I2C_ADDRESSING_MODE_07BIT,get_thermo_addr());
    I2C_ConfigClkFrequency(APB_I2C0,I2C_CLOCKFREQUENY_STANDARD);
    I2C_Enable(APB_I2C0,1);
    I2C_IntEnable(APB_I2C0,(1<<I2C_INT_CMPL)|(1<<I2C_INT_ADDR_HIT));
#endif
    i2c_init(I2C_PORT_0);
}

#endif

uint32_t setup_profile(void *data, void *user_data)
{
    init_service();
    app_timer = xTimerCreate("app",
                            pdMS_TO_TICKS(2000),
                            pdTRUE,
                            NULL,
                            app_timer_callback);
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);

#ifndef SIMULATION
    setup_peripherals_i2c();
    setup_env_sensor();
#endif

    return 0;
}
