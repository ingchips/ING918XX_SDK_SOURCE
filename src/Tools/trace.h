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
} trace_uart_t;

typedef struct
{
    SemaphoreHandle_t mutex;
} trace_rtt_t;

typedef struct
{
    SemaphoreHandle_t mutex;
    uint32_t page_cnt;
    uint32_t start_addr;
    uint32_t cur_page;
    uint32_t total_size;
    uint16_t offset_in_page;
    uint8_t frag[4];
    uint8_t frag_size;
    uint8_t enable;
} trace_flash_t;

/**
 ****************************************************************************************
 * @brief Initialize UART trace
 *
 * @param[in] ctx       trace context
 ****************************************************************************************
 */
void trace_uart_init(trace_uart_t *ctx);

/**
 ****************************************************************************************
 * @brief Initialize RTT trace
 *
 * @param[in] ctx       trace context
 ****************************************************************************************
 */
void trace_rtt_init(trace_rtt_t *ctx);

/**
 ****************************************************************************************
 * @brief Initialize Flash trace
 *
 * Note: 1. trace is save to flash cyclically.
 *       2. `eflash.c` must be included in the project.
 *
 * @param[in] ctx                   trace context
 * @param[in] flash_start_addr      start address of flash storage 
 *                                  (must be at boundary of a page)
 * @param[in] total_size            total size of flash storage
 *                                  (must be multiple of page size)
 ****************************************************************************************
 */
void trace_flash_init(trace_flash_t *ctx, uint32_t flash_start_addr, uint32_t total_size);

/**
 ****************************************************************************************
 * @brief Control Flash trace
 *
 * Note: Use this to start/stop trace programmaticaly.
 *       Default: Disabled.
 *
 * @param[in] ctx                   trace context
 * @param[in] enable                start/continue (1) or stop/pause (0)
 ****************************************************************************************
 */
void trace_flash_enable(trace_flash_t *ctx, int enable);

/**
 ****************************************************************************************
 * @brief Erase Flash trace storage
 *
 *
 * @param[in] ctx                   trace context
 ****************************************************************************************
 */
void trace_flash_erase_all(trace_flash_t *ctx);

/**
 ****************************************************************************************
 * @brief UART ISR callback
 *
 * @param[in] trace_uart_t  trace context
 ****************************************************************************************
 */
uint32_t trace_uart_isr(trace_uart_t *ctx);

/**
 ****************************************************************************************
 * @brief Trace event callback
 *
 * @param[in] trace         trace event 
 * @param[in] ctx           trace context
 ****************************************************************************************
 */
uint32_t cb_trace_uart(const platform_evt_trace_t *trace, trace_uart_t *ctx);

/**
 ****************************************************************************************
 * @brief Trace event callback
 *
 * @param[in] trace         trace event 
 * @param[in] ctx           trace context
 ****************************************************************************************
 */
uint32_t cb_trace_rtt(const platform_evt_trace_t *trace, trace_rtt_t *ctx);

/**
 ****************************************************************************************
 * @brief Trace event callback
 *
 * @param[in] trace         trace event 
 * @param[in] ctx           trace context
 ****************************************************************************************
 */
uint32_t cb_trace_flash(const platform_evt_trace_t *trace, trace_flash_t *ctx);

/**
 ****************************************************************************************
 * @brief Flush saved trace data to UART
 *
 * @param[in] trace_uart_t  trace context
 ****************************************************************************************
 */
void trace_flush(trace_uart_t *ctx);

typedef int (* f_trace_puts)(const char *str);

/**
 ****************************************************************************************
 * @brief dump full memory & registers for later analysis 
 *
 * @param[in] f_puts        callback function for print strings (ending with '\n\0', w/o '\n' or '\r')
 * @param[in] size          memory block size (64 or 32)
 ****************************************************************************************
 */
void trace_full_dump(f_trace_puts f_puts, int size);

#ifdef __cplusplus
}
#endif

#include "SEGGER_RTT.h"

#endif
