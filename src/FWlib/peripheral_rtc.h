#ifndef __PERIPHERAL_RTC_H__
#define __PERIPHERAL_RTC_H__

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include <stdint.h>
#include "ingsoc.h"

/**
 * @brief Driver for RTC
 *
 * @section About software RTC
 *
 * A software implementation emulating day/hour/minute/second real-time clock as
 * in ING916 is also provided, which can be enabled by compiling switch `SOFTWARE_RTC_DHMS`.
 *
 * For ING916, when `SOFTWARE_RTC_DHMS` is defined, the hardware based one is not used
 * any more.
 *
 * Differences comparing to hardware implementation:
 *
 * 1. All API are only allowed to be used after the stack is initialized, except
 *      `RTC_SoftSetISR`;
 *
 * 1. ISR should be registered by `RTC_SoftSetISR` instead of platform API;
 *
 * 1. Do not need to be trimmed;
 *
 * 1. This is based on platform timer API, so "jitter" is larger.
 */

/**
 ****************************************************************************************
 * @brief Get current RTC value (lower 32bit)
 *
 * @return              Current value
 ****************************************************************************************
 */
uint32_t RTC_Current(void);

/**
 ****************************************************************************************
 * @brief Get current full RTC value (full 43bit)
 *
 * @return              Current value
 ****************************************************************************************
 */
uint64_t RTC_CurrentFull(void);

/**
 ****************************************************************************************
 * @brief Enable/Disable
 *
 * Enabled by default. Disabled to save power.
 *
 * @param[in] enable            Enable(1)/Disable(0)
 ****************************************************************************************
 */
void RTC_EnableFreeRun(uint8_t enable);

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#define RTC_ENABLED             1
#define RTC_DISABLED            0

/**
 ****************************************************************************************
 * @brief Enable RTC module
 *
 * @param[in] flag     RTC_ENABLED or RTC_DISABLED
 ****************************************************************************************
 */
#define RTC_Enable(const uint8_t flag)  RTC_EnableFreeRun(flag)

/**
 ****************************************************************************************
 * @brief Set timing offset (number of cycles) of next RTC interrupt
 *
 * @param[in] offset    Offset in cycles
 ****************************************************************************************
 */
void RTC_SetNextIntOffset(const uint32_t offset);

/**
 ****************************************************************************************
 * @brief Clear RTC interrupt status
 ****************************************************************************************
 */
void RTC_ClearInt(void);

#ifdef SOFTWARE_RTC_DHMS

typedef enum
{
    RTC_IRQ_ALARM = 0x04,
    RTC_IRQ_DAY = 0x08,
    RTC_IRQ_HOUR = 0x10,
    RTC_IRQ_MINUTE = 0x20,
    RTC_IRQ_SECOND = 0x40,
    RTC_IRQ_HALF_SECOND = 0x80,
} rtc_irq_t;

/**
 ****************************************************************************************
 * @brief Get current date time
 *
 *
 * @param[out] hour             Hour (0..23)
 * @param[out] minute           Minute (0..59)
 * @param[out] second           Second (0..59)
 * @return                      Days passed (5 bits, 0..31)
 ****************************************************************************************
 */
uint16_t RTC_GetTime(uint8_t *hour, uint8_t *minute, uint8_t *second);

/**
 ****************************************************************************************
 * @brief Modify date time
 *
 * @param[in] day               days passed (0..31)
 * @param[in] hour              Hour (0..23)
 * @param[in] minute            Minute (0..59)
 * @param[in] second            Second (0..59)
 ****************************************************************************************
 */
void RTC_ModifyTime(uint16_t day, uint8_t hour, uint8_t minute, uint8_t second);

/**
 ****************************************************************************************
 * @brief Check if RTC modification is done or not
 *
 * This indicator becomes zero when any of RTC control registers (the Counter, Alarm and
 * Control registers) are updated, and returned to `1` after modications are synchronized
 * to the much slower RTC clock domain.
 *
 * @return                      if RTC modification is done (non-0) or not (0)
 ****************************************************************************************
 */
int RTC_IsModificationDone(void);

/**
 ****************************************************************************************
 * @brief Config alarm
 *
 * @param[in] hour              Hour (0..23)
 * @param[in] minute            Minute (0..59)
 * @param[in] second            Second (0..59)
 ****************************************************************************************
 */
void RTC_ConfigAlarm(uint8_t hour, uint8_t minute, uint8_t second);

/**
 ****************************************************************************************
 * @brief Enable/Disable IRQs
 *
 * An IQR is disabled if not in `mask`.
 *
 * @param[in] mask              combination of `rtc_irq_t`
 ****************************************************************************************
 */
void RTC_EnableIRQ(uint32_t mask);

/**
 ****************************************************************************************
 * @brief Get IRQs state
 *
 * @return                      combination of `rtc_irq_t`
 ****************************************************************************************
 */
uint32_t RTC_GetIntState(void);

/**
 ****************************************************************************************
 * @brief Clear IRQs state
 *
 * @param[in] state             combination of `rtc_irq_t`
 ****************************************************************************************
 */
void RTC_ClearIntState(uint32_t state);

/**
 ****************************************************************************************
 * @brief Set interrupt (software simulated) service routine for RTC
 *
 * @param[in] irq_cb            callback for IRQ
 * @param[in] user_data         user data
 ****************************************************************************************
 */
void RTC_SoftSetISR(uint32_t (*irq_cb)(void *user_data), void *user_data);

#endif

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

typedef enum
{
    RTC_IRQ_ALARM = 0x04,
    RTC_IRQ_DAY = 0x08,
    RTC_IRQ_HOUR = 0x10,
    RTC_IRQ_MINUTE = 0x20,
    RTC_IRQ_SECOND = 0x40,
    RTC_IRQ_HALF_SECOND = 0x80,
} rtc_irq_t;

typedef enum
{
    RTC_TRIM_SPEED_UP = 0x00,
    RTC_TRIM_SLOW_DOWN = 0x01,
}rtc_trim_direction_t;

/**
 ****************************************************************************************
 * @brief Enable/Disable RTC
 *
 * RTC must be enabled before changing configurations.
 *
 * @param[in] enable            Enable(1)/Disable(0)
 ****************************************************************************************
 */
void RTC_Enable(uint8_t enable);

/**
 ****************************************************************************************
 * @brief Get current date time
 *
 *
 * @param[out] hour             Hour (0..23)
 * @param[out] minute           Minute (0..59)
 * @param[out] second           Second (0..59)
 * @return                      Days passed (5 bits, 0..31)
 ****************************************************************************************
 */
uint16_t RTC_GetTime(uint8_t *hour, uint8_t *minute, uint8_t *second);

/**
 ****************************************************************************************
 * @brief Modify date time
 *
 * @param[in] day               days passed (check datasheet for max. value)
 * @param[in] hour              Hour (0..23)
 * @param[in] minute            Minute (0..59)
 * @param[in] second            Second (0..59)
 ****************************************************************************************
 */
void RTC_ModifyTime(uint16_t day, uint8_t hour, uint8_t minute, uint8_t second);

/**
 ****************************************************************************************
 * @brief Check if RTC modification is done or not
 *
 * This indicator becomes zero when any of RTC control registers (the Counter, Alarm and
 * Control registers) are updated, and returned to `1` after modications are synchronized
 * to the much slower RTC clock domain.
 *
 * @return                      if RTC modification is done (non-0) or not (0)
 ****************************************************************************************
 */
int RTC_IsModificationDone(void);

/**
 ****************************************************************************************
 * @brief Config alarm
 *
 * @param[in] hour              Hour (0..23)
 * @param[in] minute            Minute (0..59)
 * @param[in] second            Second (0..59)
 ****************************************************************************************
 */
void RTC_ConfigAlarm(uint8_t hour, uint8_t minute, uint8_t second);

/**
 ****************************************************************************************
 * @brief Enable/Disable IRQs
 *
 * An IQR is disabled if not in `mask`.
 *
 * @param[in] mask              combination of `rtc_irq_t`
 ****************************************************************************************
 */
void RTC_EnableIRQ(uint32_t mask);

/**
 ****************************************************************************************
 * @brief Get IRQs state
 *
 * @return                      combination of `rtc_irq_t`
 ****************************************************************************************
 */
uint32_t RTC_GetIntState(void);

/**
 ****************************************************************************************
 * @brief Clear IRQs state
 *
 * @param[in] state             combination of `rtc_irq_t`
 ****************************************************************************************
 */
void RTC_ClearIntState(uint32_t state);

#ifndef SOFTWARE_RTC_DHMS

/**
 ****************************************************************************************
 * @brief Enable RTC timer (interrupt) as a wakeup source for DEEP SLEEP mode
 *
 * @param[in] enable            Enable(1)/disable(0)
 ****************************************************************************************
 */
void RTC_EnableDeepSleepWakeupSource(uint8_t enable);

/**
 ****************************************************************************************
 * @brief Digital trimming value for the second on the day boundary.
 *
 * @param[in] trim  Digital trimming value(1 ~ 31).
 * @param[in] dir   Set trimming direction in `rtc_trim_direction_t`.
 ****************************************************************************************
 */
void RTC_DayCntTrim(uint8_t trim, rtc_trim_direction_t dir);

/**
 ****************************************************************************************
 * @brief Digital trimming value for the second on the hour boundary.
 *
 * @param[in] trim  Digital trimming value(1 ~ 31).
 * @param[in] dir   Set trimming direction in `rtc_trim_direction_t`.
 ****************************************************************************************
 */
void RTC_HourCntTrim(uint8_t trim, rtc_trim_direction_t dir);

/**
 ****************************************************************************************
 * @brief Digital trimming value for the second on the minute boundary.
 *
 * @param[in] trim  Digital trimming value(1 ~ 31).
 * @param[in] dir   Set trimming direction in `rtc_trim_direction_t`.
 ****************************************************************************************
 */
void RTC_MinCntTrim(uint8_t trim, rtc_trim_direction_t dir);

/**
 ****************************************************************************************
 * @brief Digital trimming value for the rest of seconds.
 *
 * @param[in] trim  Digital trimming value(1 ~ 31).
 * @param[in] dir   Set trimming direction in `rtc_trim_direction_t`.
 ****************************************************************************************
 */
void RTC_SecCntTrim(uint8_t trim, rtc_trim_direction_t dir);

/**
 ****************************************************************************************
 * @brief Digital trimming value for the rest of half-seconds.
 *
 * @param[in] trim  Digital trimming value(0 ~ 16383).
 ****************************************************************************************
 */
void RTC_HSecCntTrim(uint16_t trim);

/**
 ****************************************************************************************
 * @brief Set all settings of TRIM register.
 *
 * @param[in] day_trim      Digital trimming value(1 ~ 31) for the second on the day boundary.
 * @param[in] hour_trim     Digital trimming value(1 ~ 31) for the second on the hour boundary.
 * @param[in] min_trim      Digital trimming value(1 ~ 31) for the second on the minute boundary.
 * @param[in] sec_trim      Digital trimming value(1 ~ 31) for the rest of seconds.
 *
 * @note Each trimming value uses only the lower 5 bits , and the highest bit indicates the
 *       direction in `rtc_trim_direction_t`.
 *       i.e. `uint8_t day_trim = (RTC_TRIM_SPEED_UP << 7) | 0x15`, reduce the count by 21.
 ****************************************************************************************
 */
void RTC_SetAllTrimValue(uint8_t day_trim, uint8_t hour_trim, uint8_t min_trim,uint8_t sec_trim);

/**
 ****************************************************************************************
 * @brief Set all settings of TRIM register according to calibration value of 32k clock
 *
 * Note: frequency of 32k clock shall be <= (32768 + 31.5)Hz.
 *
 * About theoretical precision:
 *     1. 32k clock calibration: 0.5ppm;
 *     1. RTC trim: ~0.5/32768 (worst case) = 15ppm.
 *
 * @param[in] cali_value    Calibration value of 32k clock
 *                          which can be got from `platform_read_info(PLATFORM_INFO_32K_CALI_VALUE)`.
 * @return                  0 if frequency of 32k clock is in valid range else non-0
 ****************************************************************************************
 */
int RTC_Trim(uint32_t cali_value);

/**
 ****************************************************************************************
 * @brief Get all settings of TRIM register.
 *
 * @param[out] hour_trim    Digital trimming value(1 ~ 31) for the second on the hour boundary.
 * @param[out] min_trim     Digital trimming value(1 ~ 31) for the second on the minute boundary.
 * @param[out] sec_trim     Digital trimming value(1 ~ 31) for the rest of seconds.
 * @return Digital trimming value(1 ~ 31) for the second on the day boundary.
 ****************************************************************************************
 */
uint8_t RTC_GetAllTrimValue(uint8_t *hour_trim, uint8_t *min_trim,uint8_t *sec_trim);

/**
 ****************************************************************************************
 * @brief Clear all settings of TRIM register.
 *
 ****************************************************************************************
 */
void RTC_ClearAllTrimValue(void);

#else

/**
 ****************************************************************************************
 * @brief Set interrupt (software simulated) service routine for RTC
 *
 * @param[in] irq_cb            callback for IRQ
 * @param[in] user_data         user data
 ****************************************************************************************
 */
void RTC_SoftSetISR(uint32_t (*irq_cb)(void *user_data), void *user_data);

#endif

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)

#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

