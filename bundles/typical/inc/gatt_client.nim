##  ----------------------------------------------------------------------------
##  Copyright Message
##  ----------------------------------------------------------------------------
##
##  INGCHIPS confidential and proprietary.
##  COPYRIGHT (c) 2018 by INGCHIPS
##
##  All rights are reserved. Reproduction in whole or in part is
##  prohibited without the written consent of the copyright owner.
##
##
##  ----------------------------------------------------------------------------

import
  bluetooth, bt_types, btstack_util, btstack_event, btstack_linked_list

type
  gatt_client_service_t* {.importc: "gatt_client_service_t",
                          header: "gatt_client.h", bycopy.} = object
    start_group_handle* {.importc: "start_group_handle".}: uint16
    end_group_handle* {.importc: "end_group_handle".}: uint16
    uuid16* {.importc: "uuid16".}: uint16
    uuid128* {.importc: "uuid128".}: array[16, uint8]

  gatt_client_characteristic_t* {.importc: "gatt_client_characteristic_t",
                                 header: "gatt_client.h", bycopy.} = object
    start_handle* {.importc: "start_handle".}: uint16
    value_handle* {.importc: "value_handle".}: uint16
    end_handle* {.importc: "end_handle".}: uint16
    properties* {.importc: "properties".}: uint16
    uuid16* {.importc: "uuid16".}: uint16
    uuid128* {.importc: "uuid128".}: array[16, uint8]

  gatt_client_characteristic_descriptor_t* {.
      importc: "gatt_client_characteristic_descriptor_t", header: "gatt_client.h",
      bycopy.} = object
    handle* {.importc: "handle".}: uint16
    uuid16* {.importc: "uuid16".}: uint16
    uuid128* {.importc: "uuid128".}: array[16, uint8]


## *
##  @brief Discovers all primary services. For each found service, an le_service_event_t with type set to GATT_EVENT_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t, with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
##

proc gatt_client_discover_primary_services*(callback: user_packet_handler_t;
    con_handle: hci_con_handle_t): uint8 {.importc: "gatt_client_discover_primary_services",
                                        header: "gatt_client.h".}
## *
##  @brief Discovers a specific primary service given its UUID. This service may exist multiple times. For each found service, an le_service_event_t with type set to GATT_EVENT_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t, with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
##

proc gatt_client_discover_primary_services_by_uuid16*(
    callback: user_packet_handler_t; con_handle: hci_con_handle_t; uuid16: uint16): uint8 {.
    importc: "gatt_client_discover_primary_services_by_uuid16",
    header: "gatt_client.h".}
proc gatt_client_discover_primary_services_by_uuid128*(
    callback: user_packet_handler_t; con_handle: hci_con_handle_t;
    uuid128: ptr uint8): uint8 {.importc: "gatt_client_discover_primary_services_by_uuid128",
                             header: "gatt_client.h".}
proc gatt_client_discover_characteristics_for_service*(
    callback: user_packet_handler_t; con_handle: hci_con_handle_t;
    service: ptr gatt_client_service_t): uint8 {.
    importc: "gatt_client_discover_characteristics_for_service",
    header: "gatt_client.h".}
## *
##  @brief Finds included services within the specified service. For each found included service, an le_service_event_t with type set to GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery. Information about included service type (primary/secondary) can be retrieved either by sending an ATT find information request for the returned start group handle (returning the handle and the UUID for primary or secondary service) or by comparing the service to the list of all primary services.
##

proc gatt_client_find_included_services_for_service*(
    callback: user_packet_handler_t; con_handle: hci_con_handle_t;
    service: ptr gatt_client_service_t): uint8 {.
    importc: "gatt_client_find_included_services_for_service",
    header: "gatt_client.h".}
## *
##  @brief The following four functions are used to discover all characteristics within the specified service or handle range, and return those that match the given UUID. For each found characteristic, an le_characteristic_event_t with type set to GATT_EVENT_CHARACTERISTIC_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
##

proc gatt_client_discover_characteristics_for_handle_range_by_uuid16*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    start_handle: uint16; end_handle: uint16; uuid16: uint16): uint8 {.
    importc: "gatt_client_discover_characteristics_for_handle_range_by_uuid16",
    header: "gatt_client.h".}
proc gatt_client_discover_characteristics_for_handle_range_by_uuid128*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    start_handle: uint16; end_handle: uint16; uuid128: ptr uint8): uint8 {.importc: "gatt_client_discover_characteristics_for_handle_range_by_uuid128",
    header: "gatt_client.h".}
proc gatt_client_discover_characteristics_for_service_by_uuid16*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    service: ptr gatt_client_service_t; uuid16: uint16): uint8 {.
    importc: "gatt_client_discover_characteristics_for_service_by_uuid16",
    header: "gatt_client.h".}
proc gatt_client_discover_characteristics_for_service_by_uuid128*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    service: ptr gatt_client_service_t; uuid128: ptr uint8): uint8 {.
    importc: "gatt_client_discover_characteristics_for_service_by_uuid128",
    header: "gatt_client.h".}
## *
##  @brief Discovers attribute handle and UUID of a characteristic descriptor within the specified characteristic. For each found descriptor, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of discovery.
##

proc gatt_client_discover_characteristic_descriptors*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    characteristic: ptr gatt_client_characteristic_t): uint8 {.
    importc: "gatt_client_discover_characteristic_descriptors",
    header: "gatt_client.h".}
## *
##  @brief Reads the characteristic value using the characteristic's value handle. If the characteristic value is found, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
##

proc gatt_client_read_value_of_characteristic*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    characteristic: ptr gatt_client_characteristic_t): uint8 {.
    importc: "gatt_client_read_value_of_characteristic", header: "gatt_client.h".}
proc gatt_client_read_value_of_characteristic_using_value_handle*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    characteristic_value_handle: uint16): uint8 {.
    importc: "gatt_client_read_value_of_characteristic_using_value_handle",
    header: "gatt_client.h".}
## *
##  @brief Reads the characteric value of all characteristics with the uuid. For each found, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
##

proc gatt_client_read_value_of_characteristics_by_uuid16*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    start_handle: uint16; end_handle: uint16; uuid16: uint16): uint8 {.
    importc: "gatt_client_read_value_of_characteristics_by_uuid16",
    header: "gatt_client.h".}
proc gatt_client_read_value_of_characteristics_by_uuid128*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    start_handle: uint16; end_handle: uint16; uuid128: ptr uint8): uint8 {.
    importc: "gatt_client_read_value_of_characteristics_by_uuid128",
    header: "gatt_client.h".}
## *
##  @brief Reads the long characteristic value using the characteristic's value handle. The value will be returned in several blobs. For each blob, an le_characteristic_value_event_t with type set to GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT and updated value offset will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, mark the end of read.
##

proc gatt_client_read_long_value_of_characteristic*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    characteristic: ptr gatt_client_characteristic_t): uint8 {.
    importc: "gatt_client_read_long_value_of_characteristic",
    header: "gatt_client.h".}
proc gatt_client_read_long_value_of_characteristic_using_value_handle*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    characteristic_value_handle: uint16): uint8 {.importc: "gatt_client_read_long_value_of_characteristic_using_value_handle",
    header: "gatt_client.h".}
proc gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    characteristic_value_handle: uint16; offset: uint16): uint8 {.importc: "gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset",
    header: "gatt_client.h".}
##
##  @brief Read multiple characteristic values
##  @param number handles
##  @param list_of_handles list of handles
##

proc gatt_client_read_multiple_characteristic_values*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    num_value_handles: cint; value_handles: ptr uint16): uint8 {.
    importc: "gatt_client_read_multiple_characteristic_values",
    header: "gatt_client.h".}
## *
##  @brief Writes the characteristic value using the characteristic's value handle without an acknowledgment that the write was successfully performed.
##

proc gatt_client_write_value_of_characteristic_without_response*(
    con_handle: hci_con_handle_t; characteristic_value_handle: uint16;
    length: uint16; data: ptr uint8): uint8 {.importc: "gatt_client_write_value_of_characteristic_without_response",
                                        header: "gatt_client.h".}
## *
##  @brief Writes the authenticated characteristic value using the characteristic's value handle without an acknowledgment that the write was successfully performed.
##

proc gatt_client_signed_write_without_response*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    handle: uint16; message_len: uint16; message: ptr uint8): uint8 {.
    importc: "gatt_client_signed_write_without_response", header: "gatt_client.h".}
## *
##  @brief Writes the characteristic value using the characteristic's value handle. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of write. The write is successfully performed, if the event's status field is set to 0.
##

proc gatt_client_write_value_of_characteristic*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    characteristic_value_handle: uint16; length: uint16; data: ptr uint8): uint8 {.
    importc: "gatt_client_write_value_of_characteristic", header: "gatt_client.h".}
proc gatt_client_write_long_value_of_characteristic*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    characteristic_value_handle: uint16; length: uint16; data: ptr uint8): uint8 {.
    importc: "gatt_client_write_long_value_of_characteristic",
    header: "gatt_client.h".}
proc gatt_client_write_long_value_of_characteristic_with_offset*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    characteristic_value_handle: uint16; offset: uint16; length: uint16;
    data: ptr uint8): uint8 {.importc: "gatt_client_write_long_value_of_characteristic_with_offset",
                          header: "gatt_client.h".}
## *
##  @brief Writes of the long characteristic value using the characteristic's value handle. It uses server response to validate that the write was correctly received. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE marks the end of write. The write is successfully performed, if the event's status field is set to 0.
##

proc gatt_client_reliable_write_long_value_of_characteristic*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    characteristic_value_handle: uint16; length: uint16; data: ptr uint8): uint8 {.
    importc: "gatt_client_reliable_write_long_value_of_characteristic",
    header: "gatt_client.h".}
## *
##  @brief Reads the characteristic descriptor using its handle. If the characteristic descriptor is found, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
##

proc gatt_client_read_characteristic_descriptor*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    descriptor: ptr gatt_client_characteristic_descriptor_t): uint8 {.
    importc: "gatt_client_read_characteristic_descriptor", header: "gatt_client.h".}
proc gatt_client_read_characteristic_descriptor_using_descriptor_handle*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    descriptor_handle: uint16): uint8 {.importc: "gatt_client_read_characteristic_descriptor_using_descriptor_handle",
                                     header: "gatt_client.h".}
## *
##  @brief Reads the long characteristic descriptor using its handle. It will be returned in several blobs. For each blob, an le_characteristic_descriptor_event_t with type set to GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of read.
##

proc gatt_client_read_long_characteristic_descriptor*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    descriptor: ptr gatt_client_characteristic_descriptor_t): uint8 {.
    importc: "gatt_client_read_long_characteristic_descriptor",
    header: "gatt_client.h".}
proc gatt_client_read_long_characteristic_descriptor_using_descriptor_handle*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    descriptor_handle: uint16): uint8 {.importc: "gatt_client_read_long_characteristic_descriptor_using_descriptor_handle",
                                     header: "gatt_client.h".}
proc gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    descriptor_handle: uint16; offset: uint16): uint8 {.importc: "gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset",
    header: "gatt_client.h".}
## *
##  @brief Writes the characteristic descriptor using its handle. The gatt_complete_event_t with type set to GATT_EVENT_QUERY_COMPLETE, marks the end of write. The write is successfully performed, if the event's status field is set to 0.
##

proc gatt_client_write_characteristic_descriptor*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    descriptor: ptr gatt_client_characteristic_descriptor_t; length: uint16;
    data: ptr uint8): uint8 {.importc: "gatt_client_write_characteristic_descriptor",
                          header: "gatt_client.h".}
proc gatt_client_write_characteristic_descriptor_using_descriptor_handle*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    descriptor_handle: uint16; length: uint16; data: ptr uint8): uint8 {.importc: "gatt_client_write_characteristic_descriptor_using_descriptor_handle",
    header: "gatt_client.h".}
proc gatt_client_write_long_characteristic_descriptor*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    descriptor: ptr gatt_client_characteristic_descriptor_t; length: uint16;
    data: ptr uint8): uint8 {.importc: "gatt_client_write_long_characteristic_descriptor",
                          header: "gatt_client.h".}
proc gatt_client_write_long_characteristic_descriptor_using_descriptor_handle*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    descriptor_handle: uint16; length: uint16; data: ptr uint8): uint8 {.importc: "gatt_client_write_long_characteristic_descriptor_using_descriptor_handle",
    header: "gatt_client.h".}
proc gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    descriptor_handle: uint16; offset: uint16; length: uint16; data: ptr uint8): uint8 {.importc: "gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset",
    header: "gatt_client.h".}
## *
##  @brief Writes the client characteristic configuration of the specified characteristic. It is used to subscribe for notifications or indications of the characteristic value. For notifications or indications specify: GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION resp. GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION as configuration value.
##

proc gatt_client_write_client_characteristic_configuration*(
    callback: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    characteristic: ptr gatt_client_characteristic_t; configuration: uint16): uint8 {.
    importc: "gatt_client_write_client_characteristic_configuration",
    header: "gatt_client.h".}
## *
##  @brief -> gatt complete event
##

proc gatt_client_prepare_write*(callback: btstack_packet_handler_t;
                               con_handle: hci_con_handle_t;
                               attribute_handle: uint16; offset: uint16;
                               length: uint16; data: ptr uint8): uint8 {.
    importc: "gatt_client_prepare_write", header: "gatt_client.h".}
## *
##  @brief -> gatt complete event
##

proc gatt_client_execute_write*(callback: btstack_packet_handler_t;
                               con_handle: hci_con_handle_t): uint8 {.
    importc: "gatt_client_execute_write", header: "gatt_client.h".}
## *
##  @brief -> gatt complete event
##

proc gatt_client_cancel_write*(callback: btstack_packet_handler_t;
                              con_handle: hci_con_handle_t): uint8 {.
    importc: "gatt_client_cancel_write", header: "gatt_client.h".}
proc gatt_client_deserialize_service*(packet: ptr uint8; offset: cint;
                                     service: ptr gatt_client_service_t) {.
    importc: "gatt_client_deserialize_service", header: "gatt_client.h".}
proc gatt_client_deserialize_characteristic*(packet: ptr uint8; offset: cint;
    characteristic: ptr gatt_client_characteristic_t) {.
    importc: "gatt_client_deserialize_characteristic", header: "gatt_client.h".}
proc gatt_client_deserialize_characteristic_descriptor*(packet: ptr uint8;
    offset: cint; descriptor: ptr gatt_client_characteristic_descriptor_t) {.
    importc: "gatt_client_deserialize_characteristic_descriptor",
    header: "gatt_client.h".}
proc ble_gatts_find_svc*(uuid: ptr ble_uuid_t; out_handle: ptr uint16): cint {.
    importc: "ble_gatts_find_svc", header: "gatt_client.h".}
proc ble_gatts_find_chr*(srv_handle: uint16; uuid: ptr ble_uuid_t;
                        out_att_chr: ptr uint16): cint {.
    importc: "ble_gatts_find_chr", header: "gatt_client.h".}
type
  gatt_client_notification_t* {.importc: "gatt_client_notification_t",
                               header: "gatt_client.h", bycopy.} = object
    item* {.importc: "item".}: btstack_linked_item_t
    callback* {.importc: "callback".}: btstack_packet_handler_t
    attribute_handle* {.importc: "attribute_handle".}: uint16


## *
##  @brief MTU is available after the first query has completed. If status is equal to 0, it returns the real value, otherwise the default value of 23.
##

proc gatt_client_get_mtu*(con_handle: hci_con_handle_t; mtu: ptr uint16): uint8 {.
    importc: "gatt_client_get_mtu", header: "gatt_client.h".}
## *
##  @brief Returns if the GATT client is ready to receive a query. It is used with daemon.
##

proc gatt_client_is_ready*(con_handle: hci_con_handle_t): cint {.
    importc: "gatt_client_is_ready", header: "gatt_client.h".}
## *
##  @brief Register for notifications and indications of a characteristic enabled by gatt_client_write_client_characteristic_configuration
##  @param notification struct used to store registration
##  @param packet_handler
##  @param con_handle
##  @param characteristic
##

proc gatt_client_listen_for_characteristic_value_updates*(
    notification: ptr gatt_client_notification_t;
    packet_handler: btstack_packet_handler_t; con_handle: hci_con_handle_t;
    characteristic: ptr gatt_client_characteristic_t) {.
    importc: "gatt_client_listen_for_characteristic_value_updates",
    header: "gatt_client.h".}
## *
##  @brief Register for general events
##  @param handler
##

proc gatt_client_register_handler*(handler: btstack_packet_handler_t) {.
    importc: "gatt_client_register_handler", header: "gatt_client.h".}
##  only used for testing

proc gatt_client_pts_suppress_mtu_exchange*() {.
    importc: "gatt_client_pts_suppress_mtu_exchange", header: "gatt_client.h".}
type
  gatt_event_value_packet_t* {.importc: "gatt_event_value_packet_t",
                              header: "gatt_client.h", bycopy.} = object
    handle* {.importc: "handle".}: uint16
    value* {.importc: "value".}: UncheckedArray[uint8] ##  length is given in `value_size`

  gatt_event_long_value_packet_t* {.importc: "gatt_event_long_value_packet_t",
                                   header: "gatt_client.h", bycopy.} = object
    handle* {.importc: "handle".}: uint16
    offset* {.importc: "offset".}: uint16
    value* {.importc: "value".}: UncheckedArray[uint8] ##  length is given in `value_size`


## *
##  @brief Get field handle from event GATT_EVENT_QUERY_COMPLETE
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc gatt_event_query_complete_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field status from event GATT_EVENT_QUERY_COMPLETE
##  @param event packet
##  @return status
##  @note: btstack_type 1
##

proc gatt_event_query_complete_get_status*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 4)[]

## *
##  @brief Get field handle from event GATT_EVENT_SERVICE_QUERY_RESULT
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc gatt_event_service_query_result_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field service from event GATT_EVENT_SERVICE_QUERY_RESULT
##  @param event packet
##  @param Pointer to storage for service
##  @note: btstack_type X
##

proc gatt_event_service_query_result_get_service*(event: ptr uint8;
    service: ptr gatt_client_service_t) =
  gatt_client_deserialize_service(event, 4, service)

## *
##  @brief Get field handle from event GATT_EVENT_CHARACTERISTIC_QUERY_RESULT
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc gatt_event_characteristic_query_result_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field characteristic from event GATT_EVENT_CHARACTERISTIC_QUERY_RESULT
##  @param event packet
##  @param Pointer to storage for characteristic
##  @note: btstack_type Y
##

proc gatt_event_characteristic_query_result_get_characteristic*(event: ptr uint8;
    characteristic: ptr gatt_client_characteristic_t) =
  gatt_client_deserialize_characteristic(event, 4, characteristic)

## *
##  @brief Get field handle from event GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc gatt_event_included_service_query_result_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field include_handle from event GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT
##  @param event packet
##  @return include_handle
##  @note: btstack_type 2
##

proc gatt_event_included_service_query_result_get_include_handle*(
    event: ptr uint8): uint16 =
  return little_endian_read_16(event, 4)

## *
##  @brief Get field service from event GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT
##  @param event packet
##  @param Pointer to storage for service
##  @note: btstack_type X
##

proc gatt_event_included_service_query_result_get_service*(event: ptr uint8;
    service: ptr gatt_client_service_t) =
  gatt_client_deserialize_service(event, 6, service)

## *
##  @brief Get field handle from event GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc gatt_event_all_characteristic_descriptors_query_result_get_handle*(
    event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field characteristic_descriptor from event GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT
##  @param event packet
##  @param Pointer to storage for characteristic_descriptor
##  @note: btstack_type Z
##

proc gatt_event_all_characteristic_descriptors_query_result_get_characteristic_descriptor*(
    event: ptr uint8;
    characteristic_descriptor: ptr gatt_client_characteristic_descriptor_t) =
  gatt_client_deserialize_characteristic_descriptor(event, 4,
      characteristic_descriptor)

## *
##  @brief Get value from event GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT
##  @param[in]           event packet
##  @param[in]           event_size
##  @param[out]          value_size
##  @return gatt_event_value_packet_t *
##

proc gatt_event_long_characteristic_value_query_result_parse*(
    event_packet: ptr uint8; event_size: uint16; value_size: ptr uint16): ptr gatt_event_long_value_packet_t =
  value_size[] = event_size - 1 -
      cast[uint16](sizeof((gatt_event_long_value_packet_t)))
  return decode_event_offset(event_packet, gatt_event_long_value_packet_t, 1)

## *
##  @brief Get value from event GATT_EVENT_NOTIFICATION
##  @param[in]           event packet
##  @param[in]           event_size
##  @param[out]          value_size
##  @return gatt_event_value_packet_t *
##

proc gatt_event_notification_parse*(event_packet: ptr uint8; event_size: uint16;
                                   value_size: ptr uint16): ptr gatt_event_value_packet_t =
  value_size[] = event_size - 1 - cast[uint16](sizeof((gatt_event_value_packet_t)))
  return decode_event_offset(event_packet, gatt_event_value_packet_t, 1)

## *
##  @brief Get value from event GATT_EVENT_INDICATION
##  @param[in]           event packet
##  @param[in]           event_size
##  @param[out]          value_size
##  @return gatt_event_notification_t *
##

proc gatt_event_indication_parse*(event_packet: ptr uint8; event_size: uint16;
                                 value_size: ptr uint16): ptr gatt_event_value_packet_t =
  value_size[] = event_size - 1 - cast[uint16](sizeof((gatt_event_value_packet_t)))
  return decode_event_offset(event_packet, gatt_event_value_packet_t, 1)

## *
##  @brief Get value from event GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT
##  @param[in]           event packet
##  @param[in]           event_size
##  @param[out]          value_size
##  @return gatt_event_notification_t *
##

proc gatt_event_characteristic_value_query_result_parse*(event_packet: ptr uint8;
    event_size: uint16; value_size: ptr uint16): ptr gatt_event_value_packet_t =
  value_size[] = event_size - 1 - cast[uint16](sizeof((gatt_event_value_packet_t)))
  return decode_event_offset(event_packet, gatt_event_value_packet_t, 1)

## *
##  @brief Get value from event GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT
##  @param[in]           event packet
##  @param[in]           event_size
##  @param[out]          value_size
##  @return gatt_event_notification_t *
##

proc gatt_event_characteristic_descriptor_query_result_parse*(
    event_packet: ptr uint8; event_size: uint16; value_size: ptr uint16): ptr gatt_event_value_packet_t =
  value_size[] = event_size - 1 - cast[uint16](sizeof((gatt_event_value_packet_t)))
  return decode_event_offset(event_packet, gatt_event_value_packet_t, 1)

## *
##  @brief Get value from event GATT_EVENT_LONG_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT
##  @param[in]           event packet
##  @param[in]           event_size
##  @param[out]          value_size
##  @return gatt_event_value_packet_t *
##

proc gatt_event_long_characteristic_descriptor_query_result_parse*(
    event_packet: ptr uint8; event_size: uint16; value_size: ptr uint16): ptr gatt_event_long_value_packet_t =
  value_size[] = event_size - 1 -
      cast[uint16](sizeof((gatt_event_long_value_packet_t)))
  return decode_event_offset(event_packet, gatt_event_long_value_packet_t, 1)

## *
##  @brief Get field handle from event GATT_EVENT_MTU
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc gatt_event_mtu_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field MTU from event GATT_EVENT_MTU
##  @param event packet
##  @return MTU
##  @note: btstack_type 2
##

proc gatt_event_mtu_get_mtu*(event: ptr uint8): uint16 =
  return little_endian_read_16(event, 4)
