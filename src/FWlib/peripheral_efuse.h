
#ifndef __PERIPHERAL_EFUSE_H__
#define __PERIPHERAL_EFUSE_H__


#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "ingsoc.h"


#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916

/*
 * Description:
 * Bit shifts and widths for efuse_cfg0
 */
#define bsEFUSE_CFG0_PROM_MODE           0
#define bsEFUSE_CFG0_PROM_ADDR           1
#define bsEFUSE_CFG0_PG_EFUSE            8
#define bsEFUSE_CFG0_RD_EFUSE            9
#define bsEFUSE_CFG0_DATA_VALID          10
#define bsEFUSE_CFG0_EFUSE_LOCK_FLAG     11
#define bsEFUSE_CFG0_EFUSE_LOCK          12

#define bwEFUSE_CFG0_PROM_MODE           1
#define bwEFUSE_CFG0_PROM_ADDR           7
#define bwEFUSE_CFG0_PG_EFUSE            1
#define bwEFUSE_CFG0_RD_EFUSE            1
#define bwEFUSE_CFG0_DATA_VALID          1
#define bwEFUSE_CFG0_EFUSE_LOCK_FLAG     1
#define bwEFUSE_CFG0_EFUSE_LOCK          6

/*
 * Description:
 * Bit shifts and widths for efuse_status
 */
#define bsEFUSE_STATUS_BUSY            0
#define bsEFUSE_STATUS_STATE           1

#define bwEFUSE_STATUS_BUSY            1
#define bwEFUSE_STATUS_STATE           4

#define EFUSE_REG_WORD_NUM (4)
#define EFUSE_UNLOCK_FLAG (0xef5e)

typedef enum
{
  EFUSE_PROGRAMWORDCNT_0,//0 ~ 31bit
  EFUSE_PROGRAMWORDCNT_1,//32 ~ 63bit
  EFUSE_PROGRAMWORDCNT_2,//64 - 95bit
  EFUSE_PROGRAMWORDCNT_3//96 - 127bit
}EFUSE_ProgramWordCnt;

void EFUSE_SetCfg0(EFUSE_TypeDef* EFUSE_BASE, uint32_t val);

/**
 * @brief Get EFUSE data
 *
 * @param[in] EFUSE_BASE              base address
 * @param[in] index                   from 0 to 3(each represent 32bit)
 */
uint32_t EFUSE_GetEfuseData(EFUSE_TypeDef* EFUSE_BASE, EFUSE_ProgramWordCnt index);

/**
 * @brief Set EFUSE data per bit
 *
 * @param[in] EFUSE_BASE              base address
 * @param[in] addr                    from 0 to 127
 */
void EFUSE_WriteEfuseDataBitToOne(EFUSE_TypeDef* EFUSE_BASE, uint8_t addr);

/**
 * @brief Unlock EFUSE before write access
 *
 * @param[in] EFUSE_BASE              base address
 * @param[in] data                    has to be EFUSE_UNLOCK_FLAG
 */
void EFUSE_UnLock(EFUSE_TypeDef* EFUSE_BASE, uint16_t data);

/**
 * @brief Lock EFUSE after write access
 *
 * @param[in] EFUSE_BASE              base address
 */
void EFUSE_Lock(EFUSE_TypeDef* EFUSE_BASE);

uint8_t EFUSE_GetStatusState(EFUSE_TypeDef* EFUSE_BASE);
uint8_t EFUSE_GetDataValidFlag(EFUSE_TypeDef* EFUSE_BASE);

/**
 * @brief Read flag before read access
 *
 * @param[in] EFUSE_BASE              base address
 */
void EFUSE_SetRdFlag(EFUSE_TypeDef* EFUSE_BASE);

/**
 * @brief Set EFUSE data per word
 *
 * @param[in] EFUSE_BASE              base address
 * @param[in] index                   from 0 to 3
 * @param[in] data                    data to program
 */
void EFUSE_WriteEfuseDataWord(EFUSE_TypeDef* EFUSE_BASE, EFUSE_ProgramWordCnt index, uint32_t data);

#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

