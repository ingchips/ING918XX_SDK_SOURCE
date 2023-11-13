#ifndef _PERIPHERAL_QDEC_H
#define _PERIPHERAL_QDEC_H

#include <stdint.h>

#include "ingsoc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
typedef enum {
    QDEC_CH_CTRL        = 0x00,
    QDEC_CH_MODE        = 0x04,
    QDEC_CH_WRITE_A     = 0x14,
    QDEC_CH_WRITE_B     = 0x18,
    QDEC_CH_WRITE_C     = 0x1c,
    QDEC_CH_TIOB0_RD    = 0x20,
    QDEC_CH_INT_EN      = 0x24,
    QDEC_BCR            = 0xc0,
    QDEC_BMR            = 0xc4,
    QDEC_STATUS_SEL     = 0xd4,
} QDEC_qdecReg;

typedef enum {
    QDEC_DIV_4          = 0x0,
    QDEC_DIV_16         = 0x1,
    QDEC_DIV_64         = 0x2,
    QDEC_DIV_256        = 0x3,
    QDEC_DIV_1024       = 0x4,
    QDEC_DIV_4096       = 0x5,
    QDEC_DIV_16384      = 0x6,
    QDEC_DIV_65536      = 0x7,
} QDEC_indexCfg;

/**
 * @brief Set QDEC index divider and enable QDEC index register
 *
 * Interrupt or DMA request is triggered by the output of this divider.
 * Input of this divider is `clk_qdec_div` which is the output of another
 * divider on `qdec_clk` (see `SYSCTRL_SelectQDECClk`).
 *
 * @param[in] div            QDEC index div, see 'QDEC_indexCfg'
 */
void QDEC_EnableQdecDiv(QDEC_indexCfg div);

/**
 * @brief QDEC stantard configuration interface
 *
 * @param[in] fliter         QDEC signal filter value(0-63)
 * @param[in] miss           max value of QDEC miss signal(0-15)
 */
void QDEC_QdecCfg(uint8_t fliter, uint8_t miss);

/**
 * @brief Enable/disable QDEC channel
 *
 * @param[in] enable         enable/disable
 */
void QDEC_ChannelEnable(uint8_t enable);

/**
 * @brief Get QDEC decoding result
 *
 * @return                   accumulation of valid motion sample values
 */
uint16_t QDEC_GetData(void);

/**
 * @brief Get the direction of the most recent valid motion
 *
 * The overall direction of motion within a period of time can be determined
 * by comparing accumulation values.
 *
 * @return                   0:clockwise, 1:anticlockwise
 */
uint8_t QDEC_GetDirection(void);

/**
 * @brief Reset QDEC
 *
 */
void QDEC_Reset(void);

/**
 * @brief Clear QDEC interrupt
 *
 */
void QDEC_IntClear(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
