// ----------------------------------------------------------------------------
// Copyright Message
// ----------------------------------------------------------------------------
//
// INGCHIPS confidential and proprietary.
// COPYRIGHT (c) 2018 by INGCHIPS
//
// All rights are reserved. Reproduction in whole or in part is
// prohibited without the written consent of the copyright owner.
//
//
// ----------------------------------------------------------------------------

#ifndef _STACK_API_H_
#define _STACK_API_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct platform_ver_s
{
    unsigned short major;
    char  minor;
    char  patch;
} platform_ver_t;

typedef struct hard_fault_info_s
{
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
} hard_fault_info_t;

typedef struct assertion_info_s
{
    const char *file_name;
    int line_no;
} assertion_info_t;

typedef enum
{
    // platform callback for putc (for logging)
    // NOTE: param (void *data) is casted from char *
    // example: uint32_t cb_putc(char *c, void *dummy)
    PLATFORM_CB_EVT_PUTC,

    // when bluetooth protocol stack ask app to initialize
    PLATFORM_CB_EVT_PROFILE_INIT,

    // periphrals need to be re-initialized after deep-sleep, user can handle this event
    PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP,

    // return 0 if deep sleep is not allowed now; else deep sleep is allowed
    // e.g. when periphrals still have data to process (UART is tx buffer not empty)
    PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED,
    
    // when hard fault occurs
    // NOTE: param (void *data) is casted from hard_fault_info_t *
    // example: uint32_t cb_hard_fault(hard_fault_info_t *info, void *dummy)
    // if this callback is not defined, CPU enters a dead loop 
    PLATFORM_CB_EVT_HARD_FAULT,
    
    // when software assertion fails
    // NOTE: param (void *data) is casted from assertion_info_t *
    // if this callback is not defined, CPU enters a dead loop 
    PLATFORM_CB_EVT_ASSERTION,
    
    // when LLE is initializing
    PLATFORM_CB_EVT_LLE_INIT,
    
    // when allocation on heap fails (heap out of memory)
    // if this callback is not defined, CPU enters a dead loop 
    PLATFORM_CB_EVT_HEAP_OOM,
    
    // platform callback for output or save a trace item
    // NOTE: param (void *data) is casted from platform_trace_evt_t *
    PLATFORM_CB_EVT_TRACE,

    PLATFORM_CB_EVT_MAX
} platform_evt_callback_type_t;

typedef enum
{
    PLATFORM_CB_IRQ_RTC,
    PLATFORM_CB_IRQ_TIMER0,
    PLATFORM_CB_IRQ_TIMER1,
    PLATFORM_CB_IRQ_TIMER2,
    PLATFORM_CB_IRQ_GPIO,
    PLATFORM_CB_IRQ_SPI0,
    PLATFORM_CB_IRQ_SPI1,
    PLATFORM_CB_IRQ_UART0,
    PLATFORM_CB_IRQ_UART1,
    PLATFORM_CB_IRQ_I2C0,
    PLATFORM_CB_IRQ_I2C1,

    PLATFORM_CB_IRQ_MAX
} platform_irq_callback_type_t;

typedef uint32_t (*f_platform_evt_cb)(void *data, void *user_data);
typedef uint32_t (*f_platform_irq_cb)(void *user_data);

// A trace item is a combination of data1 and data2. Note:
// 1. len1 or len2 might be 0, but not both
// 2. if callback function finds that it can't output data of size len1 + len2, then, both data1
//    & data2 should be discarded to avoid trace item corruption.
typedef struct
{
    const void *data1;
    const void *data2;
    uint16_t len1;
    uint16_t len2;
} platform_evt_trace_t;

// A trace item is has an ID
typedef enum
{
    PLATFORM_TRACE_ID_EVENT                 = 0,
    PLATFORM_TRACE_ID_HCI_CMD               = 1,
    PLATFORM_TRACE_ID_HCI_EVENT             = 2,
    PLATFORM_TRACE_ID_HCI_ACL               = 3,
    PLATFORM_TRACE_ID_LLCP                  = 4
} platform_trace_item_t;

/**
 ****************************************************************************************
 * @brief register callback function for platform events
 *
 * @param[in] type          the event
 * @param[in] f             the callback function
 * @param[in] user_data     user data that will be passed into callback function `f`
 ****************************************************************************************
 */
void platform_set_evt_callback(platform_evt_callback_type_t type, f_platform_evt_cb f, void *user_data);

/**
 ****************************************************************************************
 * @brief register callback function for platform interrupt requests
 *
 * @param[in] type          the irq
 * @param[in] f             the callback function
 * @param[in] user_data     user data that will be passed into callback function `f`
 ****************************************************************************************
 */
void platform_set_irq_callback(platform_irq_callback_type_t type, f_platform_irq_cb f, void *user_data);

/**
 ****************************************************************************************
 * @brief Get platform version number
 *
 * @return                  version number
 ****************************************************************************************
 */
const platform_ver_t *platform_get_version(void);

/**
 ****************************************************************************************
 * @brief raise an assertion. This function will raise the assertion event:
 *        `PLATFORM_CB_EVT_ASSERTION`
 *
 ****************************************************************************************
 */
void platform_raise_assertion(const char *file_name, int line_no);

typedef struct
{
    uint32_t bytes_free;                // total free bytes
    uint32_t bytes_minimum_ever_free;   // mininum of bytes_free from startup
} platform_heap_status_t;

/**
 ****************************************************************************************
 * @brief Get heap status
 *
 * @param[out]  status              heap status
 ****************************************************************************************
 */
void platform_get_heap_status(platform_heap_status_t *status);

/**
 ****************************************************************************************
 * @brief Reset platform.
 *
 * Note: when calling this function, the code after it will not be executed.
 ****************************************************************************************
 */
void platform_reset(void);

/**
 ****************************************************************************************
 * @brief Switch to a secondary app.
 *
 * @param[in] app_addr              app entry addr (i.e. ISR vector address)
 ****************************************************************************************
 */
void platform_switch_app(const uint32_t app_addr);

typedef enum
{
    PLATFORM_CFG_LOG_HCI,       // flag is ENABLE or DISABLE. default: DISABLE
    PLATFORM_CFG_POWER_SAVING,  // flag is ENABLE or DISABLE. default: DISABLE
    PLATFORM_CFG_TRACE_MASK     // flag is bitmap of platform_trace_item_t. default: 0
} platform_cfg_item_t;

#define PLATFORM_CFG_ENABLE     1
#define PLATFORM_CFG_DISABLE    0

/**
 ****************************************************************************************
 * @brief platform configurations.
 *
 * @param[in] item          Configuration item
 * @param[in] flag          see platform_cfg_item_t.
 ****************************************************************************************
 */
void platform_config(const platform_cfg_item_t item, const uint32_t flag);

/**
 ****************************************************************************************
 * @brief Shutdown the whole system, and power on again after a duration
 *        specified by duration_cycles.
 *        Optionally, a portion of SYS memory can be retentioned during shutdown.
 *
 * @param[in] duration_cycles       Duration before power on again (measured in cycles of 32k clock)
 *                                  Mininum value: 825 cycles (about 25.18ms)
 * @param[in] p_retention_data      Pointer to the start of data to be retentioned
 * @param[in] data_size             Size of the data to be retentioned
 ****************************************************************************************
 */
void platform_shutdown(const uint32_t duration_cycles, const void *p_retention_data, const uint32_t data_size);

/**
 ****************************************************************************************
 * @brief generate random bytes by using true hardware random-number generator
 *
 * @param[out] bytes            random data output
 * @param[in]  len              byte number of random data
 ****************************************************************************************
 */
// void platform_hrng(uint8_t *bytes, const uint32_t len);
// WARNING: ^^^ this API is not available in this release


/**
 ****************************************************************************************
 * @brief the printf function
 *
 * @param[in]  format           format string
 * @param[in]  ...              arguments
 ****************************************************************************************
 */
void platform_printf(const char *format, ...);

// NOTE: for debug only
void sysSetPublicDeviceAddr(const unsigned char *addr);

// set rf source
// 0: use external crystal
// 1: use internal clock loopback
void platform_set_rf_clk_source(const uint8_t source);

void platform_set_rf_init_data(const uint32_t *rf_init_data);

void platform_set_rf_power_mapping(const int16_t *rf_power_mapping);

typedef enum coded_scheme_e
{
    BLE_CODED_S8,
    BLE_CODED_S2
} coded_scheme_t;

/**
 ****************************************************************************************
 * @brief set coded scheme of a advertising set
 *        Note: this function should be called before the advertising set is enabled.
 *              Default scheme for all advertising sets are S=8
 *
 * @param[in]  adv_hdl          handle of the advertising set
 * @param[in]  scheme           selected scheme
 ****************************************************************************************
 */
void ll_set_adv_coded_scheme(const uint8_t adv_hdl, const coded_scheme_t scheme);

/**
 ****************************************************************************************
 * @brief set coded scheme for initiating PDU
 *        Note: this function should be called before initiating is enabled.
 *
 * @param[in]  scheme           selected scheme (default S=8)
 ****************************************************************************************
 */
void ll_set_initiating_coded_scheme(const coded_scheme_t scheme);

/**
 ****************************************************************************************
 * @brief Give link layer (slave role) a hint on ce_len of a connection
 *
 * @param[in]  conn_handle      handle of an existing connection
 * @param[in]  min_ce_len       information parameter about the minimum length of connection
 *                              event needed for this LE connection.
 *                              Range: 0x0000 – 0xFFFF
 *                              Time = N * 0.625 ms.
 * @param[in]  max_ce_len       information parameter about the maximum length of connection
 *                              event needed for this LE connection.
 ****************************************************************************************
 */
void ll_hint_on_ce_len(const uint16_t conn_handle, const uint16_t min_ce_len, const uint16_t max_ce_len);

#ifdef __cplusplus
}
#endif
   
#endif
