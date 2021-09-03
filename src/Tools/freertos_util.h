/*
 * Some FreeRTOS functions are not linked in `platform.bin`,
 * and can be implemented out of the kernel.
 *
 * Note: These functions are adapted from FreeRTOS Kernel source code.
 * 
 * See documentation of FreeRTOS.
 */
 
#ifndef _freertos_util_h
#define _freertos_util_h

#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement );

/**
 ****************************************************************************************
 * @brief a more accurate version of `pdMS_TO_TICKS`
 *
 * Note: for `pdMS_TO_TICKS`, the tick rate is configTICK_RATE_HZ, while the actual tick
 * rate is (SYSTICK_CLOCK_HZ / (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ)). Since
 * `(configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ)` is evaluated to an integer, generally,
 * the actual tick rate is faster than configTICK_RATE_HZ when: 
 *    `configSYSTICK_CLOCK_HZ mod configTICK_RATE_HZ != 0`.
 *
 * When `configSYSTICK_CLOCK_HZ mod configTICK_RATE_HZ == 0`, `AccurateMS_TO_TICKS` is more
 * accurate than `pdMS_TO_TICKS` because the actual `SYSTICK_CLOCK_HZ` is used.
 ****************************************************************************************
 */
TickType_t AccurateMS_TO_TICKS( uint32_t xTimeInMs );

/**
 ****************************************************************************************
 * @brief refresh internal clock frequency (driven ticks)
 *
 * Note: The clock is typically running at `RTC_CLK_FREQ` with drifting. Call this function to 
 * get an up-to-date frequency if needed.
 *
 ****************************************************************************************
 */
void vUpdateTicksClockFrequency(void);

#ifdef __cplusplus
}
#endif

#endif
