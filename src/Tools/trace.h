#ifndef _trace_h
#define _trace_h

#include <stdint.h>
#include "FreeRTOS.h"
#ifndef C2NIM
#include "task.h"
#include "semphr.h"
#include "queue.h"
#endif
#include "ingsoc.h"
#include "platform_api.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifndef TRACE_BUFF_SIZE
#define TRACE_BUFF_SIZE         (4096)  // must be 2^n
#endif

#define TRACE_BUFF_SIZE_MASK    (TRACE_BUFF_SIZE - 1)

typedef struct
{
    uint8_t           buffer[TRACE_BUFF_SIZE];
    uint16_t          write_next;
    uint16_t          read_next;
    TaskHandle_t      handle;
    SemaphoreHandle_t tx_sem; 
    SemaphoreHandle_t mutex;
    UART_TypeDef     *port;
} trace_info_t;

/**
 ****************************************************************************************
 * @brief Initialize UART trace
 *
 * @param[in] trace_info_t  trace context
 ****************************************************************************************
 */
void trace_init(trace_info_t *ctx);

/**
 ****************************************************************************************
 * @brief UART ISR callback
 *
 * @param[in] trace_info_t  trace context
 ****************************************************************************************
 */
uint32_t trace_uart_isr(trace_info_t *ctx);

/**
 ****************************************************************************************
 * @brief Trace event callback
 *
 * @param[in] trace         trace event 
 * @param[in] trace_info_t  trace context
 ****************************************************************************************
 */
uint32_t cb_trace(const platform_evt_trace_t *trace, trace_info_t *ctx);

/**
 ****************************************************************************************
 * @brief Flush saved trace data to UART
 *
 * @param[in] trace_info_t  trace context
 ****************************************************************************************
 */
void trace_flush(trace_info_t *ctx);

#ifdef __cplusplus
}
#endif

#endif
