#ifndef _CRC_H
#define _CRC_H

#include <stdint.h>
#include "ingsoc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t (* f_crc_t)(uint8_t *buffer, uint16_t len);
typedef uint16_t (* f_void_t)(void);
typedef void (* f_void_void_t)(void);

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#define ROM_FUNC_ADDR_CRC                   (0x00000f79)
#define ROM_FUNC_ADDR_BOOT_UART_INIT        (0x000006f5)

#define boot_uart_download  ((f_void_t)(0x00000bc9))

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#define ROM_FUNC_ADDR_CRC                   (0x00001d21)
#define ROM_FUNC_ADDR_BOOT_UART_INIT        (0x00003aa1)

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)

#define ROM_FUNC_ADDR_CRC                   (0x00001ccd)
#define ROM_FUNC_ADDR_BOOT_UART_INIT        (0x000013c3)

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
#define crc                     ((f_crc_t)(ROM_FUNC_ADDR_CRC))

/**
 ****************************************************************************************
 * @brief Start UART downloading mode
 *
 ****************************************************************************************
 */
#define boot_uart_init          ((f_void_void_t)(ROM_FUNC_ADDR_BOOT_UART_INIT))

#ifdef __cplusplus
}
#endif

#endif

