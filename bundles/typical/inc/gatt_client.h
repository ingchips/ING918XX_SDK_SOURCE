// ----------------------------------------------------------------------------
// Copyright Message
// ----------------------------------------------------------------------------
//
// INGCHIPS confidential and proprietary.
// COPYRIGHT (c) 2018 by INGCHIPS
//
// All rights are reserved. Reproduction in whole or in part is
// prohibited without the written consent of the copyright owner.
//
//
// ----------------------------------------------------------------------------

#ifndef _GATT_CLIENT_H
#define _GATT_CLIENT_H

#include <stdint.h>
#include "bluetooth.h"
#include "bt_types.h"
#include "btstack_util.h"
#include "btstack_event.h"
#include "btstack_linked_list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t start_group_handle;
    uint16_t end_group_handle;
    uint16_t uuid16;
    uint8_t  uuid128[16];
} gatt_client_service_t;

typedef struct {
    uint16_t start_handle;
    uint16_t value_handle;
    uint16_t end_handle;
    uint16_t properties;
    uint16_t uuid16;
    uint8_t  uuid128[16];
} gatt_client_characteristic_t;

typedef struct {
    uint16_t handle;
    uint16_t uuid16;
    uint8_t  uuid128[16];
} gatt_client_characteristic_descriptor_t;

/**
 * @brief Discovers all primary services. For each found service, an le_service_event_t with type set to GATT_EVENT_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t, with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
 */
uint8_t gatt_client_discover_primary_services(user_packet_handler_t callback, hci_con_handle_t con_handle);

/**
 * @brief Discovers a specific primary service given its UUID. This service may exist multiple times. For each found service, an le_service_event_t with type set to GATT_EVENT_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t, with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
 */
uint8_t gatt_client_discover_primary_services_by_uuid16(user_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t uuid16);
uint8_t gatt_client_discover_primary_services_by_uuid128(user_packet_handler_t callback, hci_con_handle_t con_handle, const uint8_t * uuid128);
uint8_t gatt_client_discover_characteristics_for_service(user_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t *service);

/**
 * @brief Finds included services within the specified service. For each found included service, an le_service_event_t with type set to GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery. Information about included service type (primary/secondary) can be retrieved either by sending an ATT find information request for the returned start group handle (returning the handle and the UUID for primary or secondary service) or by comparing the service to the list of all primary services.
 */
uint8_t gatt_client_find_included_services_for_service(user_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t *service);


/**
 * @brief The following four functions are used to discover all characteristics within the specified service or handle range, and return those that match the given UUID. For each found characteristic, an le_characteristic_event_t with type set to GATT_EVENT_CHARACTERISTIC_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
 */
uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid16(btstack_packet_handler_t callback,
                const hci_con_handle_t con_handle, const uint16_t start_handle, const uint16_t end_handle, const uint16_t uuid16);
uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid128(btstack_packet_handler_t callback,
                const hci_con_handle_t con_handle, const uint16_t start_handle, const uint16_t end_handle,
                const uint8_t * uuid128);
uint8_t gatt_client_discover_characteristics_for_service_by_uuid16 (btstack_packet_handler_t callback,
                const hci_con_handle_t con_handle, gatt_client_service_t  *service,
                const uint16_t  uuid16);
uint8_t gatt_client_discover_characteristics_for_service_by_uuid128(btstack_packet_handler_t callback,
                const hci_con_handle_t con_handle, gatt_client_service_t  *service, const uint8_t  * uuid128);

/**
 * @brief Discovers attribute handle and UUID of a characteristic descriptor within the specified characteristic. For each found descriptor, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
 */
uint8_t gatt_client_discover_characteristic_descriptors(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t  *characteristic);

/**
 * @brief Reads the characteristic value using the characteristic's value handle. If the characteristic value is found, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 */
uint8_t gatt_client_read_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t  *characteristic);
uint8_t gatt_client_read_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle);

/**
 * @brief Reads the characteric value of all characteristics with the uuid. For each found, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 */
uint8_t gatt_client_read_value_of_characteristics_by_uuid16(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
uint8_t gatt_client_read_value_of_characteristics_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint8_t * uuid128);

/**
 * @brief Reads the long characteristic value using the characteristic's value handle. The value will be returned in several blobs. For each blob, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT and updated value offset will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, mark the end of read.
 */
uint8_t gatt_client_read_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t  *characteristic);
uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle);
uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t offset);

/*
 * @brief Read multiple characteristic values
 * @param number handles
 * @param list_of_handles list of handles
 */
uint8_t gatt_client_read_multiple_characteristic_values(btstack_packet_handler_t callback, hci_con_handle_t con_handle, int num_value_handles, uint16_t * value_handles);

/**
 * @brief Writes the characteristic value using the characteristic's value handle without an acknowledgment that the write was successfully performed.
 */
uint8_t gatt_client_write_value_of_characteristic_without_response(hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);

/**
 * @brief Writes the authenticated characteristic value using the characteristic's value handle without an acknowledgment that the write was successfully performed.
 */
uint8_t gatt_client_signed_write_without_response(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t handle, uint16_t message_len, uint8_t  * message);

/**
 * @brief Writes the characteristic value using the characteristic's value handle. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 */
uint8_t gatt_client_write_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);
uint8_t gatt_client_write_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);
uint8_t gatt_client_write_long_value_of_characteristic_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t offset, uint16_t length, uint8_t  * data);

/**
 * @brief Writes of the long characteristic value using the characteristic's value handle. It uses server response to validate that the write was correctly received. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 */
uint8_t gatt_client_reliable_write_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);

/**
 * @brief Reads the characteristic descriptor using its handle. If the characteristic descriptor is found, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 */
uint8_t gatt_client_read_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor);
uint8_t gatt_client_read_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle);

/**
 * @brief Reads the long characteristic descriptor using its handle. It will be returned in several blobs. For each blob, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 */
uint8_t gatt_client_read_long_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor);
uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle);
uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t offset);

/**
 * @brief Writes the characteristic descriptor using its handle. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 */
uint8_t gatt_client_write_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor, uint16_t length, uint8_t  * data);
uint8_t gatt_client_write_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t length, uint8_t  * data);
uint8_t gatt_client_write_long_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor, uint16_t length, uint8_t  * data);
uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t length, uint8_t  * data);
uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t offset, uint16_t length, uint8_t  * data);

/**
 * @brief Writes the client characteristic configuration of the specified characteristic. It is used to subscribe for notifications or indications of the characteristic value. For notifications or indications specify: GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION resp. GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION as configuration value.
 */
uint8_t gatt_client_write_client_characteristic_configuration(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic, uint16_t configuration);


/**
 * @brief -> gatt complete event
 */
uint8_t gatt_client_prepare_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint16_t length, uint8_t * data);

/**
 * @brief -> gatt complete event
 */
uint8_t gatt_client_execute_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle);

/**
 * @brief -> gatt complete event
 */
uint8_t gatt_client_cancel_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
void gatt_client_deserialize_service(const uint8_t *packet, int offset, gatt_client_service_t *service);
void gatt_client_deserialize_characteristic(const uint8_t * packet, int offset, gatt_client_characteristic_t * characteristic);
void gatt_client_deserialize_characteristic_descriptor(const uint8_t * packet, int offset, gatt_client_characteristic_descriptor_t * descriptor);

int ble_gatts_find_svc(const ble_uuid_t *uuid, uint16_t *out_handle);
int ble_gatts_find_chr(uint16_t srv_handle, const ble_uuid_t *uuid,uint16_t *out_att_chr);

typedef struct gatt_client_notification {
    btstack_linked_item_t    item;
    btstack_packet_handler_t callback;
    uint16_t attribute_handle;
} gatt_client_notification_t;

/**
 * @brief MTU is available after the first query has completed. If status is equal to 0, it returns the real value, otherwise the default value of 23.
 */
uint8_t gatt_client_get_mtu(hci_con_handle_t con_handle, uint16_t * mtu);

/**
 * @brief Returns if the GATT client is ready to receive a query. It is used with daemon.
 */
int gatt_client_is_ready(hci_con_handle_t con_handle);

/**
 * @brief Register for notifications and indications of a characteristic enabled by gatt_client_write_client_characteristic_configuration
 * @param notification struct used to store registration
 * @param packet_handler
 * @param con_handle
 * @param characteristic
 */
void gatt_client_listen_for_characteristic_value_updates(gatt_client_notification_t * notification, btstack_packet_handler_t packet_handler, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic);

/**
 * @brief Register for general events
 * @param handler
 */
void gatt_client_register_handler(btstack_packet_handler_t handler);

// only used for testing
void gatt_client_pts_suppress_mtu_exchange(void);

#pragma pack(push, 1)

typedef struct
{
    uint16_t handle;
    uint8_t  value[0];          // length is given in `value_size`
} gatt_event_value_packet_t;

typedef struct
{
    uint16_t handle;
    uint16_t offset;
    uint8_t  value[0];          // length is given in `value_size`
} gatt_event_long_value_packet_t;

#pragma pack(pop)

/**
 * @brief Get field handle from event GATT_EVENT_QUERY_COMPLETE
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t gatt_event_query_complete_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}
/**
 * @brief Get field status from event GATT_EVENT_QUERY_COMPLETE
 * @param event packet
 * @return status
 * @note: btstack_type 1
 */
static __INLINE uint8_t gatt_event_query_complete_get_status(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 4);
}

/**
 * @brief Get field handle from event GATT_EVENT_SERVICE_QUERY_RESULT
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t gatt_event_service_query_result_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}
/**
 * @brief Get field service from event GATT_EVENT_SERVICE_QUERY_RESULT
 * @param event packet
 * @param Pointer to storage for service
 * @note: btstack_type X
 */
static __INLINE void gatt_event_service_query_result_get_service(const uint8_t * event, gatt_client_service_t * service){
    gatt_client_deserialize_service(event, 4, service);
}

/**
 * @brief Get field handle from event GATT_EVENT_CHARACTERISTIC_QUERY_RESULT
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t gatt_event_characteristic_query_result_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}
/**
 * @brief Get field characteristic from event GATT_EVENT_CHARACTERISTIC_QUERY_RESULT
 * @param event packet
 * @param Pointer to storage for characteristic
 * @note: btstack_type Y
 */
static __INLINE void gatt_event_characteristic_query_result_get_characteristic(const uint8_t * event, gatt_client_characteristic_t * characteristic){
    gatt_client_deserialize_characteristic(event, 4, characteristic);
}

/**
 * @brief Get field handle from event GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t gatt_event_included_service_query_result_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}
/**
 * @brief Get field include_handle from event GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT
 * @param event packet
 * @return include_handle
 * @note: btstack_type 2
 */
static __INLINE uint16_t gatt_event_included_service_query_result_get_include_handle(const uint8_t * event){
    return little_endian_read_16(event, 4);
}
/**
 * @brief Get field service from event GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT
 * @param event packet
 * @param Pointer to storage for service
 * @note: btstack_type X
 */
static __INLINE void gatt_event_included_service_query_result_get_service(const uint8_t * event, gatt_client_service_t * service){
    gatt_client_deserialize_service(event, 6, service);
}

/**
 * @brief Get field handle from event GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t gatt_event_all_characteristic_descriptors_query_result_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}
/**
 * @brief Get field characteristic_descriptor from event GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT
 * @param event packet
 * @param Pointer to storage for characteristic_descriptor
 * @note: btstack_type Z
 */
static __INLINE void gatt_event_all_characteristic_descriptors_query_result_get_characteristic_descriptor(const uint8_t * event, gatt_client_characteristic_descriptor_t * characteristic_descriptor){
    gatt_client_deserialize_characteristic_descriptor(event, 4, characteristic_descriptor);
}

/**
 * @brief Get value from event GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT
 * @param[in]           event packet
 * @param[in]           event_size
 * @param[out]          value_size
 * @return gatt_event_value_packet_t *
 */
static __INLINE const gatt_event_long_value_packet_t * gatt_event_long_characteristic_value_query_result_parse(const uint8_t *event_packet, uint16_t event_size, uint16_t *value_size)
{
    *value_size = event_size - 1 - (uint16_t)sizeof(gatt_event_long_value_packet_t);
    return decode_event_offset(event_packet, gatt_event_long_value_packet_t, 1);
}

/**
 * @brief Get value from event GATT_EVENT_NOTIFICATION
 * @param[in]           event packet
 * @param[in]           event_size
 * @param[out]          value_size
 * @return gatt_event_value_packet_t *
 */
static __INLINE const gatt_event_value_packet_t * gatt_event_notification_parse(const uint8_t *event_packet, uint16_t event_size, uint16_t *value_size)
{
    *value_size = event_size - 1 - (uint16_t)sizeof(gatt_event_value_packet_t);
    return decode_event_offset(event_packet, gatt_event_value_packet_t, 1);
}

/**
 * @brief Get value from event GATT_EVENT_INDICATION
 * @param[in]           event packet
 * @param[in]           event_size
 * @param[out]          value_size
 * @return gatt_event_notification_t *
 */
static __INLINE const gatt_event_value_packet_t * gatt_event_indication_parse(const uint8_t *event_packet, uint16_t event_size, uint16_t *value_size)
{
    *value_size = event_size - 1 - (uint16_t)sizeof(gatt_event_value_packet_t);
    return decode_event_offset(event_packet, gatt_event_value_packet_t, 1);
}

/**
 * @brief Get value from event GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT
 * @param[in]           event packet
 * @param[in]           event_size
 * @param[out]          value_size
 * @return gatt_event_notification_t *
 */
static __INLINE const gatt_event_value_packet_t * gatt_event_characteristic_value_query_result_parse(const uint8_t *event_packet, uint16_t event_size, uint16_t *value_size)
{
    *value_size = event_size - 1 - (uint16_t)sizeof(gatt_event_value_packet_t);
    return decode_event_offset(event_packet, gatt_event_value_packet_t, 1);
}

/**
 * @brief Get value from event GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT
 * @param[in]           event packet
 * @param[in]           event_size
 * @param[out]          value_size
 * @return gatt_event_notification_t *
 */
static __INLINE const gatt_event_value_packet_t * gatt_event_characteristic_descriptor_query_result_parse(const uint8_t *event_packet, uint16_t event_size, uint16_t *value_size)
{
    *value_size = event_size - 1 - (uint16_t)sizeof(gatt_event_value_packet_t);
    return decode_event_offset(event_packet, gatt_event_value_packet_t, 1);
}

/**
 * @brief Get value from event GATT_EVENT_LONG_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT
 * @param[in]           event packet
 * @param[in]           event_size
 * @param[out]          value_size
 * @return gatt_event_value_packet_t *
 */
static __INLINE const gatt_event_long_value_packet_t * gatt_event_long_characteristic_descriptor_query_result_parse(const uint8_t *event_packet, uint16_t event_size, uint16_t *value_size)
{
    *value_size = event_size - 1 - (uint16_t)sizeof(gatt_event_long_value_packet_t);
    return decode_event_offset(event_packet, gatt_event_long_value_packet_t, 1);
}

/**
 * @brief Get field handle from event GATT_EVENT_MTU
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t gatt_event_mtu_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}
/**
 * @brief Get field MTU from event GATT_EVENT_MTU
 * @param event packet
 * @return MTU
 * @note: btstack_type 2
 */
static __INLINE uint16_t gatt_event_mtu_get_mtu(const uint8_t * event){
    return little_endian_read_16(event, 4);
}

#ifdef __cplusplus
}
#endif

#endif
