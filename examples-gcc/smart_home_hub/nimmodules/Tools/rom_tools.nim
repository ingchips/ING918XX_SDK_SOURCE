## *
## ***************************************************************************************
##  @brief Calculate a 16bit CRC code (Polynomial x^16+x^15+x^5+1)
##
##  @param[in] buffer        input bytes
##  @param[in] len           data length
##  @return                  CRC result
## ***************************************************************************************
##

type
  f_crc_t* = proc (buffer: ptr uint8; len: uint16): uint16 {.noconv.}

const
  crc* = ((f_crc_t)(0x00000F79))
