#ifndef _platform_lib_h
#define _platform_lib_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Powers up the system with the specified PMU mode.
 *
 * This function initializes the platform by setting the power management unit (PMU) mode.
 * The PMU mode determines the power-up sequence and configuration of the platform.
 *
 * @param pmu_mode          The mode to set for the PMU.
 *                          * 0: PMU in LDO mode. It's default mode
 *                          * 1: PMU in DCDC mode
 *
 * @return void
 */
void platform_power_up(uint8_t pmu_mode);

/**
 * @brief Launches the platform.
 *
 * This function initializes and starts the platform. It sets up necessary resources
 * and starts the platform's main loop.
 *
 * This function never returns.
 */
void launch_platform(void);

#ifdef __cplusplus
}
#endif

#endif
