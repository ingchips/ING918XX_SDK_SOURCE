
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
 *  - if buffer == NULL, don't copy data, just return size of value
 *                       when ATT_DEFERRED_READ is returned in this case, deferred read is used
 *                       Note: Deferred read works for ATT_READ_REQUEST, ATT_READ_BLOB_REQUEST, ATT_READ_BY_TYPE_REQUEST.
 *                             Deferred read does NOT work for ATT_READ_MULTIPLE_REQUEST, in which case
 *                                     the value length is treated as 0.
 *  - if buffer != NULL, copy data and return number bytes copied
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

/**
 * @brief Enable support for Enhanced ATT bearer
 * @note Requires ENABLE_GATT_OVER_EATT
 * @param num_eatt_bearers
 * @param storage_buffer
 * @param storage_size must be >= num_eatt_bearers * sizeof(att_server_eatt_bearer_t)
 * @return status   ERROR_CODE_SUCCESS
 *                  ERROR_CODE_MEMORY_CAPACITY_EXCEEDED if buffer too small or no entry in l2cap service pool
 *                  L2CAP_SERVICE_ALREADY_REGISTERED if called twice
 */
// uint8_t att_server_eatt_init(uint8_t num_eatt_bearers, uint8_t * storage_buffer, uint16_t storage_size);
// WARNING: ^^^ this API is not available in this release


/* API_START */
/*
 * @brief setup ATT server
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
 * @param value                  buffer to value starting from `offset` given in read callback
 * @param value_len              length of value starting from `offset` given in read callback
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
 * @brief setup ATT database and enable robust caching feature by telling stack
          the characteristic handles of the Generic Attribute Profile service.
 *
 * Note: stack will take over read/write operation of Generic Attribute Profile service
 *       once enabled.
 *
 * When switching database, handles related to Generic Attribute Profile service shall not be changed.
 *
 * @param con_handle                                    associated connection handle
 * @param db                                            the ATT db
 * @param att_handle_service_changed                    handle of Service Changed characteristic
 * @param att_handle_service_changed_cccd               handle of Client_Characteristics Configuration descriptor
 *                                                      for Service Changed characteristic
 * @param att_handle_client_supported_features          handle of Client Supported Features characteristic
 * @param att_handle_database_hash                      handle of Database Hash characteristic
 * @param att_handle_server_supported_features          handle of Server Supported Features characteristic
 */
// void att_set_db2(hci_con_handle_t con_handle, const uint8_t *db,
//     uint16_t att_handle_service_changed,
//     uint16_t att_handle_service_changed_cccd,
//     uint16_t att_handle_client_supported_features,
//     uint16_t att_handle_database_hash,
//     uint16_t att_handle_server_supported_features);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief visitor callback for update visibility of services
 *
 * @param con_handle        The connection handle to which the service is associated.
 * @param service_handle    The handle of the service whose visibility is to be set.
 * @param flags             Flags of this service (combination of `ATT_PROPERTY_...`)
 * @param uuid              UUID of this service (maybe UUID-128 or UUID-16)
 * @param user_data         User data
 * @return                  Visible (1) or invisible(0)
 */
typedef int (*att_get_service_visibility_t)(hci_con_handle_t con_handle,
    uint16_t service_handle, uint16_t flags, const uint8_t *uuid, void *user_data);

/**
 * @brief Update the visibility of each service for a given connection handle.
 *
 * This function allows you to refresh the visibility of all services
 * associated with a connection handle. The visibility can be toggled on or off
 * based on the return value of `callback`.
 *
 * IMPORTANT:
 *      - This is applicable only if the database is stored in RAM.
 *      - When robust caching is enabled, Generic Attribute Profile shall be kept visible.
 *
 * @param con_handle        The connection handle to which the service is associated.
 * @param callback          Callback function to get the updated visibility of each service.
 * @param user_data         User data passed to `callback`.
 * @return                  Returns 0 on success, or error code on failure:
 *                          -1: connection not exists
 */
// int att_update_service_visibility(hci_con_handle_t con_handle,
//     att_get_service_visibility_t callback, void *user_data);
// WARNING: ^^^ this API is not available in this release


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
