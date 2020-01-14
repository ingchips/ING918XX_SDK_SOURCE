#ifndef __I2C_H
#define __I2C_H

#include <stdint.h>

#include "cm32gpm3_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

void i2c_init(const i2c_port_t port);
void i2c_write(const i2c_port_t port, uint8_t addr, const uint8_t *byte_data, int16_t length);
void i2c_read(const i2c_port_t port, uint8_t addr, const uint8_t *write_data, int16_t write_len, 
              uint8_t *byte_data, int16_t length);

#ifdef __cplusplus
}
#endif

#endif
