/*
 *  provisioning_device.h
 */

#ifndef __PROVISIONING_DEVICE_H
#define __PROVISIONING_DEVICE_H

#include <stdint.h>

#include "btstack_defines.h"

#include "mesh/mesh_keys.h"
#include "provisioning.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Public Key OOB Available
 * @param public_key  (64 bytes), need to stay valid forever
 * @param private_key (32 bytes), need to stay valid forever 
 * @note Requires ability to transmit public key out-of-band. Memory needs to stay available. Only works with Software ECC.
 */
void provisioning_device_set_public_key_oob(const uint8_t * public_key, const uint8_t * private_key);

/**
 * @brief Static OOB Available
 * @param static_oob_len in bytes
 * @param static_oob_data
 */
void provisioning_device_set_static_oob(uint16_t static_oob_len, const uint8_t * static_oob_data);

/**
 * @brief Configure Output OOB Options
 * @param supported_output_oob_action_types bitfield
 * @param max_oob_output_size in bytes
 */
void provisioning_device_set_output_oob_actions(uint16_t supported_output_oob_action_types, uint8_t max_oob_output_size);

/**
 * @brief Configure Input OOB Options
 * @param supported_input_oob_action_types bitfield
 * @param max_oob_input_size in bytes
 */
void provisioning_device_set_input_oob_actions(uint16_t supported_input_oob_action_types, uint8_t max_oob_input_size);

/**
 * @brief Input OOB Complete Numeric
 * @param pv_adv_cid
 * @Param input_oob number
 */
void provisioning_device_input_oob_complete_numeric(uint16_t pb_adv_cid, uint32_t input_oob);

/**
 * @brief Input OOB Complete Alphanumeric
 * @param pv_adv_cid
 * @Param input_oob_data string
 * @Param input_oob_len 
 */
void provisioning_device_input_oob_complete_alphanumeric(uint16_t pb_adv_cid, const uint8_t * input_oob_data, uint16_t input_oob_len);

/**
 * @brief Get provisioning data after provisioning completed
 * @param provisioning_data 
 */
void provisioning_device_data_get(mesh_provisioning_data_t * provisioning_data);

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
