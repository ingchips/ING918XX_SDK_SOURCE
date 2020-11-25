/*
* Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
* Licensed under the MIT License (the "License"); you may not use this file except in
* compliance with the License. You may obtain a copy of the License at
* http://opensource.org/licenses/MIT
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
* either express or implied. See the License for the specific language governing permissions and
* limitations under the License.
*
*/
#ifdef __cplusplus
extern "C" {
#endif

#include "ble_qiot_template.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "ble_qiot_export.h"
#include "ble_qiot_common.h"
#include "ble_qiot_param_check.h"

int ble_property_power_switch_set(const char *data, uint16_t len);
int ble_property_power_switch_get(char *data, uint16_t buf_len);
int ble_property_brightness_set(const char *data, uint16_t len);
int ble_property_brightness_get(char *data, uint16_t buf_len);
int ble_property_color_set(const char *data, uint16_t len);
int ble_property_color_get(char *data, uint16_t buf_len);
int ble_property_color_temp_set(const char *data, uint16_t len);
int ble_property_color_temp_get(char *data, uint16_t buf_len);
int ble_property_name_set(const char *data, uint16_t len);
int ble_property_name_get(char *data, uint16_t buf_len);

static ble_property_t sg_ble_property_array[BLE_QIOT_PROPERTY_ID_BUTT] = {
	{ble_property_power_switch_set, ble_property_power_switch_get, BLE_QIOT_PROPERTY_AUTH_RW, BLE_QIOT_DATA_TYPE_BOOL},
	{ble_property_brightness_set,   ble_property_brightness_get,   BLE_QIOT_PROPERTY_AUTH_RW, BLE_QIOT_DATA_TYPE_INT},
	{ble_property_color_set,        ble_property_color_get,        BLE_QIOT_PROPERTY_AUTH_RW, BLE_QIOT_DATA_TYPE_ENUM},
	{ble_property_color_temp_set,   ble_property_color_temp_get,   BLE_QIOT_PROPERTY_AUTH_RW, BLE_QIOT_DATA_TYPE_INT},
	{ble_property_name_set,         ble_property_name_get,         BLE_QIOT_PROPERTY_AUTH_RW, BLE_QIOT_DATA_TYPE_STRING},
};

static bool ble_check_space_enough_by_type(uint8_t type, uint16_t left_size)
{
    switch(type)
    {
        case BLE_QIOT_DATA_TYPE_BOOL:
            return left_size >= sizeof(uint8_t);
        case BLE_QIOT_DATA_TYPE_INT:
        case BLE_QIOT_DATA_TYPE_FLOAT:
        case BLE_QIOT_DATA_TYPE_TIME:
            return left_size >= sizeof(uint32_t);
        case BLE_QIOT_DATA_TYPE_ENUM:
            return left_size >= sizeof(uint16_t);
        default:
            // string length is unknow, default true
            return true;
    }
}

static uint16_t ble_check_ret_value_by_type(uint8_t type, uint16_t buf_len, uint16_t ret_val)
{
    switch(type)
    {
        case BLE_QIOT_DATA_TYPE_BOOL:
            return ret_val <= sizeof(uint8_t);
        case BLE_QIOT_DATA_TYPE_INT:
        case BLE_QIOT_DATA_TYPE_FLOAT:
        case BLE_QIOT_DATA_TYPE_TIME:
            return ret_val <= sizeof(uint32_t);
        case BLE_QIOT_DATA_TYPE_ENUM:
            return ret_val <= sizeof(uint16_t);
        default:
            // string length is unknow, default true
            return ret_val <= buf_len;
    }
}

uint8_t ble_get_property_type_by_id(uint8_t id)
{
    if (id >= BLE_QIOT_PROPERTY_ID_BUTT) {
        ble_qiot_log_e("invalid property id %d", id);
        return BLE_QIOT_DATA_TYPE_BUTT;
    }
    return sg_ble_property_array[id].type;
}

int ble_user_property_set_data(const e_ble_tlv *tlv)
{
    POINTER_SANITY_CHECK(tlv, BLE_QIOT_RS_ERR_PARA);
    if (tlv->id >= BLE_QIOT_PROPERTY_ID_BUTT) {
        ble_qiot_log_e("invalid property id %d", tlv->id);
        return BLE_QIOT_RS_ERR;
    }

    if (NULL != sg_ble_property_array[tlv->id].set_cb) {
        if (0 != sg_ble_property_array[tlv->id].set_cb(tlv->val, tlv->len)) {
            ble_qiot_log_e("set property id %d failed", tlv->id);
            return BLE_QIOT_RS_ERR;
        }else {
            return BLE_QIOT_RS_OK;
        }
    }
    ble_qiot_log_e("invalid set callback, id %d", tlv->id);

    return BLE_QIOT_RS_ERR;
}

int ble_user_property_get_data_by_id(uint8_t id, char *buf, uint16_t buf_len)
{
    int ret_len = 0;

    POINTER_SANITY_CHECK(buf, BLE_QIOT_RS_ERR_PARA);
    if (id >= BLE_QIOT_PROPERTY_ID_BUTT) {
        ble_qiot_log_e("invalid property id %d", id);
        return -1;
    }

    if (NULL != sg_ble_property_array[id].get_cb) {
        if (!ble_check_space_enough_by_type(sg_ble_property_array[id].type, buf_len)) {
            ble_qiot_log_e("not enough space get property id %d data", id);
            return -1;
        }
        ret_len = sg_ble_property_array[id].get_cb(buf, buf_len);
        if (ret_len < 0) {
            ble_qiot_log_e("get property id %d data failed", id);
            return -1;
        }else {
            if (ble_check_ret_value_by_type(sg_ble_property_array[id].type, buf_len, ret_len)){
                return ret_len;
            }else{
                ble_qiot_log_e("property id %d length invalid", id);
                return -1;
            }
        }
    }
    ble_qiot_log_e("invalid callback, property id %d", id);

    return 0;
}

int ble_user_property_report_reply_handle(uint8_t result)
{
    ble_qiot_log_d("report reply result %d", result);

    return BLE_QIOT_RS_OK;
}

static int ble_event_get_status_report_status(char *data, uint16_t buf_len)
{
	return sizeof(uint8_t);
}

static int ble_event_get_status_report_message(char *data, uint16_t buf_len)
{
	return buf_len;
}

static ble_event_param sg_ble_event_status_report_array[BLE_QIOT_EVENT_STATUS_REPORT_PARAM_ID_BUTT] = {
	{ble_event_get_status_report_status,  BLE_QIOT_DATA_TYPE_BOOL},
	{ble_event_get_status_report_message,  BLE_QIOT_DATA_TYPE_STRING},
};

static int ble_event_get_low_voltage_voltage(char *data, uint16_t buf_len)
{
	return sizeof(float);
}

static ble_event_param sg_ble_event_low_voltage_array[BLE_QIOT_EVENT_LOW_VOLTAGE_PARAM_ID_BUTT] = {
	{ble_event_get_low_voltage_voltage,  BLE_QIOT_DATA_TYPE_FLOAT},
};

static int ble_event_get_hardware_fault_name(char *data, uint16_t buf_len)
{
	return buf_len;
}

static int ble_event_get_hardware_fault_error_code(char *data, uint16_t buf_len)
{
	return sizeof(uint32_t);
}

static ble_event_param sg_ble_event_hardware_fault_array[BLE_QIOT_EVENT_HARDWARE_FAULT_PARAM_ID_BUTT] = {
	{ble_event_get_hardware_fault_name,  BLE_QIOT_DATA_TYPE_STRING},
	{ble_event_get_hardware_fault_error_code,  BLE_QIOT_DATA_TYPE_INT},
};

static ble_event_t sg_ble_event_array[BLE_QIOT_EVENT_ID_BUTT] = {
	{sg_ble_event_status_report_array, sizeof(sg_ble_event_status_report_array) / sizeof(ble_event_param)},
	{sg_ble_event_low_voltage_array,  sizeof(sg_ble_event_low_voltage_array) / sizeof(ble_event_param)},
	{sg_ble_event_hardware_fault_array, sizeof(sg_ble_event_hardware_fault_array) / sizeof(ble_event_param)},
};

int ble_event_get_id_array_size(uint8_t event_id)
{
    if (event_id >= BLE_QIOT_EVENT_ID_BUTT) {
        ble_qiot_log_e("invalid event id %d", event_id);
        return -1;
    }

    return sg_ble_event_array[event_id].array_size;
}

uint8_t ble_event_get_param_id_type(uint8_t event_id, uint8_t param_id)
{
    if (event_id >= BLE_QIOT_EVENT_ID_BUTT) {
        ble_qiot_log_e("invalid event id %d", event_id);
        return BLE_QIOT_DATA_TYPE_BUTT;
    }
    if (param_id >= sg_ble_event_array[event_id].array_size) {
        ble_qiot_log_e("invalid param id %d", param_id);
        return BLE_QIOT_DATA_TYPE_BUTT;
    }

    return sg_ble_event_array[event_id].event_array[param_id].type;
}

int ble_event_get_data_by_id(uint8_t event_id, uint8_t param_id, char *out_buf, uint16_t buf_len)
{
    int ret_len = 0;

    if (event_id >= BLE_QIOT_EVENT_ID_BUTT) {
        ble_qiot_log_e("invalid event id %d", event_id);
        return -1;
    }
    if (param_id >= sg_ble_event_array[event_id].array_size) {
        ble_qiot_log_e("invalid param id %d", param_id);
        return -1;
    }
    if (NULL == sg_ble_event_array[event_id].event_array[param_id].get_cb) {
        ble_qiot_log_e("invalid callback, event id %d, param id %d", event_id, param_id);
        return 0;
    }

    if (!ble_check_space_enough_by_type(sg_ble_event_array[event_id].event_array[param_id].type, buf_len)) {
        ble_qiot_log_e("not enough space get data, event id %d, param id %d", event_id, param_id);
        return -1;
    }
    ret_len = sg_ble_event_array[event_id].event_array[param_id].get_cb(out_buf, buf_len);
    if (ret_len < 0) {
        ble_qiot_log_e("get event data failed, event id %d, param id %d", event_id, param_id);
        return -1;
    }else {
        if (ble_check_ret_value_by_type(sg_ble_event_array[event_id].event_array[param_id].type, buf_len, ret_len)){
            return ret_len;
        }else{
            ble_qiot_log_e("evnet data length invalid, event id %d, param id %d", event_id, param_id);
            return -1;
        }
    }
}

int ble_user_event_reply_handle(uint8_t event_id, uint8_t result)
{
    ble_qiot_log_d("event id %d, reply result %d", event_id, result);

    return BLE_QIOT_RS_OK;
}


#ifdef __cplusplus
}
#endif
