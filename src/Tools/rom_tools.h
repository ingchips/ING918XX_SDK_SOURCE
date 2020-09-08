#ifndef _CRC_H
#define _CRC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 * @brief Calculate a 16bit CRC code (Polynomial x^16+x^15+x^5+1)
 *
 * @param[in] buffer        input bytes
 * @param[in] len           data length
 * @return                  CRC result
 ****************************************************************************************
 */
typedef uint16_t (* f_crc_t)(uint8_t *buffer, uint16_t len);
#define crc     ((f_crc_t)(0x00000f79))

#ifdef __cplusplus
}
#endif

#endif

