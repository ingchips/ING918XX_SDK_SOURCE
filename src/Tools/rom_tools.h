#ifndef _CRC_H
#define _CRC_H

#include <stdint.h>
#include "ingsoc.h"

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

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#define crc     ((f_crc_t)(0x00000f79))

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#define crc     ((f_crc_t)(0x00005699))

typedef void (* f_erase_sector)(uint32_t addr);
typedef void (* f_void)(void);
typedef void (* f_prog_page)(uint32_t addr, const uint8_t data[256], uint32_t len);

#define FlashSectorErase    ((f_erase_sector)0x0000150d)
#define FlashWriteDisable   ((f_void)0x000015f1)
#define FlashPageProgram    ((f_prog_page)0x00001405)

#define FlashDisableContinuousMode      ((f_void)0x000012cd)
#define FlashEnableContinuousMode       ((f_void)0x00001311)

#define DCacheDisable       ((f_void)0x000010c1)
#define DCacheEnable        ((f_void)0x000010d5)
#define DCacheFlush         ((f_void)0x000010e9)
#define ICacheDisable       ((f_void)0x00001669)
#define ICacheEnable        ((f_void)0x0000167d)
#define ICacheFlush         ((f_void)0x00001691)

#define ROM_NVIC_SystemReset               ((f_void)0x00001b11)

#endif

#ifdef __cplusplus
}
#endif

#endif

