import
  cm32gpm3_i2c

proc i2c_init*(port: i2c_port_t) {.importc: "i2c_init", header: "iic.h".}
proc i2c_write*(port: i2c_port_t; `addr`: uint8; byte_data: ptr uint8; length: int16) {.
    importc: "i2c_write", header: "iic.h".}
proc i2c_read*(port: i2c_port_t; `addr`: uint8; write_data: ptr uint8; write_len: int16;
              byte_data: ptr uint8; length: int16) {.importc: "i2c_read",
    header: "iic.h".}