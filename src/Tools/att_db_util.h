#ifndef __ATT_DB_UTIL
#define __ATT_DB_UTIL

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* API_START */

/**
 * @brief Init ATT DB storage
 */
void att_db_util_init(uint8_t *att_db_storage, const int16_t db_max_size);

/**
 * @brief Add primary service for 16-bit UUID
 */
void att_db_util_add_service_uuid16(uint16_t uuid16);

/**
 * @brief Add primary service for 128-bit UUID
 */
void att_db_util_add_service_uuid128(const uint8_t * uuid128);

/**
 * @brief Add primary service for 16-bit UUID
 */
void att_db_util_add_primary_service_uuid16(uint16_t uuid16);

/**
 * @brief Add secondary service for 16-bit UUID
 */
void att_db_util_add_secondary_service_uuid16(uint16_t uuid16);

/**
 * @brief Add include for 16-bit UUID
 */
void att_db_util_add_include_service_uuid16(uint16_t included_service_att_handle, uint16_t end_group_handle, uint16_t uuid16);

/**
 * @brief Set the next attribute value handle to be used by ATT DB.
 */
void att_db_util_set_next_handle(uint16_t handle);

/**
 * @brief Required minimum encryption key size of a characteristic
 *
 * @param[in]   n       required minimum key size {0, 7..16}
 * @returns             encoded bits that can be combined with ATT_PROPERTY_*
 */
#define ATT_PROPERTY_MIN_ENCRYPTION_KEY_SIZE(n)        ((n) > 0 ? (((uint16_t)(n) - 1) & 0xf) << 12 : 0)

/**
 * @brief Add Characteristic with 16-bit UUID, properties, and data
 * @returns attribute value handle
 * @see `ATT_PROPERTY_MIN_ENCRYPTION_KEY_SIZE`, ATT_PROPERTY_* in ble/att_db.h
 */
uint16_t att_db_util_add_characteristic_uuid16(uint16_t uuid16,  uint16_t properties, const uint8_t * data, uint16_t data_len);

/**
 * @brief Add Descriptor with 16-bit UUID, and data
 * @returns descriptor handle
 */
uint16_t att_db_util_add_descriptor_uuid16(uint16_t uuid16, uint16_t flags, const uint8_t * data, uint16_t data_len);

/**
 * @brief Add Characteristic with 128-bit UUID, properties, and data
 * @returns attribute value handle
 * @see `ATT_PROPERTY_REQUIRED_ENCRYPTION_SIZE`, ATT_PROPERTY_* in ble/att_db.h
 */
uint16_t att_db_util_add_characteristic_uuid128(const uint8_t * uuid128, uint16_t properties, const uint8_t * data, uint16_t data_len);

/**
 * @brief Get address of constructed ATT DB
 */
uint8_t * att_db_util_get_address(void);

/**
 * @brief Get size of constructed ATT DB
 */
uint16_t att_db_util_get_size(void);

/* API_END */

#ifdef __cplusplus
}
#endif
#endif
