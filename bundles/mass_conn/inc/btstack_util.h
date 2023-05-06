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

#ifndef __BTSTACK_UTIL_H
#define __BTSTACK_UTIL_H

#include <stdint.h>

#include "bluetooth.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef C2NIM
#define read_little_endian_t(T, buffer, position)                       (*(const T *)((const uint32_t)(buffer) + (position)))
#define little_endian_store_t(T, buffer, position, v)                   (*(T *)((const uint32_t)(buffer) + (position)) = (v))
#else
#define read_little_endian_t(T, buffer, position)                       (*(const T *)((uint32_t)(buffer) + (position)))
#define little_endian_store_t(T, buffer, position, v)                   (*(T *)((uint32_t)(buffer) + (position)) = (v))
#endif

//basic utility:
#define little_endian_read_16(buffer, position)                         read_little_endian_t(uint16_t, buffer, position)
#define little_endian_read_24(buffer, position)                         (little_endian_read_32(buffer, position) & 0xffffffu)
#define little_endian_read_32(buffer, position)                         read_little_endian_t(uint32_t, buffer, position)

#define little_endian_store_16(buffer, position, value)            little_endian_store_t(uint16_t, buffer, position, value)
#define little_endian_store_32(buffer, position, value)            little_endian_store_t(uint32_t, buffer, position, value)

/**
 * @brief Read 16/24/32 bit big endian value from buffer
 * @param buffer
 * @param position in buffer
 * @return value
 */
uint32_t big_endian_read_16( const uint8_t * buffer, int pos);
uint32_t big_endian_read_32( const uint8_t * buffer, int pos);

/**
 * @brief Write 16/32 bit big endian value into buffer
 * @param buffer
 * @param position in buffer
 * @param value
 */
void big_endian_store_16(uint8_t *buffer, uint16_t pos, uint16_t value);
void big_endian_store_32(uint8_t *buffer, uint16_t pos, uint32_t value);

/**
 * @brief Copy from source to destination and reverse byte order
 * @param src
 * @param dest
 * @param len
 */
void reverse_bytes  (const uint8_t *src, uint8_t * dest, int len);

/**
 * @brief Wrapper around reverse_bytes for common buffer sizes
 * @param src
 * @param dest
 */
void reverse_24 (const uint8_t *src, uint8_t * dest);
void reverse_48 (const uint8_t *src, uint8_t * dest);
void reverse_56 (const uint8_t *src, uint8_t * dest);
void reverse_64 (const uint8_t *src, uint8_t * dest);
void reverse_128(const uint8_t *src, uint8_t * dest);

void reverse_bd_addr(const uint8_t *src, uint8_t * dest);

// will be moved to daemon/btstack_device_name_db.h

/**
 * @brief The device name type
 */
#define DEVICE_NAME_LEN 248
typedef uint8_t device_name_t[DEVICE_NAME_LEN+1];

/**
 * @brief ASCII character for 4-bit nibble
 * @return character
 */
char char_for_nibble(int nibble);

/**
 * @brif 4-bit nibble from ASCII character
 * @return value
 */
int nibble_for_char(char c);

/**
 * @brief Compare two Bluetooth addresses
 * @param a
 * @param b
 * @return true if equal
 */
int bd_addr_cmp(bd_addr_t a, bd_addr_t b);

/**
 * @brief Copy Bluetooth address
 * @param dest
 * @param src
 */
void bd_addr_copy(bd_addr_t dest, const bd_addr_t src);

/**
 * @brief Use printf to write hexdump as single line of data
 */
void printf_hexdump(const void *data, int size);

/**
 * @brief Create human readable representation for UUID128
 * @note uses fixed global buffer
 * @return pointer to UUID128 string
 */
char * uuid128_to_str(const uint8_t * uuid);

/**
 * @brief Create human readable represenationt of Bluetooth address
 * @note uses fixed global buffer
 * @return pointer to Bluetooth address string
 */
char * bd_addr_to_str(const bd_addr_t addr);

/**
 * @brief Parse Bluetooth address
 * @param address_string
 * @param buffer for parsed address
 * @return 1 if string was parsed successfully
 */
int sscanf_bd_addr(const char * addr_string, bd_addr_t addr);

/**
 * @brief Constructs UUID128 from 16 or 32 bit UUID using Bluetooth base UUID
 * @param uuid128 output buffer
 * @param short_uuid
 */
void uuid_add_bluetooth_prefix(uint8_t * uuid128, uint32_t short_uuid);

/**
 * @brief Checks if UUID128 has Bluetooth base UUID prefix
 * @param uui128 to test
 * @return 1 if it can be expressed as UUID32
 */
int  uuid_has_bluetooth_prefix(const uint8_t * uuid128);

enum btstack_config_item {
    STACK_ATT_SERVER_ENABLE_AUTO_DATA_LEN_REQ = 1,      // enable automatic LL_DATA_LENGTH_REQ in MTU exchange of att server (default: Disabled)
    STACK_GATT_CLIENT_DISABLE_AUTO_DATA_LEN_REQ = 2,    // disable automatic LL_DATA_LENGTH_REQ in MTU exchange of gatt client (default: Enabled)
                                                        // See also `gap_set_data_length()`.
    STACK_DISABLE_L2CAP_TIMEOUT = 4,                    // Disable automatic L2CAP disconnect if no L2CAP connection is established (default: Enabled)
                                                        // Only for PTS testing
    STACK_SM_USE_FIXED_CSRK = 8,                        // for testing only (default: SM not use fixed CSRK)
    STACK_GATT_CLIENT_DISABLE_MTU_EXCHANGE = 16,        // suppress MTU exchange in gatt client
                                                        // Only for PTS testing
};

/**
 * @brief Bluetooth LE stack configurations
 * @param flags     flags (combination of btstack_config_item)
 */
void btstack_config(uint32_t flags);

#ifdef __cplusplus
}
#endif

#endif // __BTSTACK_UTIL_H
