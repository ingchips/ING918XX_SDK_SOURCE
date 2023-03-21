
/** @file
*  @brief API for ATT server
*
*  INGCHIPS confidential and proprietary.
*  COPYRIGHT (c) 2018-2023 by INGCHIPS
*
*  All rights are reserved. Reproduction in whole or in part is
*  prohibited without the written consent of the copyright owner.
*
*
*/

#ifndef __ATT_H
#define __ATT_H

#include <stdint.h>
#include "bluetooth.h"
#include "bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bluetooth
 * @defgroup Bluetooth_att_db
 * @ingroup bluetooth_host
 * @{
 */
// custom btstack error codes
#define ATT_ERROR_HCI_DISCONNECT_RECEIVED          0x1f

// custom btstack ATT error coders
#define ATT_ERROR_DATA_MISMATCH                    0x7e
#define ATT_ERROR_TIMEOUT                          0x7F

#define ATT_ERROR_CMD_UNSUPPORTED                  0x06

#define ATT_DEFERRED_READ                           0xffff

/**@brief ATT Client Read Callback for Dynamic Data
    - if buffer == NULL, don't copy data, just return size of value
                         when ATT_DEFERRED_READ is returned in this case, deferred read is used
                         Note: Deferred read only works for ATT_READ_REQUEST
     - if buffer != NULL, copy data and return number bytes copied
 * @param con_handle       hci le connection
 * @param attribute_handle attribute handle in att database
 * @param offset           defines start of attribute value
 * @param buffer           buffer to allocate the read value
 * @param buffer_size      buffer size
 * @retrun
 */
typedef uint16_t (*att_read_callback_t)(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);

/**
 * @brief ATT Client Write Callback for Dynamic Data
 * @param con_handle of hci le connection
 * @param attribute_handle the attribute handle in the att database
 * @param transaction - ATT_TRANSACTION_MODE_NONE for regular writes, ATT_TRANSACTION_MODE_ACTIVE for prepared writes and ATT_TRANSACTION_MODE_EXECUTE
 * @param offset into the value - used for queued writes and long attributes
 * @param buffer        the data to be written
 * @param buffer_size   the size of buffer
 * @param signature used for signed write commmands
 * @returns             0 if write was ok, ATT_ERROR_PREPARE_QUEUE_FULL if no space in queue, ATT_ERROR_INVALID_OFFSET if offset is larger than max buffer
 */
typedef int (*att_write_callback_t)(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, const uint8_t *buffer, uint16_t buffer_size);

/* API_START */
/*
 * @brief setup ATT server
 * @param db              attribute database in att server
 * @param read_callback   see function type @link att_read_callback_t() @endlink
 * @param write_callback  see function type @link att_write_callback_t() @endlink
 */
void att_server_init(att_read_callback_t read_callback, att_write_callback_t write_callback);

/*
 * @brief register packet handler for ATT server events:
 *        - ATT_EVENT_MTU_EXCHANGE_COMPLETE
 *        - ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE
 * @param handler         see function type @link btstack_packet_handler_t() @endlink
 */
void att_server_register_packet_handler(btstack_packet_handler_t handler);

/*
 * @brief tests if a notification or indication can be send right now
 * @param con_handle      le connection handle
 * @return 1, if packet can be sent
 */
int  att_server_can_send_packet_now(hci_con_handle_t con_handle);

/**
 * @brief Request emission of ATT_EVENT_CAN_SEND_NOW as soon as possible
 * @note ATT_EVENT_CAN_SEND_NOW might be emitted during call to this function
 *       so packet handler should be ready to handle it
 * @param con_handle      le connection handle
 */
void att_server_request_can_send_now_event(hci_con_handle_t con_handle);

/*
 * @brief notify stack that deferred read is complete
 * @param con_handle             le connection handle
 * @param attribute_handle       the attribute handle in the att database
 * @param value                  buffer to value
 * @param value_len              length of value
 * @return                       0 if ok, error otherwise
 */
int att_server_deferred_read_response(hci_con_handle_t con_handle, uint16_t attribute_handle, const uint8_t *value, uint16_t value_len);

/*
 * @brief notify client about attribute value change
 * @param con_handle             le connection handle
 * @param attribute_handle       the attribute handle in the att database
 * @param value                  buffer to value
 * @param value_len              length of value
 * @return                       0 if ok, error otherwise
 */
int att_server_notify(hci_con_handle_t con_handle, uint16_t attribute_handle, const uint8_t *value, uint16_t value_len);

/**
 * @brief indicate value change to client. client is supposed to reply with an indication_response
 * @param con_handle             le connection handle
 * @param attribute_handle       the attribute handle in the att database
 * @param value                  buffer to value
 * @param value_len              length of value
 * @return                       0 if ok, error otherwise
 */
int att_server_indicate(hci_con_handle_t con_handle, uint16_t attribute_handle, const uint8_t *value, uint16_t value_len);

/*
 * @brief get current MTU size
 * @return current MTU size
 */
uint16_t att_server_get_mtu(hci_con_handle_t con_handle);

/**
 * @brief emit a general event to att handler
 * @param event                 event to be sent
 * @param length                length of event
 * @return void
 */
void att_emit_general_event(uint8_t *event, uint16_t length);

/*
 * @brief setup ATT database
 * @param con_handle[in]        associated connection handle
 * @param db[in]                the ATT db
 */
void att_set_db(hci_con_handle_t con_handle, const uint8_t *db);

/**
 * @brief set callback for read of dynamic attributes
 * @param callback  see function type @link att_read_callback_t() @endlink
 */
void att_set_read_callback(att_read_callback_t callback);

/**
 * @brief set callback for write of dynamic attributes
 * @param callback  see function type @link att_write_callback_t() @endlink
 */
void att_set_write_callback(att_write_callback_t callback);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif // __ATT_H
