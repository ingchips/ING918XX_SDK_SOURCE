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

#define PWM_CHANNEL_NUMBER      3

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

#define PWM_CLOCK_FREQ          SYSCTRL_GetClk(SYSCTRL_ITEM_APB_PWM)

#define PWM_CHANNEL_NUMBER      3

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
    PWM_WORK_MODE_PCAP                          = 0x6
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
    PWM_WORK_MODE_DMA                           = 0x5,
    PWM_WORK_MODE_PCAP                          = 0x6,
    PWM_WORK_MODE_IR                            = 0x7
#endif
} PWM_WorkMode_t;

typedef enum
{
    PWM_FIFO_CLEAR           = (1 << 15),
    PWM_FIFO_EMPTY_EN        = (1 << 16),
    PWM_FIFO_FULL_EN         = (1 << 17),
    PWM_FIFO_HALFFULL_EN     = (1 << 18),
    PWM_FIFO_TRIGGER_EN      = (1 << 19)
} PWM_FIFO_MASK_t;

typedef struct
{
    uint32_t fifo_empty     :1;
    uint32_t fifo_full      :1;
    uint32_t fifo_halffull  :1;
    uint32_t fifo_trig      :1;
    uint32_t fifo_cnt       :3;
    uint32_t unused         :25;
} PWM_FIFO_STATUS_t;

// compatibility for the typo
#define PWM_WordMode_t  PWM_WorkMode_t

void PWM_Enable(const uint8_t channel_index, const uint8_t enable);
void PWM_EnableMulti(uint32_t channel_mask, const uint8_t enable);

void PWM_SetMask(const uint8_t channel_index, const uint8_t mask_a, const uint8_t mask_b);

void PWM_SetMode(const uint8_t channel_index, const PWM_WorkMode_t mode);

PWM_WorkMode_t PWM_GetMode(const uint8_t channel_index);

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

//for Single Step Mode:
//generate one pulse
// `pulse_width` is in ns
void PWM_SetupSingle(const uint8_t channel_index, const uint32_t pulse_width);

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

// The PWM waveform can be set to multi-duty-cycle if it's not in single step mode or halt mode
// comp_num is in [0..3], which means [1..4] duty-cycles are used (\ref PWM_SetHighThreshold)
void PWM_SetMultiDutyCycleCtrl(const uint8_t channel_index, const uint8_t comp_num);

#elif ((INGCHIPS_FAMILY == INGCHIPS_FAMILY_916) || (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20))

/**
 * @brief Enable PWM halt control
 *
 * When the register HALT_ENABLE is set to 1 for an output,
 * PWM will controlled by HALT_CONFIG registers.
 *
 * Note: `PWM_HaltCtrlEnable(channel, enable)` is an short-cut for
 * `PWM_HaltCtrlEnable2(channel, enable, enable)`.
 *
 * @param[in] channel_index     channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @param[in] enable_a          enable halt control on out A
 * @param[in] enable_a          enable halt control on out B
*/
void PWM_HaltCtrlEnable2(const uint8_t channel_index, const uint8_t enable_a, const uint8_t enable_b);

/**
 * @brief Control output phase
 *
 * @param[in] channel_index     channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @param[in] inv_a             invert output A (1) or not (0)
 * @param[in] inv_b             invert output B (1) or not (0)
 */
void PWM_SetInvertOutput(const uint8_t channel_index, const uint8_t inv_a, const uint8_t inv_b);

/**
 * @brief Enable/Disable DMA transfer
 *
 * @param[in] channel_index     channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @param[in] trig_cfg          DMA trigger configure (0..7)
 *                              PCAP mode: DMA triggered when >= trig_cfg
 *                              Other modes: DMA triggered when < trig_cfg
 * @param[in] enable            enable(1)/disable(0)
 */
void PWM_DmaEnable(const uint8_t channel_index, uint8_t trig_cfg, uint8_t enable);

enum PCAP_PULSE_EVENT
{
    PCAP_PULSE_RISING_EDGE  = 0x1,
    PCAP_PULSE_FALLING_EDGE = 0x2,
};

/**
 * @brief Enable detection of events on each inputs
 *
 * `events_on_x` are bit combinations of `PCAP_PULSE_EVENT`, such as,
 * `PCAP_PULSE_RISING_EDGE | PCAP_PULSE_FALLING_EDGE`.
 *
 * @param[in] channel_index     channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @param[in] events_on_0       events to be detected and reported on input 0
 * @param[in] events_on_1       events to be detected and reported on input 1
 */
void PCAP_EnableEvents(const uint8_t channel_index,
                       uint8_t events_on_0,
                       uint8_t events_on_1);

/**
 * @brief Enable/Disable PCAP
 *
 * @param[in] channel_index     channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 */
void PCAP_Enable(const uint8_t channel_index);

/**
 * @brief Enable/Disable DMA transfer
 *
 * see `PWM_DmaEnable`
 */
#define PCAP_DmaEnable PWM_DmaEnable

/**
 * @brief Enable/Disable PCAP counter
 *
 * All PCAP channels use the same counter.
 *
 * @param[in] enable        Enable (1) or disable (0)
 */
void PCAP_CounterEnable(uint8_t enable);

/**
 * @brief Clear PCAP FIFO
 *
 * @param[in] channel_index     channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 */
void PCAP_ClearFifo(uint8_t channel_index);

/**
 * @brief Read a PCAP data
 *
 * PCAP output are generally re-directed to DMA, this function may be used for
 * debugging.
 *
 * ```c
 *     struct data0
 *     {
 *         uint32_t cnt_high:12;
 *         uint32_t p_cap_0_p:1; // rising  edge on input 0 (A) is detected
 *         uint32_t p_cap_0_n:1; // falling edge on input 0 (A) is detected
 *         uint32_t p_cap_1_p:1; // rising  edge on input 1 (B) is detected
 *         uint32_t p_cap_1_n:1; // falling edge on input 1 (B) is detected
 *         uint32_t tag:4;
 *         uint32_t padding:12;
 *     };
 *     struct data1
 *     {
 *         uint32_t cnt_low:20;
 *         uint32_t padding:12;
 *     };
 * ```
 * @param[in] channel_index     channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @return                      data0 for the (0/2/4/..)th reading;
 *                              data1 for the (1/3/5/..)th reading.
 */
uint32_t PCAP_ReadData(const uint8_t channel_index);

/**
 * @brief Read the PCAP counter
 *
 * @return  counter value
 */
uint32_t PCAP_ReadCounter(void);

/**
 * @brief Enable/Disable PCAP FIFO interrupt requests
 *
 * Note: This is not needed for DMA mode.
 *
 * @param[in] channel_index     channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @param[in] enable            Enable (1) or disable (0)
 * @param[in] mask              combination of interrupt masks (see `PWM_FIFO_MASK_t`)
 *                              for example: `PWM_FIFO_HALFFULL_EN | PWM_FIFO_TRIGGER_EN`
 */
void PWM_FifoIntEnable(const uint8_t channel_index, uint8_t enable, uint32_t mask);

// `PWM_FifoTriggerEnable` is obsoleted.
#define PWM_FifoTriggerEnable   PWM_FifoIntEnable
#define PCAP_FifoIntEnable      PWM_FifoIntEnable

/**
 * @brief Set trigger level for PWM/PCAP for FIFO TRIGGER interrupt
 *
 * See also `PWM_FIFO_TRIGGER_EN`.
 *
 * Note: This is not needed for DMA mode.
 *
 * @param[in] channel_index     channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @param[in] trig_cfg          trigger level (0 .. 7), TRIGGER interrupt is generated when
 *                              - for PCAP: fifo_cnt >= trig_cfg
 *                              - for PWM: fifo_cnt < trig_cfg
 */
void PWM_SetIntTrigLevel(const uint8_t channel_index, const uint8_t trig_cfg);

#define PCAP_SetIntTrigLevel    PWM_SetIntTrigLevel

/**
 * @brief Get fifo status in fifo isr.
 *
 * @param[in] channel_index     channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @return                      use structure PWM_FIFO_STATUS_t to interpret the return value
 */
uint32_t PWM_GetFifoStatus(const uint8_t channel_index);


#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
/**
 * @brief The PWM step mode.
 *
 * @param[in] channel           channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @param[in] enable            Enable (1) or disable (0)
 */
void PWM_StepEnabled(const uint8_t channel,uint8_t enable);
/**
 * @brief The PWM step loop mode.
 *
 * @param[in] channel           channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @param[in] enable            Enable (1) or disable (0)
 */
void PWM_StepLoopEnabled(const uint8_t channel,uint8_t enable);
/**
 * @brief Step value of each period in PWM step mode.
 *
 * @param[in] channel           channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @param[in] cnt               Step value of each period
 */
void PWM_SetStepCnt(const uint8_t channel,uint32_t cnt);
/**
 * @brief Step target in PWM IR mode.
 *
 * @param[in] channel           channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @param[in] target            target vale
 */
void PWM_SetStepTarget(const uint8_t channel,uint32_t target);
/**
 * @brief Set frequency and duty cycle in IR cycle mode
 *
 * @param[in] channel           channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @param[in] carry_channel     Cycle register sequence number.
 * @param[in] frequency         set ferquency
 * @param[in] duty              set duty
 */
void IR_CycleCarrierSetup(uint8_t channel, uint8_t carry_channel, uint32_t frequency, uint32_t duty);
/**
 * @brief Set IR mode dma register vale
 *
 * @param[in] channel           channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @param[in] carry_channel     Cycle mode set traget and count.
 */
void IR_WriteCarrierData(uint8_t channel, uint32_t data);
/**
 * @brief Set IR mode busy state
 *
 * @return                      IR busy state.
 */
uint8_t IR_BusyState(uint8_t channel);
#endif

#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

