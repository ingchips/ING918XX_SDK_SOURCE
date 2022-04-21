#ifndef __PERIPHERAL_PWM_H__
#define __PERIPHERAL_PWM_H__

#include <stdint.h>
#include "ingsoc.h"
#include "peripheral_sysctrl.h"

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#define PWM_CLOCK_FREQ          OSC_CLK_FREQ

#define PWM_CHANNEL_NUMBER      6

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#define PWM_CLOCK_FREQ          SYSCTRL_GetClk(SYSCTRL_ITEM_APB_PWM)

#define PWM_CHANNEL_NUMBER      1

#endif

typedef enum
{
    PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE          = 0x0,
    PWM_WORK_MODE_UP_WITH_DIED_ZONE             = 0x1,
    PWM_WORK_MODE_UPDOWN_WITHOUT_DIED_ZONE      = 0x2,
    PWM_WORK_MODE_UPDOWN_WITH_DIED_ZONE         = 0x3,
    PWM_WORK_MODE_SINGLE_WITHOUT_DIED_ZONE      = 0x4,
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    PWM_WORK_MODE_DMA                           = 0x5,
#endif
} PWM_WordMode_t;

void PWM_Enable(const uint8_t channel_index, const uint8_t enable);
void PWM_EnableMulti(uint32_t channel_mask, const uint8_t enable);

void PWM_SetMask(const uint8_t channel_index, const uint8_t mask_a, const uint8_t mask_b);

void PWM_SetMode(const uint8_t channel_index, const PWM_WordMode_t mode);

// PMW Halt Mode
// When the register HALT_ENABLE is set to 1, PWM will controlled by HALT_CONFIG registers.
// The PWM_OUT_A will output the value of HALT_CONFIG[0], and the PWM_OUT_B will output the value of HALT_CONFIG[1]
void PWM_HaltCtrlEnable(const uint8_t channel_index, const uint8_t enable);
void PWM_HaltCtrlCfg(const uint8_t channel_index, const uint8_t out_a, const uint8_t out_b);

// thresholds
void PWM_SetPeraThreshold(const uint8_t channel_index, const uint32_t threshold);
void PWM_SetDiedZoneThreshold(const uint8_t channel_index, const uint32_t threshold);
// multi_duty_index is in [0..3]
void PWM_SetHighThreshold(const uint8_t channel_index, const uint8_t multi_duty_index, const uint32_t threshold);

// Below is an example:
// a PWM channel is set up to have a specified `frequency`, and `on_dudy` in each cycle (on out_a)
// `frequency` is in Hz
// `on_duty` is in percent, i.e. is in [0..100]
void PWM_SetupSimple(const uint8_t channel_index, const uint32_t frequency, const uint16_t on_duty);

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

// The PWM waveform can be set to multi-duty-cycle if it's not in single step mode or halt mode
// comp_num is in [0..3], which means [1..4] duty-cycles are used (\ref PWM_SetHighThreshold)
void PWM_SetMultiDutyCycleCtrl(const uint8_t channel_index, const uint8_t comp_num);

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

/**
 * @brief Control output phase
 *
 * @param[in] channel_index     channel index (must be 0)
 * @param[in] inv_a             invert output A (1) or not (0)
 * @param[in] inv_b             invert output B (1) or not (0)
 */
void PWM_SetInvertOutput(const uint8_t channel_index, const uint8_t inv_a, const uint8_t inv_b);

/**
 * @brief Enable/Disable DMA transfer
 *
 * @param[in] channel_index     channel index (must be 0)
 * @param[in] trig_cfg          DMA trigger configure
 * @param[in] enable            enable(1)/disable(0)
 */
void PWM_DmaEnable(const uint8_t channel_index, uint8_t trig_cfg, uint8_t enable);

#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

