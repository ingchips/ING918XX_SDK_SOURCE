#ifndef _trace_h
#define _trace_h

#include <stdint.h>
#include "port_gen_os_driver.h"
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
    gen_handle_t      handle;
    gen_handle_t      tx_sem;
    UART_TypeDef     *port;
} trace_uart_t;

typedef struct
{
    uint8_t placeholder;
} trace_rtt_t;

typedef struct
{
    uint32_t page_cnt;
    uint32_t start_addr;
    uint32_t cur_page;
    uint32_t total_size;
    uint16_t offset_in_page;
    uint8_t frag[4];
    uint8_t frag_size;
    uint8_t enable;
} trace_flash_t;

typedef struct
{
    uint8_t           buffer[TRACE_BUFF_SIZE];
    uint16_t          write_next;
    uint16_t          read_next;
    uint32_t          msg_id;
    uint16_t          value_handle;
    uint16_t          conn_handle;
    uint8_t           req_thres;
    uint8_t           enabled:1;
    uint8_t           msg_sent:1;
} trace_air_t;

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
 * @brief Initialize air trace
 *
 * @param[in] ctx                   trace context
 * @param[in] msg_id                user message ID to trigger data notification
 * @param[in] req_thres             data length threshold to send `msg_id` message
 ****************************************************************************************
 */
void trace_air_init(trace_air_t *ctx, uint32_t msg_id, uint8_t req_thres);

/**
 ****************************************************************************************
 * @brief Control air trace
 *
 * Note: Use this to start/stop trace programmaticaly.
 *       Default: Disabled.
 *
 * @param[in] ctx                   trace context
 * @param[in] enable                start/continue (1) or stop/pause (0)
 * @param[in] conn_handle           handle of connection (when enabled)
 * @param[in] value_handle          handle of the chara. to send trace data (when enabled)
 ****************************************************************************************
 */
void trace_air_enable(trace_air_t *ctx, int enable, uint16_t conn_handle, uint16_t value_handle);

/**
 ****************************************************************************************
 * @brief Send recording data over the air
 *
 * Note: Call this in the user message handler of `msg_id` given in `trace_air_init`.
 *
 * @param[in] ctx                   trace context
 ****************************************************************************************
 */
void trace_air_send(trace_air_t *ctx);

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
 * @brief Trace event callback
 *
 * @param[in] trace         trace event
 * @param[in] ctx           trace context
 ****************************************************************************************
 */
uint32_t cb_trace_air(const platform_evt_trace_t *trace, trace_air_t *ctx);

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
