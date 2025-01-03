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
    QDEC_INT_EN         = 0xc8,
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
} QDEC_ModCfg;

typedef enum {
    QDEC_CH0 = 0,
    QDEC_CH1,
    QDEC_CH2,
} QDEC_CHX;

/** 
 *  Configures the different modes of QDEC external triggering. 
*/
typedef enum {
    QDEC_EX_NO_TRIG = 0,
    QDEC_EX_RISING_EDGE,
    QDEC_EX_FALLING_EDGE,
    QDEC_EX_BOTH_EDGE,
}QDEC_ExTrigger;

/** 
* Qdec module interrupt flag.
 * - QDEC_INT_COVFS_CMB_STATE: Counter overflow state.
 * - QDEC_INT_LOVFS_CMB_STATE: Counter underflow state.
 * - QDEC_INT_CPAS_CMB_STATE: Capture Compare register a state.
 * - QDEC_INT_CPBS_CMB_STATE: Capture Compare register b state.
 * - QDEC_INT_CPCS_CMB_STATE: Capture Compare register c state.
 * - QDEC_INT_LDRAS_CMB_STATE: Load register a state.
 * - QDEC_INT_LDRBS_CMB_STATE: Load register b state.
 * - QDEC_INT_ETRGS_CMB_STATE: External trigger state.
 * - QDEC_INT_PDC_END_STATE: Periodic data conversion end state.
 * - QDEC_INT_BUF_FULL_STATE: Buffer full state.
*/ 
typedef enum {
    QDEC_INT_COVFS_CMB_STATE = 1<<0,
    QDEC_INT_LOVFS_CMB_STATE = 1<<1,
    QDEC_INT_CPAS_CMB_STATE = 1<<2,
    QDEC_INT_CPBS_CMB_STATE = 1<<3,
    QDEC_INT_CPCS_CMB_STATE = 1<<4,
    QDEC_INT_LDRAS_CMB_STATE = 1<<5,
    QDEC_INT_LDRBS_CMB_STATE = 1<<6,
    QDEF_INT_ETRGS_CMB_STATE = 1<<7,
    QDEC_INT_PDC_END_STATE = 1<<8,
    QDEC_INT_BUF_FULL_STATE = 1<<9,
}QDEC_IntState;

/**
* Configuring Timer Overflow Mode.
 */
typedef enum {
    QDEC_TMR_RELOAD = 0,
    QDEC_TMR_STOP,
}QDEC_TMR_UP_MODE;

typedef enum {
    QDEC_TMR_RELOAD_UP_FULL = 0,
    QDEC_TMR_RELOAD_UP_VALE,
}QDEC_TMR_RELOAD_MODE;

/**  Qdec Channel x(channel number) output config.
 * - QDEC_CHX_ACPA: Load config Capture Compare register a Timer input output A
 * - QDEC_CHX_ACPC: Load config Capture Compare register c Timer input output A
 * - QDEC_CHX_BCPB: Load config Capture Compare register b Timer input output B
 * - QDEC_CHX_BCPC: Load config Capture Compare register c Timer input output B
*/
typedef enum {
    QDEC_CHX_ACPA = 0,
    QDEC_CHX_ACPC,
    QDEC_CHX_BCPB,
    QDEC_CHX_BCPC,
}QDEC_OUTX_Config;
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
 * @brief QDEC standard configuration interface
 *
 * @param[in] filter         QDEC signal filter value(0-63)
 * @param[in] miss           max value of QDEC miss signal(0-15)
 */
void QDEC_QdecCfg(uint8_t filter, uint8_t miss);

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

/**
 * @brief Enable QDEC other interrupt
 * @param[in] mask     mask of interrupts(0x1 enable dir changed int)
 *
 */
void QDEC_EnableInt(uint8_t mask);

/**
 * @brief Enable QDEC rising and falling edge trigger
 *
 * @param[in] enable             enable/disable
 */
void QDEC_EnableDoubleEdge(uint8_t enable);

/**
 * @brief Set qdec channel mode.
 * @param[in] Channel         QDEC channel
 * @param[in] ModeCfg         qdec mode,can used timer,pacp,pwm,qdec.
 *
 */
void QDEC_ChModeCfg(QDEC_CHX Channel, QDEC_ModCfg ModeCfg);

/**
 * @brief compare with RC trigger enable
 * @param[in] Channel         QDEC channel
 * @param[in] enable             enable/disable
 *
 */
void QDEC_SetChxCpcTrg(QDEC_CHX Channel, uint8_t enable);

/**
 * @brief capture stop
 * @param[in] Channel         QDEC channel
 * @param[in] enable             enable/disable
 *
 */
void QDEC_SetChxCpcStopEn(QDEC_CHX Channel, uint8_t enable);

/**
 * @brief Set trigger edge
 * @param[in] Channel         QDEC channel
 * @param[in] edge            external trigger edge
 *
 */
void QDEC_ExternalEventEdgeSet(QDEC_CHX Channel, QDEC_ExTrigger edge);

/**
 * @brief Set external trigger edge
 * @param[in] Channel         QDEC channel
 * @param[in] OutxChannal     Qdec output channel config
 * @param[in] edge            external trigger edge
 *
 */
void QDEC_SetOutxEdge(QDEC_CHX Channel,QDEC_OUTX_Config OutxChannal, QDEC_ExTrigger edge);

/**
 * @brief Set external trigger edge mode
 * @param[in] Channel         QDEC channel
 * @param[in] val             external trigger edge, input from QDEC_ExTrigger.
 *
 */
void QDEC_SetExternalTrigger(QDEC_CHX Channel, uint8_t val);

/**
 * @brief Enable external trigger edge
 * @param[in] Channel         QDEC channel
 * @param[in] enable          enable/disable
 *
 */
void QDEC_SetExternalTriggerEn(QDEC_CHX Channel, uint8_t enable);

/**
 * @brief Set tmr cnt
 * @param[in] Channel         QDEC channel
 * @param[in] val             cnt
 *
 */
void QDEC_SetCHxTmrCntA(QDEC_CHX Channel, uint16_t val);
void QDEC_SetCHxTmrCntB(QDEC_CHX Channel, uint16_t val);
void QDEC_SetCHxTmrCntC(QDEC_CHX Channel, uint16_t val);

/**
 * @brief config tmr
 * @param[in] Channel         QDEC channel
 * @param[in] TmrUpMode       up to cnt mode
 * @param[in] TmrReloadMode   reload mode
 *
 */
void QDEC_TmrCfg(QDEC_CHX Channel, QDEC_TMR_UP_MODE TmrUpMode, QDEC_TMR_RELOAD_MODE TmrReloadMode);

/**
 * @brief enable channel Int
 * @param[in] Channel         QDEC channel
 * @param[in] enable          enable/disable
 * @param[in] items           int items
 *
 */
void QDEC_SetChxIntEn(QDEC_CHX Channel, uint8_t enable, uint16_t items);

/**
 * @brief get Int enable state
 * @param[in] Channel         QDEC channel
 * @return                   Int enable items
 *
 */
uint32_t QDEC_GetChxIntEn(QDEC_CHX Channel);

/**
 * @brief get Int state and clear it.
 * @param[in] Channel         QDEC channel
 * @return                   Int state
 *
 */
uint32_t QDEC_GetChxIntStateClr(QDEC_CHX Channel);

/**
 * @brief Enable qdec channel.
 * @param[in] Channel         QDEC channel
 * @param[in] enable          enable/disable
 *
 */
void QDEC_EnableChannel(QDEC_CHX Channel, uint8_t enable);

/**
 * @brief Get qdec mode enable state.
 * @return                   Enable state
 *
 */
uint8_t QDEC_GetQdecModeEn(void);

/**
 * @brief Get qdec channel register a/b cnt
 * @param[in] Channel         QDEC channel
 * @return                   rab cnt
 *
 */
uint32_t QDEC_GetChxReadRab(QDEC_CHX Channel);

/**
 * @brief Enable Qdec dma.
 * @param[in] Channel         QDEC channel
 * @param[in] enable_a        enable/disable
 * @param[in] enable_b        enable/disable
 */
void QDEC_EnableDMAChAB(QDEC_CHX Channel, uint8_t enable_a,uint8_t enable_b);

/**
 * @brief Get dma enable state.
 * @param[in] Channel         QDEC channel
 * @return                   enable state.
 */
uint8_t QDEC_GetDMAChAB(QDEC_CHX Channel);


#endif

#ifdef __cplusplus
}
#endif

#endif
