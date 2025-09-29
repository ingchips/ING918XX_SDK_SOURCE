/** @file
 *  @brief generic access profile
 * Copyright Message
 *
 *  INGCHIPS confidential and proprietary.
 *  COPYRIGHT (c) 2018-2023 by INGCHIPS
 *
 *  All rights are reserved. Reproduction in whole or in part is
 *  prohibited without the written consent of the copyright owner.
 *
 *
*/
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

/**
 * @brief Bluetooth
 * @defgroup Bluetooth_gatt_client
 * @ingroup bluetooth_stack
 * @{
 */

#define UUID128_LEN     (16)

#pragma pack(push, 1)

/** gatt event packet */
typedef struct
{
    uint16_t handle;            // Attribute handle
    uint8_t  value[0];          // length is given in `value_size`
} gatt_event_value_packet_t;

/** type for gatt event long value packet */
typedef struct
{
    uint16_t handle;            // Attribute handle
    uint16_t offset;            // Offset in the value
    uint8_t  value[0];          // length is given in `value_size`
} gatt_event_long_value_packet_t;

/** type of gatt event query complete */
typedef struct
{
    hci_con_handle_t handle;    // Connection handle
    uint8_t status;             // Status of the query
} gatt_event_query_complete_t;

/** type of gatt event service query result */
typedef struct
{
    uint16_t start_group_handle;    // Start group handle of the service
    uint16_t end_group_handle;      // End group handle of the service
    uint8_t  uuid128[UUID128_LEN];  // UUID of the service
} gatt_client_service_t;

/** type of gatt service query result */
typedef struct
{
    hci_con_handle_t     handle;    // Connection handle
    gatt_client_service_t service;  // Service information
} gatt_event_service_query_result_t;

/** typedef gatt client characteristic */
typedef struct {
    uint16_t start_handle;          // Start handle of the characteristic
    uint16_t value_handle;          // Handle of the characteristic value
    uint16_t end_handle;            // End handle of the characteristic
    uint16_t properties;            // Properties of the characteristic, e.g., read, write, notify
    uint8_t  uuid128[UUID128_LEN];  // UUID of the characteristic
} gatt_client_characteristic_t;

/** typedef gatt client characteristic query result */
typedef struct
{
    hci_con_handle_t             handle;            // Connection handle
    gatt_client_characteristic_t characteristic;    // Characteristic information
} gatt_event_characteristic_query_result_t;

/** typedef gatt client characteristic descriptor */
typedef struct {
    uint16_t handle;                // Handle of the characteristic descriptor
    uint8_t  uuid128[UUID128_LEN];  // UUID of the characteristic descriptor, 128 bit
} gatt_client_characteristic_descriptor_t;

/** typedef gatt characteristic descriptor query result */
typedef struct
{
    hci_con_handle_t                        handle;     // Connection handle
    gatt_client_characteristic_descriptor_t descriptor; // Characteristic descriptor information
} gatt_event_characteristic_descriptor_query_result_t;

/** typedef gatt included service query result */
typedef struct
{
    hci_con_handle_t      handle;           // Connection handle
    uint16_t              include_handle;   // Handle of the included service
    gatt_client_service_t service;          // Included service information
} gatt_event_included_service_query_result_t;

/** typedef all characteristic descriptors query result */
typedef struct {
    hci_con_handle_t                        handle;     // Connection handle
    gatt_client_characteristic_descriptor_t descriptor; // Characteristic descriptor information
} gatt_event_all_characteristic_descriptors_query_result_t;

#pragma pack(pop)

#define MAX_NR_EATT_CHANNELS 5

/**
 * @brief Setup Enhanced LE Bearer with up to 5 channels on existing LE connection
 * @param callback for GATT_EVENT_CONNECTED and GATT_EVENT_DISCONNECTED events
 * @param con_handle
 * @param num_channels
 * @param storage_buffer for L2CAP connection
 * @param storage_size - each channel requires (2 * ATT MTU) + 10 bytes
 * @return
 */
uint8_t gatt_client_le_enhanced_connect(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint8_t num_channels, uint8_t * storage_buffer, uint16_t storage_size);

/**
 * @brief Discovers all primary services. For each found service, an le_service_event_t with type set to GATT_EVENT_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t, with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
 * @param callback          Callback to discover primary services.
 * @param con_handle        Connection handle to discover services on.
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_discover_primary_services(user_packet_handler_t callback, hci_con_handle_t con_handle);

/**
 * @brief Discovers a specific primary service given its UUID. This service may exist multiple times. For each found service, an le_service_event_t with type set to GATT_EVENT_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t, with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
 * @param callback          callback to discover primary services by UUID16
 * @param con_handle        connection handle to discover services on
 * @param uuid16            16-bit UUID of the primary service to discover
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_discover_primary_services_by_uuid16(user_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t uuid16);

/**
 * @brief Discovers a specific primary service given its UUID. This service may exist multiple times. For each found service, an le_service_event_t with type set to GATT_EVENT_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t, with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
 * @param callback          callback to discover primary services by UUID128
 * @param con_handle        connection handle to discover services on
 * @param uuid128           128-bit UUID of the primary service to discover
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_discover_primary_services_by_uuid128(user_packet_handler_t callback, hci_con_handle_t con_handle, const uint8_t * uuid128);


/**
 * @brief Finds included services within the specified service. For each found included service, an le_service_event_t with type set to GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery. Information about included service type (primary/secondary) can be retrieved either by sending an ATT find information request for the returned start group handle (returning the handle and the UUID for primary or secondary service) or by comparing the service to the list of all primary services.
 * @param callback           Callback to find included services.
 * @param con_handle         Connection handle to find included services on.
 * @param start_group_handle Start group handle of the service to find included services for.
 * @param end_group_handle   End group handle of the service to find included services for.
 * @return                   0 on success, or error code on failure.
 */
uint8_t gatt_client_find_included_services_for_service(user_packet_handler_t callback, hci_con_handle_t con_handle,
                const uint16_t start_group_handle, const uint16_t end_group_handle);


/**
 * @brief The following function are used to discover all characteristics within the specified service or handle range
 * @param callback           Callback to discover characteristics.
 * @param con_handle         Connection handle to discover characteristics on.
 * @param start_group_handle Start group handle of the service to discover characteristics for.
 * @param end_group_handle   End group handle of the service to discover characteristics for.
 * @return                   0 on success, or error code on failure.
 */
uint8_t gatt_client_discover_characteristics_for_service(user_packet_handler_t callback, hci_con_handle_t con_handle,
                const uint16_t start_group_handle, const uint16_t end_group_handle);

/**
 * @brief The following two functions are used to discover all characteristics within the specified service or handle range, and return those that match the given UUID. For each found characteristic, an le_characteristic_event_t with type set to GATT_EVENT_CHARACTERISTIC_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
 * @param callback          Callback to discover characteristics by UUID.
 * @param con_handle        Connection handle to discover characteristics on.
 * @param start_handle      Start handle of the range to discover characteristics for.
 * @param end_handle        End handle of the range to discover characteristics for.
 * @param uuid16            16-bit UUID of the characteristic to discover.
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid16(btstack_packet_handler_t callback,
                const hci_con_handle_t con_handle, const uint16_t start_handle, const uint16_t end_handle, const uint16_t uuid16);

/**
 * @brief The following two functions are used to discover all characteristics within the specified service or handle range, and return those that match the given UUID. For each found characteristic, an le_characteristic_event_t with type set to GATT_EVENT_CHARACTERISTIC_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
 * @param callback          Callback to discover characteristics by UUID.
 * @param con_handle        Connection handle to discover characteristics on.
 * @param start_handle      Start handle of the range to discover characteristics for.
 * @param end_handle        End handle of the range to discover characteristics for.
 * @param uuid128           128-bit UUID of the characteristic to discover.
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid128(btstack_packet_handler_t callback,
                const hci_con_handle_t con_handle, const uint16_t start_handle, const uint16_t end_handle,
                const uint8_t * uuid128);

/**
 * @brief Discovers attribute handle and UUID of a characteristic descriptor within the specified characteristic. For each found descriptor, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
 * @param callback          Callback to discover characteristic descriptors.
 * @param con_handle        Connection handle to discover characteristic descriptors on.
 * @param characteristic    Pointer to the characteristic for which to discover descriptors.
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_discover_characteristic_descriptors(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t  *characteristic);

/**
 * @brief Reads the characteristic value using the characteristic's value handle. If the characteristic value is found, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 * @callback                            Callback to read characteristic value.
 * @param con_handle                    Connection handle to read characteristic value on.
 * @param characteristic_value_handle   Handle of the characteristic value to read.
 * @return                              0 on success, or error code on failure.
 */
uint8_t gatt_client_read_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle);

/**
 * @brief Reads the characteristic value of all characteristics with the uuid. For each found, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 *
 * Note: To search through all attributes, the starting handle shall be set to 0x0001 and the ending handle shall be set to 0xFFFF.
 * @param callback          Callback to read characteristic value by UUID16.
 * @param con_handle        Connection handle to read characteristic value on.
 * @param start_handle      Start handle of the range to read characteristic value for.
 * @param end_handle        End handle of the range to read characteristic value for.
 * @param uuid16            16-bit UUID of the characteristic to read.
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_read_value_of_characteristics_by_uuid16(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);

/**
 * @brief Reads the characteristic value of all characteristics with the uuid. For each found, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 * Note: To search through all attributes, the starting handle shall be set to 0x0001 and the ending handle shall be set to 0xFFFF.
 * @param callback          Callback to read characteristic value by UUID128.
 * @param con_handle        Connection handle to read characteristic value on.
 * @param start_handle      Start handle of the range to read characteristic value for.
 * @param end_handle        End handle of the range to read characteristic value for.
 * @param uuid128           128-bit UUID of the characteristic to read.
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_read_value_of_characteristics_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint8_t * uuid128);

/**
 * @brief Reads the long characteristic value using the characteristic's value handle. The value will be returned in several blobs. For each blob, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT and updated value offset will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, mark the end of read.
 * @param callback                      Callback to read long characteristic value.
 * @param con_handle                    Connection handle to read long characteristic value on.
 * @param characteristic_value_handle   Handle of the characteristic value to read.
 * @return                              0 on success, or error code on failure.
 */
uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle);

/**
 * @brief Reads the long characteristic value using the characteristic's value handle with offset. The value will be returned in several blobs. For each blob, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT and updated value offset will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 * @param callback                      Callback to read long characteristic value with offset.
 * @param con_handle                    Connection handle to read long characteristic value on.
 * @param characteristic_value_handle   Handle of the characteristic value to read.
 * @param offset                        Offset from which to start reading the long characteristic value.
 * @return                              0 on success, or error code on failure.
 */
uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t offset);

/**
 * @brief Read multiple characteristic values
 * @param callback                      to read multiple characteristic values
 * @param con_handle                    connection handle to read multiple characteristic values on
 * @param num_value_handles             number of characteristic value handles to read
 * @param value_handles                 array of characteristic value handles to read
 * @return                              0 on success, or error code on failure.
 */
uint8_t gatt_client_read_multiple_characteristic_values(btstack_packet_handler_t callback, hci_con_handle_t con_handle, int num_value_handles, uint16_t * value_handles);

/**
 * @brief Writes the characteristic value using the characteristic's value handle without an acknowledgment that the write was successfully performed.
 * @param con_handle                    Connection handle to write characteristic value on.
 * @param characteristic_value_handle   Handle of the characteristic value to write.
 * @param length                        Length of the data to write.
 * @param data                          Pointer to the data to write.
 * @return                              0 on success, or error code on failure.
 */
uint8_t gatt_client_write_value_of_characteristic_without_response(hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, const uint8_t  * data);

/**
 * @brief Writes the authenticated characteristic value using the characteristic's value handle without an acknowledgment that the write was successfully performed.
 * @param callback          Callback to write authenticated characteristic value without response.
 * @param con_handle        Connection handle to write authenticated characteristic value on.
 * @param handle            Handle of the characteristic value to write.
 * @param message_len       Length of the data to write.
 * @param message           Pointer to the data to write.
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_signed_write_without_response(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t handle, uint16_t message_len, const uint8_t  * message);

/**
 * @brief Writes the characteristic value using the characteristic's value handle. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 * @param callback                      Callback to write characteristic value.
 * @param con_handle                    Connection handle to write characteristic value on.
 * @param characteristic_value_handle   Handle of the characteristic value to write.
 * @param length                        Length of the data to write.
 * @param data                          Pointer to the data to write.
 * @return                              0 on success, or error code on failure.
 */
uint8_t gatt_client_write_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, const uint8_t  * data);

/**
 * @brief Writes the characteristic value using the characteristic's value handle. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 * @note This function is used for writing long values that may exceed the maximum transmission unit (MTU) size.
 * @param callback                      Callback to write long characteristic value.
 * @param con_handle                    Connection handle to write long characteristic value on.
 * @param characteristic_value_handle   Handle of the characteristic value to write.
 * @param length                        Length of the data to write.
 * @param data                          Pointer to the data to write.
 * @return                              0 on success, or error code on failure.
 */
uint8_t gatt_client_write_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, const uint8_t  * data);

/**
 * @brief Writes the long characteristic value using the characteristic's value handle with offset. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 * @note This function is used for writing long values that may exceed the maximum transmission unit (MTU) size.
 * @param callback                      Callback to write long characteristic value with offset.
 * @param con_handle                    Connection handle to write long characteristic value on.
 * @param characteristic_value_handle   Handle of the characteristic value to write.
 * @param offset                        Offset from which to start writing the long characteristic value.
 * @param length                        Length of the data to write.
 * @param data                          Pointer to the data to write.
 * @return                              0 on success, or error code on failure.
 */
uint8_t gatt_client_write_long_value_of_characteristic_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t offset, uint16_t length, const uint8_t  * data);

/**
 * @brief Writes of the long characteristic value using the characteristic's value handle. It uses server response to validate that the write was correctly received. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 * @note This function is used for writing long values that may exceed the maximum transmission unit (MTU) size.
 * @param callback                      Callback to write long characteristic value with reliable write.
 * @param con_handle                    Connection handle to write long characteristic value on.
 * @param characteristic_value_handle   Handle of the characteristic value to write.
 * @param length                        Length of the data to write.
 * @param data                          Pointer to the data to write.
 * @return                              0 on success, or error code on failure.
 */
uint8_t gatt_client_reliable_write_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, const uint8_t  * data);

/**
 * @brief Reads the characteristic descriptor using its handle. If the characteristic descriptor is found, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 * @param callback          Callback to read characteristic descriptor.
 * @param con_handle        Connection handle to read characteristic descriptor on.
 * @param descriptor_handle Handle of the characteristic descriptor to read.
 */
uint8_t gatt_client_read_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle);

/**
 * @brief Reads the long characteristic descriptor using its handle. It will be returned in several blobs. For each blob, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 * @param callback          Callback to read long characteristic descriptor.
 * @param con_handle        Connection handle to read long characteristic descriptor on.
 * @param descriptor_handle Handle of the characteristic descriptor to read.
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle);

/**
 * @brief Reads the long characteristic descriptor using its handle with offset. It will be returned in several blobs. For each blob, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 * @param callback          Callback to read long characteristic descriptor with offset.
 * @param con_handle        Connection handle to read long characteristic descriptor on.
 * @param descriptor_handle Handle of the characteristic descriptor to read.
 * @param offset            Offset from which to start reading the long characteristic descriptor.
 */
uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t offset);

/**
 * @brief Writes the characteristic descriptor using its handle. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 * @param callback          Callback to write characteristic descriptor.
 * @param con_handle        Connection handle to write characteristic descriptor on.
 * @param descriptor_handle Handle of the characteristic descriptor to write.
 * @param length            Length of the data to write.
 * @param data              Pointer to the data to write.
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_write_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t length, uint8_t  * data);

/**
 * @brief Writes the long characteristic descriptor using its handle. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 * @param callback          Callback to write long characteristic descriptor.
 * @param con_handle        Connection handle to write long characteristic descriptor on.
 * @param descriptor_handle Handle of the characteristic descriptor to write.
 * @param length            Length of the data to write.
 * @param data              Pointer to the data to write.
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t length, uint8_t  * data);

/**
 * @brief Writes the long characteristic descriptor using its handle with offset. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 * @param callback          Callback to write long characteristic descriptor with offset.
 * @param con_handle        Connection handle to write long characteristic descriptor on.
 * @param descriptor_handle Handle of the characteristic descriptor to write.
 * @param offset            Offset from which to start writing the long characteristic descriptor.
 * @param length            Length of the data to write.
 * @param data              Pointer to the data to write.
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t offset, uint16_t length, uint8_t  * data);

/**
 * @brief Writes the client characteristic configuration of the specified characteristic. It is used to subscribe for notifications or indications of the characteristic value. For notifications or indications specify: GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION resp. GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION as configuration value.
 * @param callback          Callback to write client characteristic configuration.
 * @param con_handle        Connection handle to write client characteristic configuration on.
 * @param characteristic    Pointer to the characteristic for which to write client characteristic configuration.
 * @param configuration     Client characteristic configuration value to write. Use GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION or GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION to enable notifications or indications, respectively.
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_write_client_characteristic_configuration(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic, uint16_t configuration);


/**
 * @brief -> gatt complete event
 * @param callback          Callback to prepare write.
 * @param con_handle        Connection handle to prepare write on.
 * @param attribute_handle  Handle of the attribute to prepare write.
 * @param offset            Offset from which to start writing the attribute.
 * @param length            Length of the data to write.
 * @param data              Pointer to the data to write.
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_prepare_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint16_t length, uint8_t * data);

/**
 * @brief -> gatt complete event
 * @param callback          Callback to execute write.
 * @param con_handle        Connection handle to execute write on.
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_execute_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle);

/**
 * @brief -> gatt complete event
 * @param callback          Callback to cancel write.
 * @param con_handle        Connection handle to cancel write on.
 * @return                  0 on success, or error code on failure.
 */
uint8_t gatt_client_cancel_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle);

/** typedef struct gatt client notification */
typedef struct gatt_client_notification {
    btstack_linked_item_t    item;              // for linked list
    btstack_packet_handler_t callback;          // callback to receive notifications or indications
    uint16_t attribute_handle;                  // handle of the characteristic value to listen for notifications or indications
} gatt_client_notification_t;

/**
 * @brief MTU is available after the first query has completed. If status is equal to 0, it returns the real value, otherwise the default value of 23.
 * @param con_handle        connection handle
 * @param mtu               pointer to store the MTU size
 * @return                  0: successful, or error code on failure.
 */
uint8_t gatt_client_get_mtu(hci_con_handle_t con_handle, uint16_t * mtu);

/**
 * @brief Exchange MTU request with customized size
 *
 * Generally, this API is NOT needed, and MTU negotiations is performed automatically.
 * If developers DO need to control it, then,
 *
 * 1. set flag `STACK_GATT_CLIENT_DISABLE_MTU_EXCHANGE` by `btstack_config`;
 * 1. call this ONCE on a connection.
 *
 * @param con_handle        connection handle
 * @param mtu               requested MTU size (>= ATT_DEFAULT_MTU)
 * @return                  0: successful
 *                          Other error codes: BTSTACK_MEMORY_ALLOC_FAILED, GATT_CLIENT_IN_WRONG_STATE
 */
int gatt_client_exchange_mtu_request(hci_con_handle_t con_handle, uint16_t mtu);

/**
 * @brief Returns if the GATT client is ready to receive a query. It is used with daemon.
 * @param con_handle        connection handle
 * @return                  1 if ready, 0 if not ready
 */
int gatt_client_is_ready(hci_con_handle_t con_handle);

/**
 * @brief Register for notifications and indications of a characteristic enabled by gatt_client_write_client_characteristic_configuration
 * @param notification      struct used to store registration
 * @param packet_handler    callback to receive notifications or indications
 * @param con_handle        connection handle to register for notifications or indications
 * @param characteristic    characteristic to register for notifications or indications
 */
void gatt_client_listen_for_characteristic_value_updates(gatt_client_notification_t * notification, btstack_packet_handler_t packet_handler, hci_con_handle_t con_handle, uint16_t value_handle);

/**
 * @brief Register for general events
 * @param handler           btstack packet handler to receive GATT client events
 */
void gatt_client_register_handler(btstack_packet_handler_t handler);

/**
 * @brief Parse event GATT_EVENT_QUERY_COMPLETE
 * @param event packet
 * @return parsed result
 */
static __INLINE const gatt_event_query_complete_t * gatt_event_query_complete_parse(const uint8_t * event){
    return decode_event_offset(event, gatt_event_query_complete_t, 2);
}

/**
 * @brief Parse event GATT_EVENT_SERVICE_QUERY_RESULT
 * @param event packet
 * @return parsed result
 */
static __INLINE const gatt_event_service_query_result_t * gatt_event_service_query_result_parse(const uint8_t * event){
    return decode_event_offset(event, gatt_event_service_query_result_t, 2);
}

/**
 * @brief Parse event GATT_EVENT_CHARACTERISTIC_QUERY_RESULT
 * @param event packet
 * @return handle
 * @note: parsed result
 */
static __INLINE const gatt_event_characteristic_query_result_t * gatt_event_characteristic_query_result_parse(const uint8_t * event){
    return decode_event_offset(event, gatt_event_characteristic_query_result_t, 2);
}

/**
 * @brief Parse event GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT
 * @param event packet
 * @return handle
 * @note: parsed result
 */
static __INLINE const gatt_event_included_service_query_result_t * gatt_event_included_service_query_result_parse(const uint8_t * event){
    return decode_event_offset(event, gatt_event_included_service_query_result_t, 2);
}

/**
 * @brief Parse event GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT
 * @param event packet
 * @return handle
 * @note: parsed result
 */
static __INLINE const gatt_event_all_characteristic_descriptors_query_result_t * gatt_event_all_characteristic_descriptors_query_result_parse(const uint8_t * event){
    return decode_event_offset(event, gatt_event_all_characteristic_descriptors_query_result_t, 2);
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
static __INLINE const gatt_event_value_packet_t * gatt_event_characteristic_descriptor_value_query_result_parse(const uint8_t *event_packet, uint16_t event_size, uint16_t *value_size)
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
static __INLINE uint16_t gatt_event_mtu_get_mtu(const uint8_t * event) {
    return little_endian_read_16(event, 4);
}

/**
 * @brief Get field `event_type` from event GATT_EVENT_UNHANDLED_SERVER_VALUE
 * @param event packet
 * @return `type` (GATT_EVENT_NOTIFICATION or GATT_EVENT_INDICATION)
 */
static __INLINE uint8_t gatt_event_unhandled_server_value_get_type(const uint8_t * event) {
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field `value_handle` from event GATT_EVENT_UNHANDLED_SERVER_VALUE
 * @param event packet
 * @return `value_handle`
 */
static __INLINE uint16_t gatt_event_unhandled_server_value_get_value_handle(const uint8_t * event) {
    return little_endian_read_16(event, 3);
}

/**
 * @brief Get field `size` from event GATT_EVENT_UNHANDLED_SERVER_VALUE
 * @param event packet
 * @return size of data
 */
static __INLINE uint16_t gatt_event_unhandled_server_value_get_size(const uint8_t * event) {
    return little_endian_read_16(event, 5);
}

/**
 * @brief Get field `data` from event GATT_EVENT_UNHANDLED_SERVER_VALUE
 * @param event packet
 * @return pointer of data
 */
static __INLINE const uint8_t * gatt_event_unhandled_server_value_get_data(const uint8_t * event) {
    return (const uint8_t *)little_endian_read_32(event, 7);
}

/**
 * @}
*/
#ifdef __cplusplus
}
#endif

#endif
