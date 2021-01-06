#include <string.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "eflash.h"
#include "att_db_util.h"

#include "dev_info.h"
#include "hal.h"

static uint8_t adv_data[31] = {
    #include "../data/advertising.adv"
};

static uint8_t scan_data[31] = {
    #include "../data/scan_response.adv"
};

char_def_t qcloud_char_defs[Q_CHAR_EVENT + 1] = {0};

/**
 * @brief  get device product id
 * @param  product_id  the buf storage product id, 10 bytes permanent
 * @return 0 is success, other is error
 */
int ble_get_product_id(char *product_id)
{
    memcpy(product_id, PRODUCT_ID, strlen(PRODUCT_ID));
    return 0;
}

/**
 * @brief  get device name
 * @param  device_name     the buf storage device name, the max length of the device name is 48 bytes
 * @return length of device name, 0 is error
 */
int ble_get_device_name(char *device_name)
{
    memcpy(device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    return strlen(DEVICE_NAME);
}

/**
 * @brief  get device secret
 * @param  psk         the buf storage secret, 24 bytes permanent
 * @return 0 is success, other is error
 */
int ble_get_psk(char *psk)
{
    memcpy(psk, SECRET_KEY, strlen(SECRET_KEY));
    return 0;
}

/**
 * @brief  get mac address
 * @param  mac     the buf storage mac, 6 bytes permanent
 * @return 0 is success, other is error
 */
int ble_get_mac(char *mac)
{
    extern const bd_addr_t rand_addr;
    memcpy(mac, rand_addr, sizeof(rand_addr));
    return 0;
}

/**
 * @brief write data to flash
 * @param flash_addr write address in flash
 * @param write_buf  point to write buf
 * @param write_len  length of data to write
 * @return write_len is success, other is error
 */
int ble_write_flash(uint32_t flash_addr, const char *write_buf, uint16_t write_len)
{
    uint16_t new_len = (write_len + 3) & ~0x3;
    program_flash(flash_addr, (const uint8_t *)write_buf, new_len);
    return write_len;
}

/**
 * @brief read data from flash
 * @param flash_addr read address from flash
 * @param read_buf   point to read buf
 * @param read_len   length of data to read
 * @return read_len is success, other is error
 */
int ble_read_flash(uint32_t flash_addr, char *read_buf, uint16_t read_len)
{
    memcpy(read_buf, (void *)flash_addr, read_len);
    return read_len;
}

void write_to_uuid128(uint8_t *base, uint16_t uuid)
{
    base[2] = uuid >> 8;
    base[3] = uuid & 0xff;
}

/**
 * @brief add llsync services to ble stack
 * @param qiot_service_init_s llsync service
 * @return none
 */
void ble_services_add(const qiot_service_init_s *p_service)
{
    uint8_t buffer[16];
    reverse_128(p_service->service_uuid128, buffer);

    write_to_uuid128(buffer, p_service->service_uuid16);
    att_db_util_add_service_uuid128(buffer);
    
    write_to_uuid128(buffer, p_service->device_info.uuid16);
    qcloud_char_defs[Q_CHAR_DEVICE_INFO].handle = att_db_util_add_characteristic_uuid128(buffer,
                                          p_service->device_info.gatt_char_props | ATT_PROPERTY_DYNAMIC,
                                          NULL, 0);
    
    write_to_uuid128(buffer, p_service->data.uuid16);
    qcloud_char_defs[Q_CHAR_DATA].handle = att_db_util_add_characteristic_uuid128(buffer,
                                          p_service->data.gatt_char_props | ATT_PROPERTY_DYNAMIC,
                                          NULL, 0);
    
    write_to_uuid128(buffer, p_service->event.uuid16);
    qcloud_char_defs[Q_CHAR_EVENT].handle = att_db_util_add_characteristic_uuid128(buffer,
                                          p_service->event.gatt_char_props,
                                          NULL, 0);
    qcloud_char_defs[Q_CHAR_DEVICE_INFO].on_write = p_service->device_info.on_write;
    qcloud_char_defs[Q_CHAR_DATA].on_write = p_service->data.on_write;
}

const static ext_adv_set_en_t adv_sets_en[] = { {.handle = 0, .duration = 0, .max_events = 0} };

/**
 * @brief start llsync advertising
 * @param adv a pointer point to advertising data
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_advertising_start(adv_info_s *adv)
{
    int index = 3; // first 3 bytes: Flags
    
    // 0x03 - Complete List of 16-bit Service Class UUIDs
    adv_data[index++] = 1 + adv->uuid_info.uuid_num * 2;
    adv_data[index++] = 3;
    memcpy(adv_data + index, adv->uuid_info.uuids, adv->uuid_info.uuid_num * 2);
    index += adv->uuid_info.uuid_num * 2;
    
    // 0xFF - Manufacturer Specific Data
    adv_data[index++] = 1 + 2 + adv->manufacturer_info.adv_data_len;
    adv_data[index++] = 0xff;
    memcpy(adv_data + index, &adv->manufacturer_info.company_identifier, 2);
    index += 2;
    memcpy(adv_data + index, adv->manufacturer_info.adv_data, adv->manufacturer_info.adv_data_len);
    index += adv->manufacturer_info.adv_data_len;
    
    scan_data[0] = strlen(DEVICE_NAME) + 1;
    memcpy(scan_data + 2, DEVICE_NAME, strlen(DEVICE_NAME));

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
    gap_set_ext_adv_data(0, index, (uint8_t*)adv_data);
    gap_set_ext_scan_response_data(0, scan_data[0] + 1, (uint8_t*)scan_data);
    gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);

    return BLE_QIOT_RS_OK;
}

/**
 * @brief stop advertising
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_advertising_stop(void)
{
    gap_set_ext_adv_enable(0, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
    return BLE_QIOT_RS_OK;
}

/**
 * @brief send a notification to host, use characteristic IOT_BLE_UUID_EVENT
 * @param buf a pointer point to indication information
 * @param len indication information length
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_send_notify(uint8_t *buf, uint8_t len)
{
    int r = att_server_notify(0, qcloud_char_defs[Q_CHAR_EVENT].handle,
                              buf, len);
    return r == 0 ? BLE_QIOT_RS_OK : BLE_QIOT_RS_ERR;
}

/**
 * @brief get the max size of data that user can used, normally is ATT_MTU - 3
 * @return the value
 */
uint16_t ble_get_user_data_mtu_size(void)
{
    return att_server_get_mtu(0);
}

stack_timer_t stack_timers[MAX_TIMER_NUMBER] = {0};

static void app_timer_callback(TimerHandle_t pxTimer)
{
    int index = (int)pvTimerGetTimerID(pxTimer);
    btstack_push_user_msg(USER_MESSAGE_STACK_TIMER, NULL, index);
}

void invoke_stack_timer_cb(int index)
{
    stack_timers[index].on_timer(NULL);
}

/**
 * @brief create a timer
 * @param type timer type
 * @param timeout_handle timer callback
 * @return timer identifier is return, NULL is error
 */
ble_timer_t ble_timer_create(uint8_t type, ble_timer_cb timeout_handle)
{
    int i;
    for (i = 0; i < MAX_TIMER_NUMBER; i++)
    {
        if (NULL == stack_timers[i].handle)
            break;
    }

    if (i >= MAX_TIMER_NUMBER)
        return NULL;
    
    stack_timers[i].handle = xTimerCreate("",
                                pdMS_TO_TICKS(2000),
                                type == BLE_TIMER_PERIOD_TYPE ? pdTRUE : pdFALSE,
                                (void *)i,
                                app_timer_callback);
    return (ble_timer_t)(i + 1);
}

/**
 * @brief start a timer
 * @param timer_id Timer identifier
 * @param period timer period(unit: ms)
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_timer_start(ble_timer_t timer_id, uint32_t period)
{
    int i = (int)timer_id - 1;
    if (xTimerChangePeriod(stack_timers[i].handle, 
                           pdMS_TO_TICKS(period), portMAX_DELAY) == pdPASS)
        return BLE_QIOT_RS_OK;
    else
        return BLE_QIOT_RS_ERR;
}

/**
 * @brief stop a timer
 * @param timer_id Timer identifier
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_timer_stop(ble_timer_t timer_id)
{
    int i = (int)timer_id - 1;
    if (xTimerStop(stack_timers[i].handle, portMAX_DELAY) == pdPASS)
        return BLE_QIOT_RS_OK;
    else
        return BLE_QIOT_RS_ERR;
}

/**
 * @brief delete a timer
 * @param timer_id Timer identifier
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_timer_delete(ble_timer_t timer_id)
{
    int i = (int)timer_id - 1;
    if (xTimerDelete(stack_timers[i].handle, portMAX_DELAY) == pdPASS)
        return BLE_QIOT_RS_OK;
    else
        return BLE_QIOT_RS_ERR;
}
