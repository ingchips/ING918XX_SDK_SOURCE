// ----------------------------------------------------------------------------
// Copyright Message
// ----------------------------------------------------------------------------
//
// INGCHIPS confidential and proprietary.
// COPYRIGHT (c) 2018-2023 by INGCHIPS
//
// All rights are reserved. Reproduction in whole or in part is
// prohibited without the written consent of the copyright owner.
//
//
// ----------------------------------------------------------------------------

#ifndef __SM_H
#define __SM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "compiler.h"
#include "bluetooth.h"
#include "bt_types.h"
#include "btstack_event.h"
#include "btstack_linked_list.h"

/* API_START */

const static __INLINE uint8_t * sm_private_random_addr_update_get_address(const uint8_t *packet){
    return decode_event_offset(packet, uint8_t, 2);
}

/**
 * @brief persistent data for security & privacy
 */
typedef struct sm_persistent
{
    sm_key_t        er;
    sm_key_t        ir;
    bd_addr_t       identity_addr;      // A public device address or static random address used as identity address
                                        // When privacy is not enabled, this should be the public device address or static random address.
                                        // This should not be changed, once changed, paring is lost
    bd_addr_type_t  identity_addr_type; // Note: only PUBLIC & RANDOM are allowed
} sm_persistent_t;

// Authorization state
typedef enum {
    AUTHORIZATION_UNKNOWN,
    AUTHORIZATION_PENDING,
    AUTHORIZATION_DECLINED,
    AUTHORIZATION_GRANTED
} authorization_state_t;

/**
 * @brief Security configurations
 *
 * These configurations can updated dynamically. Be careful: dynamically toggle
 * `enable` will cause problems when SM is still working on any connection.
 *
 * After IRK/DHK are derived from `persistent`, `SM_EVENT_IRK_DHK_RESULT` is emitted.
 *
 * @param[in]   enable              Enable (Bypass) SM (default: Disabled)
 *                                  When disabled, SM can be enabled per connection by `sm_config_conn`.
 * @param[in]   io_capability       Default IO Capabilities
 * @param[in]   request_security    Let peripheral request an encrypted connection right after connecting
 *                                  Not used normally. Bonding is triggered by access to protected attributes in ATT Server
 * @param[in]   persistent          persistent data for security & privacy
 * @return                          0 if ok else non-0. Possible causes for non-0 return value:
 *                                      * generation of internal keys(IRK/ERK) are under going.
 */
int sm_config(uint8_t enable,
               io_capability_t io_capability,
               int   request_security,
               const sm_persistent_t *persistent);

/**
 * @brief add an sm event handler
 *
 * @param callback_handler      the handler
 */
void sm_add_event_handler(btstack_packet_callback_registration_t * callback_handler);

/**
 * @brief Enable private random address generation
 *        When a new random address is generated, an SM_EVENT_PRIVATE_RANDOM_ADDR_UPDATE event is emitted
 *
 * @param random_address_type to use (incl. OFF)
 */
void sm_private_random_address_generation_set_mode(gap_random_address_type_t random_address_type);


/**
 * @brief Get private random address generation mode
 */
gap_random_address_type_t sm_private_random_address_generation_get_mode(void);

/**
 * @brief Set private random address generation period
 * @param period_ms         generation period
 */
void sm_private_random_address_generation_set_update_period(int period_ms);

/**
 * @brief Get current private random address
 * @return      the address
 */
const uint8_t *sm_private_random_address_generation_get(void);

/**
 * @brief Registers OOB Data Callback.
 *
 * The callback should set the oob_data and return 1 if OOB data is available.
 *
 * Signature of `get_oob_data_callback`:
 *      @param[in]  address_type            address type
 *      @param[in]  addr                    address
 *      @param[out] oob_data                OOB data (type `sm_key_t`)
 *      @return                             1 if OOB data is available, else 0.
 *
 * @param get_oob_data_callback
 */
void sm_register_oob_data_callback( int (*get_oob_data_callback)(uint8_t address_type, bd_addr_t addr, uint8_t * oob_data));

/**
 * @brief Registers secure pairing OOB Data Callback.
 *
 * The callback should set the peer_confirm & peer_random and return 1
 * if OOB data is available.
 *
 * Signature of `get_sc_oob_data_callback`:
 *      @param[in]  address_type            address type
 *      @param[in]  addr                    address
 *      @param[out] peer_confirm            peer confirm value (type `sm_key_t`)
 *      @param[out] peer_random             peer random value (type `sm_key_t`)
 *      @return                             1 if OOB data is available, else 0.
 *
 * @param get_sc_oob_data_callback
 */
// void sm_register_sc_oob_data_callback(int (*get_sc_oob_data_callback)(uint8_t address_type, bd_addr_t addr, uint8_t *peer_confirm, uint8_t *peer_random));
// WARNING: ^^^ this API is not available in this release


/**
 * @brief (When secure pairing is used and OOB is selected) Start generate local
 * OOB data (_confirm_ and _random_).
 *
 * After generated, _confirm_ and _random_ are passed to the callback.
 *
 * Each call of this function will generate a new P256 key pair that will used
 * in **all** subsequent pairing attempts. When OOB is used, it is Developers's
 * responsibility to refresh key pair properly. If OOB is not used, SM will
 * re-generate a new key pair for each pairing attempt.
 *
 * BLUETOOTH CORE SPECIFICATION Version 5.4 | Vol 3, Part H, Section 2.3.6:
 *
 * > To protect a device's private key, a device should implement a method to
 * > prevent an attacker from retrieving useful information about the device's private
 * > key. For this purpose, a device should change its private key after every pairing
 * > (successful or failed). Otherwise, it should change its private key whenever S +
 * > 3F > 8, where S is the number of successful pairings and F the number of
 * > failed attempts since the key was last changed.
 *
 * App can then pass these information together with device address to peer
 * through OOB communication.
 *
 * Signature of `callback`:
 *      @param[out] confirm                 local confirm value (type `sm_key_t`)
 *      @param[out] random                  local random value (type `sm_key_t`)
 *
 * @param callback
 * @return               0: started without error
 *                      -1: previous OOB data is not generated yet
 */
// int sm_sc_generate_oob_data(void (*callback)(const uint8_t *confirm, const uint8_t *random));
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Limit the STK generation methods. Bonding is stopped if the resulting one isn't in the list
 * @param OR combination of SM_STK_GENERATION_METHOD_
 */
void sm_set_accepted_stk_generation_methods(uint8_t accepted_stk_generation_methods);

/**
 * @brief Set the accepted encryption key size range. Bonding is stopped if the result isn't within the range
 * @param min_size (default 7)
 * @param max_size (default 16)
 */
void sm_set_encryption_key_size_range(uint8_t min_size, uint8_t max_size);

/**
 * @brief Sets the requested authentication requirements, bonding yes/no, MITM yes/no
 * @param OR combination of SM_AUTHREQ_ flags
 */
void sm_set_authentication_requirements(uint8_t auth_req);

/*
 * @brief Match address against bonded devices
 * @return 0 if successfully added to lookup queue
 * @note Triggers SM_IDENTITY_RESOLVING_* events
 */
int sm_address_resolution_lookup(uint8_t addr_type, bd_addr_t addr);

/**
 * @brief Security configuration of a connection
 *
 * Note: This function can only be used in HCI event handler of `HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE`
 *
 * @param[in]   con_handle          handle of an existing connection
 * @param[in]   io_capability       IO Capabilities
 * @param[in]   auth_req            requested authentication requirements
 */
void sm_config_conn(hci_con_handle_t con_handle,
                    io_capability_t io_capability,
                    uint8_t auth_req);

/**
 * @brief Decline bonding triggered by event before
 * @param addr_type and address
 */
void sm_bonding_decline(hci_con_handle_t con_handle);

/**
 * @brief Confirm Just Works bonding
 * @param addr_type and address
 */
void sm_just_works_confirm(hci_con_handle_t con_handle);

/**
 * @brief Reports passkey input by user
 * @param addr_type and address
 * @param passkey in [0..999999]
 */
void sm_passkey_input(hci_con_handle_t con_handle, uint32_t passkey);

/**
 * @brief Trigger Security Request
 * @note Not used normally. Bonding is triggered by access to protected attributes in ATT Server
 */
void sm_send_security_request(hci_con_handle_t con_handle);

/**
 *
 * @brief Get encryption key size.
 * @param addr_type and address
 * @return 0 if not encrypted, 7-16 otherwise
 */
int sm_encryption_key_size(hci_con_handle_t con_handle);

/**
 * @brief Get authentication property.
 * @param addr_type and address
 * @return 1 if bonded with OOB/Passkey (AND MITM protection)
 */
int sm_authenticated(hci_con_handle_t con_handle);

/**
 * @brief Queries authorization state.
 * @param addr_type and address
 * @return authorization_state for the current session
 */
authorization_state_t sm_authorization_state(hci_con_handle_t con_handle);

/**
 * @brief Used by att_server.c to request user authorization.
 * @param addr_type and address
 */
void sm_request_pairing(hci_con_handle_t con_handle);

/**
 * @brief Report user authorization decline.
 * @param addr_type and address
 */
void sm_authorization_decline(hci_con_handle_t con_handle);

/**
 * @brief Report user authorization grant.
 * @param addr_type and address
 */
void sm_authorization_grant(hci_con_handle_t con_handle);

/**
 * @brief Identify device in LE Device DB.
 * @param handle
 * @return index from le_device_db or -1 if not found/identified
 */
int sm_le_device_key(hci_con_handle_t con_handle);

/**
 * @brief To confirm numeric comparison when SM_EVENT_NUMERIC_COMPARISON_REQUEST is called.
 * @param handle
 */
// void sm_numeric_comparison_confirm(hci_con_handle_t con_handle);
// WARNING: ^^^ this API is not available in this release


/**
 *
 * @brief Register a callback for get external LTK (for empty EDIV & Random)
 *
 * WARNING: This is subject to change.
 *
 * @param get_external_ltk_callback, where
 *              @param[in]  con_handle          connection handle
 *              @param[out] ltk                 LTK
 *              @return                         0 when LTK is stored into `ltk` else non-zero
 */
void sm_register_external_ltk_callback(int (*get_external_ltk_callback)(hci_con_handle_t con_handle, uint8_t *ltk));

/**
 * @brief SM state event
 */
enum sm_state_t
{
    SM_STARTED,
    SM_FINAL_PAIRED,                // successfully paired with a new device
    SM_FINAL_REESTABLISHED,         // connection reestablished with a paired device
    SM_FINAL_FAIL_PROTOCOL,         // protocol error occurred
    SM_FINAL_FAIL_TIMEOUT,          // timeout occurred
    SM_FINAL_FAIL_DISCONNECT,       // unexpected disconnection occurred
    SM_FINAL_FAIL_OUT_OF_STORAGE,   // device database runs out of storage
                                    // i.e. too many devices have been paired.
};

#ifdef __cplusplus
}
#endif

#endif // __SM_H
