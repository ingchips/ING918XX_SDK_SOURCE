#ifndef __I2C_H
#define __I2C_H

#include <stdint.h>

#include "peripheral_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

// Note on `addr`:
//
// Device address used here DOES NOT include the trailing 0/1 which is used
// to indicate write or read operation.
//
// For example, let an address be X, many datasheets will give two addresses:
// Write address: (X << 1)
// Read address: (X << 1) + 1
//
// Here, these APIs use the address X.

// Note on timeout:
// This module uses `I2C_HW_TIME_OUT` as a simple timeout detection mechanism.
// Developers should design their own solution.

/**
 * @brief Init an I2C peripheral
 *
 * @param[in] port              I2C peripheral ID
 */
void i2c_init(const i2c_port_t port);

/**
 * @brief Write data to an I2C slave
 *
 * @param[in] port              I2C peripheral ID
 * @param[in] addr              address of the slave
 * @param[in] byte_data         data to be written
 * @param[in] length            data length
 * @return                      0 if success else non-0 (e.g. time out)
 */
int i2c_write(const i2c_port_t port, uint16_t addr, const uint8_t *byte_data, int16_t length);

/**
 * @brief Read data from an I2C slave
 *
 * @param[in] port              I2C peripheral ID
 * @param[in] addr              address of the slave
 * @param[in] write_data        data to be written before reading
 * @param[in] write_len         data length to be written before reading
 * @param[in] byte_data         data to be read
 * @param[in] length            data length to be read
 * @return                      0 if success else non-0 (e.g. time out)
 */
int i2c_read(const i2c_port_t port, uint16_t addr, const uint8_t *write_data, int16_t write_len, 
              uint8_t *byte_data, int16_t length);

#ifdef __cplusplus
}
#endif

#endif
