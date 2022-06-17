## * @file
##   @brief API for ATT server
##
##   INGCHIPS confidential and proprietary.
##   COPYRIGHT (c) 2018 by INGCHIPS
##
##   All rights are reserved. Reproduction in whole or in part is
##   prohibited without the written consent of the copyright owner.
##
##
##

import
  bluetooth, bt_types

## *
##  @brief Bluetooth
##  @defgroup Bluetooth_att_db
##  @ingroup bluetooth_host
##  @{
##
##  custom BTstack error codes

const
  ATT_ERROR_HCI_DISCONNECT_RECEIVED* = 0x0000001F

##  custom BTstack ATT error coders

const
  ATT_ERROR_DATA_MISMATCH* = 0x0000007E
  ATT_ERROR_TIMEOUT* = 0x0000007F

## *@brief ATT Client Read Callback for Dynamic Data
## // - if buffer == NULL, don't copy data, just return size of value
## // - if buffer != NULL, copy data and return number bytes copied
##  @param con_handle       hci le connection
##  @param attribute_handle attribute handle in att database
##  @param offset           defines start of attribute value
##  @param buffer           buffer to allocate the read value
##  @param buffer_size      buffer size
##  @retrun
##

type
  att_read_callback_t* = proc (con_handle: hci_con_handle_t;
                            attribute_handle: uint16; offset: uint16;
                            buffer: ptr uint8; buffer_size: uint16): uint16 {.noconv.}

## *
##  @brief ATT Client Write Callback for Dynamic Data
##  @param con_handle of hci le connection
##  @param attribute_handle the attribute handle in the att database
##  @param transaction - ATT_TRANSACTION_MODE_NONE for regular writes, ATT_TRANSACTION_MODE_ACTIVE for prepared writes and ATT_TRANSACTION_MODE_EXECUTE
##  @param offset into the value - used for queued writes and long attributes
##  @param buffer        the data to be written
##  @param buffer_size   the size of buffer
##  @param signature used for signed write commmands
##  @returns             0 if write was ok, ATT_ERROR_PREPARE_QUEUE_FULL if no space in queue, ATT_ERROR_INVALID_OFFSET if offset is larger than max buffer
##

type
  att_write_callback_t* = proc (con_handle: hci_con_handle_t;
                             attribute_handle: uint16; transaction_mode: uint16;
                             offset: uint16; buffer: ptr uint8; buffer_size: uint16): cint {.
      noconv.}

##  API_START
##
##  @brief setup ATT server
##  @param db              attribute database in att server
##  @param read_callback   see function type @link att_read_callback_t() @endlink
##  @param write_callback  see function type @link att_write_callback_t() @endlink
##

proc att_server_init*(read_callback: att_read_callback_t;
                     write_callback: att_write_callback_t) {.
    importc: "att_server_init", header: "att_db.h".}
##
##  @brief register packet handler for ATT server events:
##         - ATT_EVENT_MTU_EXCHANGE_COMPLETE
##         - ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE
##  @param handler         see function type @link btstack_packet_handler_t() @endlink
##

proc att_server_register_packet_handler*(handler: btstack_packet_handler_t) {.
    importc: "att_server_register_packet_handler", header: "att_db.h".}
##
##  @brief tests if a notification or indication can be send right now
##  @param con_handle      le connection handle
##  @return 1, if packet can be sent
##

proc att_server_can_send_packet_now*(con_handle: hci_con_handle_t): cint {.
    importc: "att_server_can_send_packet_now", header: "att_db.h".}
## *
##  @brief Request emission of ATT_EVENT_CAN_SEND_NOW as soon as possible
##  @note ATT_EVENT_CAN_SEND_NOW might be emitted during call to this function
##        so packet handler should be ready to handle it
##  @param con_handle      le connection handle
##

proc att_server_request_can_send_now_event*(con_handle: hci_con_handle_t) {.
    importc: "att_server_request_can_send_now_event", header: "att_db.h".}
##
##  @brief notify client about attribute value change
##  @param con_handle             le connection handle
##  @param attribute_handle       the attribute handle in the att database
##  @param value                  buffer to value
##  @param value_len              length of value
##  @return                       0 if ok, error otherwise
##

proc att_server_notify*(con_handle: hci_con_handle_t; attribute_handle: uint16;
                       value: ptr uint8; value_len: uint16): cint {.
    importc: "att_server_notify", header: "att_db.h".}
## *
##  @brief indicate value change to client. client is supposed to reply with an indication_response
##  @param con_handle             le connection handle
##  @param attribute_handle       the attribute handle in the att database
##  @param value                  buffer to value
##  @param value_len              length of value
##  @return                       0 if ok, error otherwise
##

proc att_server_indicate*(con_handle: hci_con_handle_t; attribute_handle: uint16;
                         value: ptr uint8; value_len: uint16): cint {.
    importc: "att_server_indicate", header: "att_db.h".}
##
##  @brief get current MTU size
##  @return current MTU size
##

proc att_server_get_mtu*(con_handle: hci_con_handle_t): uint16 {.
    importc: "att_server_get_mtu", header: "att_db.h".}
## *
##  @brief emit a general event to att handler
##  @param event                 event to be sent
##  @param length                length of event
##  @return void
##

proc att_emit_general_event*(event: ptr uint8; length: uint16) {.
    importc: "att_emit_general_event", header: "att_db.h".}
##
##  @brief setup ATT database
##  @param con_handle[in]        associated connection handle
##  @param db[in]                the ATT db
##

proc att_set_db*(con_handle: hci_con_handle_t; db: ptr uint8) {.importc: "att_set_db",
    header: "att_db.h".}
## *
##  @brief set callback for read of dynamic attributes
##  @param callback  see function type @link att_read_callback_t() @endlink
##

proc att_set_read_callback*(callback: att_read_callback_t) {.
    importc: "att_set_read_callback", header: "att_db.h".}
## *
##  @brief set callback for write of dynamic attributes
##  @param callback  see function type @link att_write_callback_t() @endlink
##

proc att_set_write_callback*(callback: att_write_callback_t) {.
    importc: "att_set_write_callback", header: "att_db.h".}
## *
##  @}
##
