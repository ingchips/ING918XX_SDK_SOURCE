#ifndef __PERIPHERAL_RTC_H__
#define __PERIPHERAL_RTC_H__

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include <stdint.h>

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

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

