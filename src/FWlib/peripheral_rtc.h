#ifndef __PERIPHERAL_RTC_H__
#define __PERIPHERAL_RTC_H__

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include <stdint.h>

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

#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

