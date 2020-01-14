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
// ----------------------------------------------------------------------------
// Design Information                                                          
// ----------------------------------------------------------------------------
//                                                                             
// File         : gatt_external.h
//                                                                             
// Author       : YiyunDeng
//                                                                             
// Organisation : INGCHIPS                                                     
//                                                                             
// Date         : 2019-03-01
//                                                                             
// Revision     : v0.1                                                         
//                                                                             
// Project      : ING91800                                                     
//                                                                             
// Description  :                                                              
//                                                                             
// ----------------------------------------------------------------------------
// Revision History                                                            
// ----------------------------------------------------------------------------
//                                                                             
// Date       Author          Revision  Change Description                     
// ========== =============== ========= =======================================
// 2019/03/01 Dengyiyun       v0.1       Creat
//                                                                             
// ----------------------------------------------------------------------------
 
#ifndef __GATT_EXTERNAL_H
#define __GATT_EXTERNAL_H

#include <stdint.h>
#include "bt_types.h"
#include "ble_uuid.h"
/* API_START */

typedef struct {
    uint16_t handle;
    uint16_t uuid16;
    uint8_t  uuid128[16];
} gatt_client_characteristic_descriptor_t;

// ATT Client Read Callback for Dynamic Data
// - if buffer == NULL, don't copy data, just return size of value
// - if buffer != NULL, copy data and return number bytes copied
// @param con_handle of hci le connection
// @param attribute_handle to be read
// @param offset defines start of attribute value
// @param buffer 
// @param buffer_size
typedef uint16_t (*att_read_callback_t)(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);

// ATT Client Write Callback for Dynamic Data
// @param con_handle of hci le connection
// @param attribute_handle to be written
// @param transaction - ATT_TRANSACTION_MODE_NONE for regular writes, ATT_TRANSACTION_MODE_ACTIVE for prepared writes and ATT_TRANSACTION_MODE_EXECUTE
// @param offset into the value - used for queued writes and long attributes
// @param buffer 
// @param buffer_size
// @param signature used for signed write commmands
// @returns 0 if write was ok, ATT_ERROR_PREPARE_QUEUE_FULL if no space in queue, ATT_ERROR_INVALID_OFFSET if offset is larger than max buffer
typedef int (*att_write_callback_t)(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);

/* API_START */
/*
 * @brief setup ATT server
 * @param db attribute database created by compile-gatt.ph
 * @param read_callback, see att_db.h, can be NULL
 * @param write_callback, see attl.h, can be NULL
 */
// void att_server_init(uint8_t const * db, att_read_callback_t read_callback, att_write_callback_t write_callback);
typedef void  (*f_att_server_init)(uint8_t const * db, att_read_callback_t read_callback, att_write_callback_t write_callback);
#define att_server_init ((f_att_server_init)(0x00007ef1))


/*
 * @brief register packet handler for ATT server events:
 *        - ATT_EVENT_MTU_EXCHANGE_COMPLETE 
 *        - ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE
 * @param handler
 */
// void att_server_register_packet_handler(btstack_packet_handler_t handler);
typedef void  (*f_att_server_register_packet_handler)(btstack_packet_handler_t handler);
#define att_server_register_packet_handler ((f_att_server_register_packet_handler)(0x00008031))


/*
 * @brief tests if a notification or indication can be send right now
 * @param con_handle
 * @return 1, if packet can be sent
 */
// int  att_server_can_send_packet_now(hci_con_handle_t con_handle);
typedef int   (*f_att_server_can_send_packet_now)(hci_con_handle_t con_handle);
#define att_server_can_send_packet_now ((f_att_server_can_send_packet_now)(0x00007e11))


/** 
 * @brief Request emission of ATT_EVENT_CAN_SEND_NOW as soon as possible
 * @note ATT_EVENT_CAN_SEND_NOW might be emitted during call to this function
 *       so packet handler should be ready to handle it
 * @param con_handle
 */
// void att_server_request_can_send_now_event(hci_con_handle_t con_handle);
typedef void  (*f_att_server_request_can_send_now_event)(hci_con_handle_t con_handle);
#define att_server_request_can_send_now_event ((f_att_server_request_can_send_now_event)(0x0000803d))


/*
 * @brief notify client about attribute value change
 * @param con_handle
 * @param attribute_handle
 * @param value
 * @param value_len
 * @return 0 if ok, error otherwise
 */
// int att_server_notify(hci_con_handle_t con_handle, uint16_t attribute_handle, uint8_t *value, uint16_t value_len);
typedef int  (*f_att_server_notify)(hci_con_handle_t con_handle, uint16_t attribute_handle, uint8_t *value, uint16_t value_len);
#define att_server_notify ((f_att_server_notify)(0x00007f41))


/*
 * @brief indicate value change to client. client is supposed to reply with an indication_response
 * @param con_handle
 * @param attribute_handle
 * @param value
 * @param value_len
 * @return 0 if ok, error otherwise
 */
// int att_server_indicate(hci_con_handle_t con_handle, uint16_t attribute_handle, uint8_t *value, uint16_t value_len);
typedef int  (*f_att_server_indicate)(hci_con_handle_t con_handle, uint16_t attribute_handle, uint8_t *value, uint16_t value_len);
#define att_server_indicate ((f_att_server_indicate)(0x00007e79))


/*
 * @brief emit a general event to att handler
 * @param event
 * @param length
 * @return void
 */
// void att_emit_general_event(uint8_t *event, uint16_t length);
typedef void  (*f_att_emit_general_event)(uint8_t *event, uint16_t length);
#define att_emit_general_event ((f_att_emit_general_event)(0x000075b5))


/** 
 * @brief Discovers all primary services. For each found service, an le_service_event_t with type set to GATT_EVENT_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t, with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery. 
 */
// uint8_t gatt_client_discover_primary_services(user_packet_handler_t callback, hci_con_handle_t con_handle);
typedef uint8_t  (*f_gatt_client_discover_primary_services)(user_packet_handler_t callback, hci_con_handle_t con_handle);
#define gatt_client_discover_primary_services ((f_gatt_client_discover_primary_services)(0x000138f9))


/** 
 * @brief Discovers a specific primary service given its UUID. This service may exist multiple times. For each found service, an le_service_event_t with type set to GATT_EVENT_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t, with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery. 
 */
// uint8_t gatt_client_discover_primary_services_by_uuid16(user_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t uuid16);
typedef uint8_t  (*f_gatt_client_discover_primary_services_by_uuid16)(user_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t uuid16);
#define gatt_client_discover_primary_services_by_uuid16 ((f_gatt_client_discover_primary_services_by_uuid16)(0x0001396f))

// uint8_t gatt_client_discover_primary_services_by_uuid128(user_packet_handler_t callback, hci_con_handle_t con_handle, const uint8_t * uuid128);
typedef uint8_t  (*f_gatt_client_discover_primary_services_by_uuid128)(user_packet_handler_t callback, hci_con_handle_t con_handle, const uint8_t * uuid128);
#define gatt_client_discover_primary_services_by_uuid128 ((f_gatt_client_discover_primary_services_by_uuid128)(0x0001392b))

// uint8_t gatt_client_discover_characteristics_for_service(user_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t *service);
typedef uint8_t  (*f_gatt_client_discover_characteristics_for_service)(user_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t *service);
#define gatt_client_discover_characteristics_for_service ((f_gatt_client_discover_characteristics_for_service)(0x000138a5))


/** 
 * @brief Finds included services within the specified service. For each found included service, an le_service_event_t with type set to GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery. Information about included service type (primary/secondary) can be retrieved either by sending an ATT find information request for the returned start group handle (returning the handle and the UUID for primary or secondary service) or by comparing the service to the list of all primary services. 
 */
// uint8_t gatt_client_find_included_services_for_service(user_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t *service);
typedef uint8_t  (*f_gatt_client_find_included_services_for_service)(user_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t *service);
#define gatt_client_find_included_services_for_service ((f_gatt_client_find_included_services_for_service)(0x000139d1))



/** 
 * @brief The following four functions are used to discover all characteristics within the specified service or handle range, and return those that match the given UUID. For each found characteristic, an le_characteristic_event_t with type set to GATT_EVENT_CHARACTERISTIC_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
 */
// uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid16(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
typedef uint8_t  (*f_gatt_client_discover_characteristics_for_handle_range_by_uuid16)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
#define gatt_client_discover_characteristics_for_handle_range_by_uuid16 ((f_gatt_client_discover_characteristics_for_handle_range_by_uuid16)(0x00013855))

// uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint8_t  * uuid);
typedef uint8_t  (*f_gatt_client_discover_characteristics_for_handle_range_by_uuid128)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint8_t  * uuid);
#define gatt_client_discover_characteristics_for_handle_range_by_uuid128 ((f_gatt_client_discover_characteristics_for_handle_range_by_uuid128)(0x00013805))

// uint8_t gatt_client_discover_characteristics_for_service_by_uuid16 (btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t  *service, uint16_t  uuid16);
typedef uint8_t  (*f_gatt_client_discover_characteristics_for_service_by_uuid16) (btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t  *service, uint16_t  uuid16);
#define gatt_client_discover_characteristics_for_service_by_uuid16 ((f_gatt_client_discover_characteristics_for_service_by_uuid16)(0x000138eb))

// uint8_t gatt_client_discover_characteristics_for_service_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t  *service, uint8_t  * uuid128);
typedef uint8_t  (*f_gatt_client_discover_characteristics_for_service_by_uuid128)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t  *service, uint8_t  * uuid128);
#define gatt_client_discover_characteristics_for_service_by_uuid128 ((f_gatt_client_discover_characteristics_for_service_by_uuid128)(0x000138dd))


/** 
 * @brief Discovers attribute handle and UUID of a characteristic descriptor within the specified characteristic. For each found descriptor, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
 */
// uint8_t gatt_client_discover_characteristic_descriptors(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t  *characteristic);
typedef uint8_t  (*f_gatt_client_discover_characteristic_descriptors)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t  *characteristic);
#define gatt_client_discover_characteristic_descriptors ((f_gatt_client_discover_characteristic_descriptors)(0x000137c3))


/** 
 * @brief Reads the characteristic value using the characteristic's value handle. If the characteristic value is found, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 */
// uint8_t gatt_client_read_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t  *characteristic);
typedef uint8_t  (*f_gatt_client_read_value_of_characteristic)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t  *characteristic);
#define gatt_client_read_value_of_characteristic ((f_gatt_client_read_value_of_characteristic)(0x00013bfd))

// uint8_t gatt_client_read_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle);
typedef uint8_t  (*f_gatt_client_read_value_of_characteristic_using_value_handle)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle);
#define gatt_client_read_value_of_characteristic_using_value_handle ((f_gatt_client_read_value_of_characteristic_using_value_handle)(0x00013c03))


/**
 * @brief Reads the characteric value of all characteristics with the uuid. For each found, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 */
// uint8_t gatt_client_read_value_of_characteristics_by_uuid16(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
typedef uint8_t  (*f_gatt_client_read_value_of_characteristics_by_uuid16)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
#define gatt_client_read_value_of_characteristics_by_uuid16 ((f_gatt_client_read_value_of_characteristics_by_uuid16)(0x00013c7d))

// uint8_t gatt_client_read_value_of_characteristics_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint8_t * uuid128);
typedef uint8_t  (*f_gatt_client_read_value_of_characteristics_by_uuid128)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint8_t * uuid128);
#define gatt_client_read_value_of_characteristics_by_uuid128 ((f_gatt_client_read_value_of_characteristics_by_uuid128)(0x00013c31))


/** 
 * @brief Reads the long characteristic value using the characteristic's value handle. The value will be returned in several blobs. For each blob, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT and updated value offset will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, mark the end of read.
 */
// uint8_t gatt_client_read_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t  *characteristic);
typedef uint8_t  (*f_gatt_client_read_long_value_of_characteristic)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t  *characteristic);
#define gatt_client_read_long_value_of_characteristic ((f_gatt_client_read_long_value_of_characteristic)(0x00013b91))

// uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle);
typedef uint8_t  (*f_gatt_client_read_long_value_of_characteristic_using_value_handle)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle);
#define gatt_client_read_long_value_of_characteristic_using_value_handle ((f_gatt_client_read_long_value_of_characteristic_using_value_handle)(0x00013b99))

// uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t offset);
typedef uint8_t  (*f_gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t offset);
#define gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset ((f_gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset)(0x00013b9f))


/*
 * @brief Read multiple characteristic values
 * @param number handles
 * @param list_of_handles list of handles 
 */
// uint8_t gatt_client_read_multiple_characteristic_values(btstack_packet_handler_t callback, hci_con_handle_t con_handle, int num_value_handles, uint16_t * value_handles);
typedef uint8_t  (*f_gatt_client_read_multiple_characteristic_values)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, int num_value_handles, uint16_t * value_handles);
#define gatt_client_read_multiple_characteristic_values ((f_gatt_client_read_multiple_characteristic_values)(0x00013bcd))


/** 
 * @brief Writes the characteristic value using the characteristic's value handle without an acknowledgment that the write was successfully performed.
 */
// uint8_t gatt_client_write_value_of_characteristic_without_response(hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);
typedef uint8_t  (*f_gatt_client_write_value_of_characteristic_without_response)(hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);
#define gatt_client_write_value_of_characteristic_without_response ((f_gatt_client_write_value_of_characteristic_without_response)(0x000143a9))


/** 
 * @brief Writes the authenticated characteristic value using the characteristic's value handle without an acknowledgment that the write was successfully performed.
 */
// uint8_t gatt_client_signed_write_without_response(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t handle, uint16_t message_len, uint8_t  * message);
typedef uint8_t  (*f_gatt_client_signed_write_without_response)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t handle, uint16_t message_len, uint8_t  * message);
#define gatt_client_signed_write_without_response ((f_gatt_client_signed_write_without_response)(0x00014145))


/** 
 * @brief Writes the characteristic value using the characteristic's value handle. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 */
// uint8_t gatt_client_write_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);
typedef uint8_t  (*f_gatt_client_write_value_of_characteristic)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);
#define gatt_client_write_value_of_characteristic ((f_gatt_client_write_value_of_characteristic)(0x00014373))

// uint8_t gatt_client_write_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);
typedef uint8_t  (*f_gatt_client_write_long_value_of_characteristic)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);
#define gatt_client_write_long_value_of_characteristic ((f_gatt_client_write_long_value_of_characteristic)(0x00014327))

// uint8_t gatt_client_write_long_value_of_characteristic_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t offset, uint16_t length, uint8_t  * data);
typedef uint8_t  (*f_gatt_client_write_long_value_of_characteristic_with_offset)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t offset, uint16_t length, uint8_t  * data);
#define gatt_client_write_long_value_of_characteristic_with_offset ((f_gatt_client_write_long_value_of_characteristic_with_offset)(0x00014337))


/** 
 * @brief Writes of the long characteristic value using the characteristic's value handle. It uses server response to validate that the write was correctly received. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 */
// uint8_t gatt_client_reliable_write_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);
typedef uint8_t  (*f_gatt_client_reliable_write_long_value_of_characteristic)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t characteristic_value_handle, uint16_t length, uint8_t  * data);
#define gatt_client_reliable_write_long_value_of_characteristic ((f_gatt_client_reliable_write_long_value_of_characteristic)(0x00013cc7))


/** 
 * @brief Reads the characteristic descriptor using its handle. If the characteristic descriptor is found, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 */
// uint8_t gatt_client_read_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor);
typedef uint8_t  (*f_gatt_client_read_characteristic_descriptor)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor);
#define gatt_client_read_characteristic_descriptor ((f_gatt_client_read_characteristic_descriptor)(0x00013b25))

// uint8_t gatt_client_read_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle);
typedef uint8_t  (*f_gatt_client_read_characteristic_descriptor_using_descriptor_handle)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle);
#define gatt_client_read_characteristic_descriptor_using_descriptor_handle ((f_gatt_client_read_characteristic_descriptor_using_descriptor_handle)(0x00013b2b))


/** 
 * @brief Reads the long characteristic descriptor using its handle. It will be returned in several blobs. For each blob, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
 */
// uint8_t gatt_client_read_long_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor);
typedef uint8_t  (*f_gatt_client_read_long_characteristic_descriptor)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor);
#define gatt_client_read_long_characteristic_descriptor ((f_gatt_client_read_long_characteristic_descriptor)(0x00013b55))

// uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle);
typedef uint8_t  (*f_gatt_client_read_long_characteristic_descriptor_using_descriptor_handle)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle);
#define gatt_client_read_long_characteristic_descriptor_using_descriptor_handle ((f_gatt_client_read_long_characteristic_descriptor_using_descriptor_handle)(0x00013b5d))

// uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t offset);
typedef uint8_t  (*f_gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t offset);
#define gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset ((f_gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset)(0x00013b63))


/** 
 * @brief Writes the characteristic descriptor using its handle. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of write. The write is successfully performed, if the event's status field is set to 0.
 */
// uint8_t gatt_client_write_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor, uint16_t length, uint8_t  * data);
typedef uint8_t  (*f_gatt_client_write_characteristic_descriptor)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor, uint16_t length, uint8_t  * data);
#define gatt_client_write_characteristic_descriptor ((f_gatt_client_write_characteristic_descriptor)(0x00014221))

// uint8_t gatt_client_write_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t length, uint8_t  * data);
typedef uint8_t  (*f_gatt_client_write_characteristic_descriptor_using_descriptor_handle)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t length, uint8_t  * data);
#define gatt_client_write_characteristic_descriptor_using_descriptor_handle ((f_gatt_client_write_characteristic_descriptor_using_descriptor_handle)(0x0001422f))

// uint8_t gatt_client_write_long_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor, uint16_t length, uint8_t  * data);
typedef uint8_t  (*f_gatt_client_write_long_characteristic_descriptor)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t  * descriptor, uint16_t length, uint8_t  * data);
#define gatt_client_write_long_characteristic_descriptor ((f_gatt_client_write_long_characteristic_descriptor)(0x000142c9))

// uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t length, uint8_t  * data);
typedef uint8_t  (*f_gatt_client_write_long_characteristic_descriptor_using_descriptor_handle)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t length, uint8_t  * data);
#define gatt_client_write_long_characteristic_descriptor_using_descriptor_handle ((f_gatt_client_write_long_characteristic_descriptor_using_descriptor_handle)(0x000142db))

// uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t offset, uint16_t length, uint8_t  * data);
typedef uint8_t  (*f_gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t offset, uint16_t length, uint8_t  * data);
#define gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset ((f_gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset)(0x000142eb))


/** 
 * @brief Writes the client characteristic configuration of the specified characteristic. It is used to subscribe for notifications or indications of the characteristic value. For notifications or indications specify: GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION resp. GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION as configuration value.
 */
// uint8_t gatt_client_write_client_characteristic_configuration(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic, uint16_t configuration);
typedef uint8_t  (*f_gatt_client_write_client_characteristic_configuration)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic, uint16_t configuration);
#define gatt_client_write_client_characteristic_configuration ((f_gatt_client_write_client_characteristic_configuration)(0x00014269))



/**
 * @brief -> gatt complete event
 */
// uint8_t gatt_client_prepare_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint16_t length, uint8_t * data);
typedef uint8_t  (*f_gatt_client_prepare_write)(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint16_t length, uint8_t * data);
#define gatt_client_prepare_write ((f_gatt_client_prepare_write)(0x00013add))


/**
 * @brief -> gatt complete event
 */
// uint8_t gatt_client_execute_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
typedef uint8_t  (*f_gatt_client_execute_write)(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
#define gatt_client_execute_write ((f_gatt_client_execute_write)(0x000139ab))


/**
 * @brief -> gatt complete event
 */
// uint8_t gatt_client_cancel_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
typedef uint8_t  (*f_gatt_client_cancel_write)(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
#define gatt_client_cancel_write ((f_gatt_client_cancel_write)(0x000136f9))


// int ble_gatts_find_svc(const ble_uuid_t *uuid, uint16_t *out_handle);
typedef int  (*f_ble_gatts_find_svc)(const ble_uuid_t *uuid, uint16_t *out_handle);
#define ble_gatts_find_svc ((f_ble_gatts_find_svc)(0x00008a41))

// int ble_gatts_find_chr(uint16_t srv_handle, const ble_uuid_t *uuid,uint16_t *out_att_chr);
typedef int  (*f_ble_gatts_find_chr)(uint16_t srv_handle, const ble_uuid_t *uuid,uint16_t *out_att_chr);
#define ble_gatts_find_chr ((f_ble_gatts_find_chr)(0x00008999))

// uint16_t ble_att_mtu(uint16_t conn_handle);
typedef uint16_t  (*f_ble_att_mtu)(uint16_t conn_handle);
#define ble_att_mtu ((f_ble_att_mtu)(0x0000888d))

#endif
