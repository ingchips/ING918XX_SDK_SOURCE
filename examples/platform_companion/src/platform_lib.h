#ifndef _platform_lib_h
#define _platform_lib_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Additional initialization for platform in ROM
 *
 * @return void
 */
void platform_init(void);

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
