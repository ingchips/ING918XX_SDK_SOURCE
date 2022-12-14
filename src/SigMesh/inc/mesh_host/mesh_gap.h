#ifndef MESH_GAP_H
#define MESH_GAP_H

/**
 * @brief Set Advertisement Data
 * @param advertising_data_length
 * @param advertising_data (max 31 octets)
 * @note data is not copied, pointer has to stay valid
 * @note '00:00:00:00:00:00' in advertising_data will be replaced with actual bd addr
 */
void gap_advertisements_set_data(uint8_t adv_handle, uint8_t advertising_data_length, uint8_t * advertising_data);

/**
 * @brief Set Advertisement Parameters
 * @param adv_int_min
 * @param adv_int_max
 * @param adv_type
 * @param direct_address_type
 * @param direct_address
 * @param channel_map
 * @param filter_policy
 * @note own_address_type is used from gap_random_address_set_mode
 */
void gap_advertisements_set_params(uint8_t adv_handle, uint16_t adv_int_min, uint16_t adv_int_max, uint8_t adv_type,
	uint8_t direct_address_typ, bd_addr_t direct_address, uint8_t channel_map, uint8_t filter_policy);

/** 
 * @brief Enable/Disable Advertisements. OFF by default.
 * @param enabled
 */
void gap_advertisements_enable(uint8_t adv_handle, int enabled);

/** 
 * @brief 
 * @param 
 */
uint8_t gap_scan_response_set_data(const uint8_t adv_handle, const uint16_t length, const uint8_t *data);

#endif
