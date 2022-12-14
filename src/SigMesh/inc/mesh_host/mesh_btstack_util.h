#ifndef MESH_BTSTACK_UTIL_H
#define MESH_BTSTACK_UTIL_H

/* API_START */

/**
 * @brief Minimum function for uint32_t
 * @param a
 * @param b
 * @return value
 */
uint32_t btstack_min(uint32_t a, uint32_t b);

/**
 * @brief Maximum function for uint32_t
 * @param a
 * @param b
 * @return value
 */
uint32_t btstack_max(uint32_t a, uint32_t b);

/**
 * @brief Calculate delta between two points in time
 * @return time_a - time_b - result > 0 if time_a is newer than time_b
 */
int32_t btstack_time_delta(uint32_t time_a, uint32_t time_b);


/** 
 * @brief Read 16/24/32 bit big endian value from buffer
 * @param buffer
 * @param position in buffer
 * @return value
 */
// uint32_t big_endian_read_16(const uint8_t * buffer, int position);
uint32_t big_endian_read_24(const uint8_t * buffer, int position);
// uint32_t big_endian_read_32(const uint8_t * buffer, int position);

/** 
 * @brief Write 16/32 bit big endian value into buffer
 * @param buffer
 * @param position in buffer
 * @param value
 */
// void big_endian_store_16(uint8_t * buffer, uint16_t position, uint16_t value);
void big_endian_store_24(uint8_t * buffer, uint16_t position, uint32_t value);
// void big_endian_store_32(uint8_t * buffer, uint16_t position, uint32_t value);

/** 
 * @brief Write 16/32 bit little endian value into buffer
 * @param buffer
 * @param position in buffer
 * @param value
 */
// void little_endian_store_16(uint8_t * buffer, uint16_t position, uint16_t value);
void little_endian_store_24(uint8_t * buffer, uint16_t position, uint32_t value);
// void little_endian_store_32(uint8_t * buffer, uint16_t position, uint32_t value);

/**
 * Returns the number of leading 0-bits in x, starting at the most significant bit position.
 * If x is 0, the result is undefined.
 * @note maps to __builtin_clz for gcc and clang
 * @param value
 * @return number of leading 0-bits
 */
uint8_t btstack_clz(uint32_t value);


/**
 * @brief Check CRC8 using ETSI TS 101 369 V6.3.0.
 * @note Only used by RFCOMM
 * @param data
 * @param len
 * @param check_sum
 */
uint8_t btstack_crc8_check(uint8_t * data, uint16_t len, uint8_t check_sum);

/**
 * @brief Calculate CRC8 using ETSI TS 101 369 V6.3.0. 
 * @note Only used by RFCOMM
 * @param data
 * @param len
 */
uint8_t btstack_crc8_calc(uint8_t * data, uint16_t len);

#endif



