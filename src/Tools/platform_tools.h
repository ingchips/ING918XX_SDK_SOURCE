#ifndef platform_tools_h
#define platform_tools_h

#include <stdint.h>
#include "platform_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int bytes_free;                // total free bytes at present
    int bytes_minimum_ever_free;   // mininum of bytes_free from last run
} platform_stack_status_t;

/**
 ****************************************************************************************
 * @brief Instrument stack status when in ISR
 *
 *        Get current status and instrument the stack for another run.
 *        CAUTION: Reported stack usage information is not always accurate.
 *
 *        Examples:
 *         1. Check stack size used by function `foo`:
 *         ```C
 *         platform_stack_status_t status;
 *         platform_instrument_isr_stack(&status);
 *         foo();
 *         int used_by_foo = status.bytes_free - platform_instrument_isr_stack(NULL);
 *         ```
 *
 *         2. Detect stack overflow:
 *         ```C
 *         if (platform_instrument_isr_stack(NULL) <= 0)
 *             ISR_STACK_OVERFLOW!
 *         ```
 *
 * @param[out]  status              status of the stack used by ISR (can be NULL)
 * @return                          mininum of bytes_free from last run
 ****************************************************************************************
 */
int platform_instrument_isr_stack(platform_stack_status_t *status);

/**
 ****************************************************************************************
 * @brief Prepare ISR stack for instrumentation
 *
 * This function is only allowed to be called before `platform_install_isr_stack`.
 *
 *
 * @param[in]  top                  stack top (must be 4-bytes aligned)
 * @param[in]  size                 size of the new stack in bytes (multiple of 4)
 ****************************************************************************************
 */
void platform_start_instrument_isr_stack(void *top, int size);

#ifdef __cplusplus
}
#endif

#endif
