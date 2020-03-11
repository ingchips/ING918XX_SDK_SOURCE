#ifndef __PERIPHERAL_PWM_H__
#define __PERIPHERAL_PWM_H__

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#ifdef TARGET_FPGA
#define PWM_CLOCK_FREQ  16000000UL
#else
#define PWM_CLOCK_FREQ  24000000UL
#endif

#define PWM_CHANNEL_NUMBER      6

typedef enum
{
    PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE          = 0x0,
    PWM_WORK_MODE_UP_WITH_DIED_ZONE             = 0x1,
    PWM_WORK_MODE_UPDOWN_WITHOUT_DIED_ZONE      = 0x2,
    PWM_WORK_MODE_UPDOWN_WITH_DIED_ZONE         = 0x3,
    PWM_WORK_MODE_SINGLE_WITHOUT_DIED_ZONE      = 0x4
} PWM_WordMode_t;

void PWM_Enable(const uint8_t channel_index, const uint8_t enable);

void PWM_SetMask(const uint8_t channel_index, const uint8_t mask_a, const uint8_t mask_b);

void PWM_SetMode(const uint8_t channel_index, const PWM_WordMode_t mode);

// PMW Halt Mode
// When the register HALT_ENABLE is set to 1, PWM will controlled by HALT_CONFIG registers.
// The PWM_OUT_A will output the value of HALT_CONFIG[0], and the PWM_OUT_B will output the value of HALT_CONFIG[1]
void PWM_HaltCtrlEnable(const uint8_t channel_index, const uint8_t enable);
void PWM_HaltCtrlCfg(const uint8_t channel_index, const uint8_t out_a, const uint8_t out_b);

// The PWM waveform can be set to multi-duty-cycle if it's not in single step mode or halt mode
// comp_num is in [0..3], which means [1..4] duty-cycles are used (\ref PWM_SetHighThreshold)
void PWM_SetMultiDutyCycleCtrl(const uint8_t channel_index, const uint8_t comp_num);

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

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

