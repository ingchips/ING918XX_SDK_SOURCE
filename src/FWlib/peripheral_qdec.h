#ifndef _PERIPHERAL_QDEC_H
#define _PERIPHERAL_QDEC_H

#include <stdint.h>

#include "ingsoc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916 || INGCHIPS_FAMILY == INGCHIPS_FAMILY_920 )
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

typedef enum {
    QDEC_TIMER = 0,
    QDEC_PWM,
    QDEC_PCM,
    QDEC_QDEC,
} QDEC_ModuCfg;

typedef enum {
    QDEC_CH0 = 0,
    QDEC_CH1,
    QDEC_CH2,
} QDEC_CHX;

typedef enum {
    QDEC_EX_NO_TRIG = 0,
    QDEC_EX_RISING_EDGE,
    QDEC_EX_FALLING_EDGE,
    QDEC_EX_BOTH_EDGE,
}QDEC_ExTrigger;

typedef enum {
    QDEC_INT_BUF_FULL_STATE = 1<<0,
    QDEC_INT_PDC_END_STATE = 1<<1,
    QDEF_INT_ETRGS_CMB_STATE = 1<<2,
    QDEC_INT_LDRBS_CMB_STATE = 1<<3,
    QDEC_INT_LDRAS_CMB_STATE = 1<<4,
    QDEC_INT_CPCS_CMB_STATE = 1<<5,
    QDEC_INT_CPBS_CMB_STATE = 1<<6,
    QDEC_INT_CPAS_CMB_STATE = 1<<7,
    QDEC_INT_LOVFS_CMB_STATE = 1<<8,
    QDEC_INT_COVFS_CMB_STATE = 1<<9,
}QDEC_IntState;

typedef enum {
    QDEC_TMR_RELOAD = 0,
    QDEC_TMR_STOP,
}QDEC_TMR_UP_MODE;

typedef enum {
    QDEC_TMR_RELOAD_UP_FULL = 0,
    QDEC_TMR_RELOAD_UP_VALE,
}QDEC_TMR_RELOAD_MODE;

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


void QDEC_ChModeCfg(QDEC_CHX Channel, QDEC_ModuCfg ModeCfg);
void QDEC_SetChxCpcTrg(QDEC_CHX Channel, uint8_t val);
void QDEC_SetChxCpcStopEn(QDEC_CHX Channel, uint8_t val);
void QDEC_ExternalEventEdgeSet(QDEC_CHX Channel, QDEC_ExTrigger edge);
void QDEC_SetEtrg(QDEC_CHX Channel, uint8_t val);
void QDEC_SetEtrgEn(QDEC_CHX Channel, uint8_t enable);
void QDEC_SetCHxTmrCntA(QDEC_CHX Channel, uint16_t val);
void QDEC_SetCHxTmrCntB(QDEC_CHX Channel, uint16_t val);
void QDEC_SetCHxTmrCntC(QDEC_CHX Channel, uint16_t val);
void QDEC_TmrCfg(QDEC_CHX Channel, QDEC_TMR_UP_MODE TmrUpMode,QDEC_TMR_RELOAD_MODE TmrReloadMode);
void QDEC_SetChxIntEn(QDEC_CHX Channel, uint8_t enable, uint16_t items);
uint32_t QDEC_GetChxIntEn(QDEC_CHX Channel);
uint32_t QDEC_GetChxIntStateClr(QDEC_CHX Channel);
void QDEC_EnableChannel(QDEC_CHX Channel, uint8_t enable);

#endif

#ifdef __cplusplus
}
#endif

#endif
