
#ifndef __PERIPHERAL_TIMER_H__
#define __PERIPHERAL_TIMER_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <math.h>
#include "ingsoc.h"

// we have 3 timers in total.
// For ING918xx: Timer 0 can be used as watchdog.
// For ING916xx: There is a dedicated watchdog timer.
#define TIMER_NUMBER                        3

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#define TMR_CLK_FREQ                        OSC_CLK_FREQ

/**
 ****************************************************************************************
 * @brief Get counter of a timer
 *
 * @param[in] pTMR          timer address
 * @return                  counter
 ****************************************************************************************
 */
uint32_t TMR_GetCNT(TMR_TypeDef *pTMR);

/**
 ****************************************************************************************
 * @brief Reload a timer: set its counter to zero
 *
 * @param[in] pTMR          timer address
 ****************************************************************************************
 */
void TMR_Reload(TMR_TypeDef *pTMR);

//-----------
// TMR_CMP
//
void TMR_SetCMP(TMR_TypeDef *pTMR, uint32_t value);

/**
 ****************************************************************************************
 * @brief Set comparison value of a timer
 *
 * @param[in] pTMR          timer address
 * @param[in] value         comparison value
 ****************************************************************************************
 */
uint32_t TMR_GetCMP(TMR_TypeDef *pTMR);

/**
 ****************************************************************************************
 * @brief Enable a timer
 *
 * @param[in] pTMR          timer address
 ****************************************************************************************
 */
void TMR_Enable(TMR_TypeDef *pTMR);

/**
 ****************************************************************************************
 * @brief Disable a timer
 *
 * @param[in] pTMR          timer address
 ****************************************************************************************
 */
void TMR_Disable(TMR_TypeDef *pTMR);

// timer work mode
#define TMR_CTL_OP_MODE_WRAPPING            0            // 0 - continuous wrapping mode
#define TMR_CTL_OP_MODE_ONESHOT             1            // 1 - one-stop mode
#define TMR_CTL_OP_MODE_FREERUN             2            // 2 - continuous free-run mode

/**
 ****************************************************************************************
 * @brief Set work mode of a timer
 *
 * @param[in] pTMR          timer address
 * @param[in] mode          work mode
 ****************************************************************************************
 */
void TMR_SetOpMode(TMR_TypeDef *pTMR, uint8_t mode);

/**
 ****************************************************************************************
 * @brief Enable interrupt of a timer
 *
 * @param[in] pTMR          timer address
 ****************************************************************************************
 */
void TMR_IntEnable(TMR_TypeDef *pTMR);

/**
 ****************************************************************************************
 * @brief Disable interrupt of a timer
 *
 * @param[in] pTMR          timer address
 ****************************************************************************************
 */
void TMR_IntDisable(TMR_TypeDef *pTMR);

/**
 ****************************************************************************************
 * @brief Clear interrupt request of a timer
 *
 * @param[in] pTMR          timer address
 ****************************************************************************************
 */
void TMR_IntClr(TMR_TypeDef *pTMR);

/**
 ****************************************************************************************
 * @brief Get interrupt status of a timer
 *
 * @param[in] pTMR          timer address
 ****************************************************************************************
 */
uint8_t TMR_IntHappened(TMR_TypeDef *pTMR);

/**
 ****************************************************************************************
 * @brief Enable watchdog.
 *        Watchdog timed out after its counter counts to `timeout` for 2 times,
 *        i.e., if `timeout` == TMR_CLK_FREQ (aka 1s), watchdog timed out after 2sec.
 *
 * @param[in] timeout    timeout counter.
 ****************************************************************************************
 */
void TMR_WatchDogEnable(uint32_t timeout);

/**
 ****************************************************************************************
 * @brief Disable watchdog.
 ****************************************************************************************
 */
void TMR_WatchDogDisable(void);

/**
 ****************************************************************************************
 * @brief Watchdog restart (aka feeding)
 ****************************************************************************************
 */
#define TMR_WatchDogRestart()      do { TMR0_UNLOCK(); TMR_Reload(APB_TMR0); TMR0_LOCK(); } while (0)

//-----------
// TMR_LOCK
//
void TMR0_LOCK(void);
void TMR0_UNLOCK(void);

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

typedef enum
{
    WDT_INTTIME_INTERVAL_0,       //Clock period × 2^6
    WDT_INTTIME_INTERVAL_1,       //Clock period × 2^8
    WDT_INTTIME_INTERVAL_2,       //Clock period × 2^10
    WDT_INTTIME_INTERVAL_3,       //Clock period × 2^11
    WDT_INTTIME_INTERVAL_4,       //Clock period × 2^12
    WDT_INTTIME_INTERVAL_5,       //Clock period × 2^13
    WDT_INTTIME_INTERVAL_6,       //Clock period × 2^14
    WDT_INTTIME_INTERVAL_7,       //Clock period × 2^15
    WDT_INTTIME_INTERVAL_8,       //Clock period × 2^17
    WDT_INTTIME_INTERVAL_9,       //Clock period × 2^19
    WDT_INTTIME_INTERVAL_10,      //Clock period × 2^21
    WDT_INTTIME_INTERVAL_11,      //Clock period × 2^23
    WDT_INTTIME_INTERVAL_12,      //Clock period × 2^25
    WDT_INTTIME_INTERVAL_13,      //Clock period × 2^27
    WDT_INTTIME_INTERVAL_14,      //Clock period × 2^29
    WDT_INTTIME_INTERVAL_15       //Clock period × 2^31
}wdt_inttime_interval_t;

typedef enum
{
    WDT_RSTTIME_INTERVAL_0,        //Clock period × 2^7
    WDT_RSTTIME_INTERVAL_1,        //Clock period × 2^8
    WDT_RSTTIME_INTERVAL_2,        //Clock period × 2^9
    WDT_RSTTIME_INTERVAL_3,        //Clock period × 2^10
    WDT_RSTTIME_INTERVAL_4,        //Clock period × 2^11
    WDT_RSTTIME_INTERVAL_5,        //Clock period × 2^12
    WDT_RSTTIME_INTERVAL_6,        //Clock period × 2^13
    WDT_RSTTIME_INTERVAL_7         //Clock period × 2^14
}wdt_rsttime_interval_t;


// timer work mode
#define TMR_CTL_OP_MODE_32BIT_TIMER_x1            1           // one 32bit timer
#define TMR_CTL_OP_MODE_16BIT_TIMER_x2            2           // dual 16bit timers
#define TMR_CTL_OP_MODE_8BIT_TIMER_x4             3           // four 8bit timers
#define TMR_CTL_OP_MODE_16BIT_PWM                 4           // PWM with two 16bit counters
#define TMR_CTL_OP_MODE_8BIT_PWM_16BIT_TIMER_x1   6           // MIXED: PWM with two 8bit counters + one 16bit timer
#define TMR_CTL_OP_MODE_8BIT_PWM_8BIT_TIMER_x2    7           // MIXED: PWM with two 8bit counters + dual 8bit timer

#define TMR_CLK_MODE_EXTERNAL                     0
#define TMR_CLK_MODE_APB                          1

/**
 ****************************************************************************************
 * @brief Set work mode of a timer channel
 *
 * @param[in] pTMR              timer peripheral address
 * @param[in] ch_id             channel ID
 * @param[in] op_mode           work mode (see `TMR_CTL_OP_MODE_...`)
 * @param[in] clk_mode          clock mode (see `TMR_CLK_MODE_...`)
 * @param[in] pwm_park_value    PWM park value ({0, 1})
 ****************************************************************************************
 */
void TMR_SetOpMode(TMR_TypeDef *pTMR, uint8_t ch_id, uint8_t op_mode, uint8_t clk_mode, uint8_t pwm_park_value);

/**
 ****************************************************************************************
 * @brief Get counter clock frequency of a timer channel
 *
 * @param[in] pTMR              timer peripheral address
 * @param[in] ch_id             channel ID
 * @return                      counter clock frequency in Hz
 ****************************************************************************************
 */
uint32_t TMR_GetClk(TMR_TypeDef *pTMR, uint8_t ch_id);

/**
 ****************************************************************************************
 * @brief Set reload value of a timer
 *
 * @param[in] pTMR              timer peripheral address
 * @param[in] ch_id             channel ID
 * @param[in] value             reload value
 * Note on reload value:
 *   - For TMR_CTL_OP_MODE_32BIT_TIMER_x1: for 32-bit Timer 0
 *   - For TMR_CTL_OP_MODE_16BIT_TIMER_x2:
 *      * lower 16-bit [0..15] for Timer 0
 *      * upper 16-bit [16..31] for Timer 1
 *   - For TMR_CTL_OP_MODE_8BIT_TIMER_x4:
 *      * bits [0..7] for Timer 0
 *      * bits [8..15] for Timer 1
 *      * bits [16..23] for Timer 2
 *      * bits [24..31] for Timer 3
 *   - For TMR_CTL_OP_MODE_16BIT_PWM:
 *      * lower 16-bit [0..15] for PWM low period
 *      * upper 16-bit [16..31] for PWM high period
 *   - For TMR_CTL_OP_MODE_8BIT_PWM_16BIT_TIMER_x1:
 *      * bits [0..15] for Timer 0
 *      * bits [16..23] for PWM low period
 *      * bits [24..31] for PWM high period
 *   - For TMR_CTL_OP_MODE_8BIT_PWM_8BIT_TIMER_x2:
 *      * bits [0..7] for Timer 0
 *      * bits [8..15] for Timer 1
 *      * bits [16..23] for low period
 *      * bits [24..31] for high period
 ****************************************************************************************
 */
void TMR_SetReload(TMR_TypeDef *pTMR, uint8_t ch_id, uint32_t value);

/**
 ****************************************************************************************
 * @brief Enable a timer
 *
 * @param[in] pTMR              timer peripheral address
 * @param[in] ch_id             channel ID
 * @param[in] mask              enable or disable a timer in a channel
 * Note:
 *   - PWM is always identified as the 3rd timer (counting from 0).
 ****************************************************************************************
 */
void TMR_Enable(TMR_TypeDef *pTMR, uint8_t ch_id, uint8_t mask);

/**
 ****************************************************************************************
 * @brief Get comparison value (counter value) of a timer
 *
 * @param[in] pTMR              timer peripheral address
 * @param[in] ch_id             channel ID
 * @return    Current comparison value
 ****************************************************************************************
 */
uint32_t TMR_GetCMP(TMR_TypeDef *pTMR, uint8_t ch_id);

/**
 ****************************************************************************************
 * @brief Enable interrupts of a timer
 *
 * @param[in] pTMR              timer peripheral address
 * @param[in] ch_id             channel ID
 * @param[in] mask              interrupt mask (max. four interrupts per channel)
 * Note: When a bit in `mask` is zero, the corresponding interrupt is disabled.
 ****************************************************************************************
 */
void TMR_IntEnable(TMR_TypeDef *pTMR, uint8_t ch_id, uint8_t mask);

/**
 ****************************************************************************************
 * @brief Clear interrupt request of a timer
 *
 * @param[in] pTMR              timer peripheral address
 * @param[in] ch_id             channel ID
 * @param[in] mask              mask of interrupts to be cleared
 ****************************************************************************************
 */
void TMR_IntClr(TMR_TypeDef *pTMR, uint8_t ch_id, uint8_t mask);

/**
 ****************************************************************************************
 * @brief Get interrupt status of a timer
 *
 * @param[in] pTMR              timer peripheral address
 * @param[in] ch_id             channel ID
 ****************************************************************************************
 */
uint8_t TMR_IntHappened(TMR_TypeDef *pTMR, uint8_t ch_id);

/**
 ****************************************************************************************
 * @brief Enable watchdog.
 *
 * The watchdog timer provides a two-stage mechanism to prevent a system from lock-up.
 *
 * In the first stage ("interrupt stage"), an interrupt will be asserted after `int_timeout_ms`.
 * The second stage ("reset stage") begins right after the interrupt stage, and the reset signal
 * will be asserted after `reset_timeout_ms`.
 *
 * Note: The internal counter runs at `RTC_CLOCK_FREQ`, and the actual timeouts are rough appropriations
 * of the specified timeout value.
 *
 * @param[in] int_timeout           timeout interval of "interrupt stage" in `wdt_inttime_interval_t`
 * @param[in] rst_timeout           timeout interval of "reset stage" in `wdt_rsttime_interval_t`
 * @param[in] enable_int            enable interrupt in "interrupt stage"
 ****************************************************************************************
 */
void TMR_WatchDogEnable3(wdt_inttime_interval_t int_timeout, wdt_rsttime_interval_t rst_timeout, uint8_t enable_int);

/**
 ****************************************************************************************
 * @brief Enable watchdog.
 *
 * This macro tries to provide similar interface as the one in ING918xx.
 *
 * Please use `TMR_WatchDogEnable3` instead for ING916xx.
 *
 * @param[in] timeout              see `TMR_WatchDogEnable` in ING918xx
 ****************************************************************************************
 */
#define TMR_WatchDogEnable(timeout) do { uint32_t TMR_CLK_FREQ = OSC_CLK_FREQ;uint32_t cnt = (uint64_t)(timeout) / OSC_CLK_FREQ;  \
                                            uint32_t mode = 7 + (uint32_t)sqrt((double)cnt) / 2;TMR_WatchDogEnable3(mode, WDT_RSTTIME_MODE_7, 1); } while (0)   \

/**
 ****************************************************************************************
 * @brief Watchdog restart (aka feeding)
 ****************************************************************************************
 */
void TMR_WatchDogRestart(void);

/**
 ****************************************************************************************
 * @brief Clear watchdog interrupt
 ****************************************************************************************
 */
void TMR_WatchDogClearInt(void);

/**
 ****************************************************************************************
 * @brief Disable watchdog.
 ****************************************************************************************
 */
void TMR_WatchDogDisable(void);

#endif

#ifdef __cplusplus
  }
#endif

#endif // __CM32GPM0_UART_H__
