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

void i2c_init(const i2c_port_t port);
void i2c_write(const i2c_port_t port, uint8_t addr, const uint8_t *byte_data, int16_t length);
void i2c_read(const i2c_port_t port, uint8_t addr, const uint8_t *write_data, int16_t write_len, 
              uint8_t *byte_data, int16_t length);

#ifdef __cplusplus
}
#endif

#endif
