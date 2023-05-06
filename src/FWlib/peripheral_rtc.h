#ifndef __PERIPHERAL_RTC_H__
#define __PERIPHERAL_RTC_H__

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include <stdint.h>
#include "ingsoc.h"

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
void RTC_Enable(const uint8_t flag);

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
 * @brief Get current full RTC value (full 48bit)
 *
 * @return              Current value
 ****************************************************************************************
 */
uint64_t RTC_CurrentFull(void);

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

/**
 ****************************************************************************************
 * @brief RTC_EnableFreeRun
 ****************************************************************************************
 */
#define RTC_EnableFreeRun(enable)   do { RTC_Enable(enable); } while(0)

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
 * @return                      Days passed
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

#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

