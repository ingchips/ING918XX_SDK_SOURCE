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
#include "ingsoc.h"
#include "ll_api.h"

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
    PLATFORM_DEEP_SLEEP = 0,
#if (INGCHIPS_FAMILY != INGCHIPS_FAMILY_918)
    PLATFORM_DEEPER_SLEEP = 1,
    PLATFORM_BLE_ONLY_SLEEP = 2,
#endif
} platform_sleep_category_b_t;

#define PLATFORM_ALLOW_DEEP_SLEEP            (1 << PLATFORM_DEEP_SLEEP)

#if (INGCHIPS_FAMILY != INGCHIPS_FAMILY_918)
#define PLATFORM_ALLOW_DEEPER_SLEEP          (1 << PLATFORM_DEEPER_SLEEP)
#define PLATFORM_ALLOW_BLE_ONLY_SLEEP        (1 << PLATFORM_BLE_ONLY_SLEEP)
#endif

typedef enum
{
    PLATFORM_WAKEUP_REASON_NORMAL = 0,  // normal wakeup: sleep procedure is completed successfully
    PLATFORM_WAKEUP_REASON_ABORTED = 1, // sleep process after last `PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED`
                                        // is aborted. See also `PLATFORM_CFG_ALWAYS_CALL_WAKEUP`
                                        //
                                        // Platform will call `PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP` with this reason
                                        // only when `PLATFORM_CFG_ALWAYS_CALL_WAKEUP` is enabled.
} platform_wakeup_call_reason_t;

typedef struct
{
    uint8_t reason; // see `platform_wakeup_call_reason_t`
    uint8_t mode;   // see `platform_sleep_category_b_t`
} platform_wakeup_call_info_t;

typedef enum
{
    // platform callback for putc (for logging)
    // NOTE: param (void *data) is casted from char *
    // example: uint32_t cb_putc(char *c, void *dummy)
    PLATFORM_CB_EVT_PUTC,

    // When bluetooth protocol stack ask app to initialize
    PLATFORM_CB_EVT_PROFILE_INIT,

    // peripherals need to be re-initialized after deep-sleep, user can handle this event
    // Note: param (void *data) is casted from (platform_wakeup_call_info_t *).
    // CAUTION: RTOS is not resumed yet, some APIs are not usable; Some platform
    //          APIs (such as `platform_get_us_time`) might be unusable either.
    // See also `PLATFORM_CB_EVT_ON_IDLE_TASK_RESUMED`.
    PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP,

    // When OS is fully resumed from power saving modes.
    // The callback is invoked after `PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP` if
    // its reason is `PLATFORM_WAKEUP_REASON_NORMAL`.
    // For NoOS variants, the callback is invoked by `platform_os_idle_resumed_hook()`.
    // This event is different with `PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP`:
    // * all OS functionalities are resumed (For NoOS variants, this depends on the
    //   proper use of `platform_os_idle_resumed_hook()`)
    // * all platform APIs are functional
    // * callback is invoked in the idle task.
    // Note: param (void *data) is always 0.
    PLATFORM_CB_EVT_ON_IDLE_TASK_RESUMED,

    // return bits combination of `PLATFORM_ALLOW_xxx`
    // return 0 if deep sleep is not allowed now; else deep sleep is allowed
    // e.g. when peripherals still have data to process (UART is tx buffer not empty)
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
    // NOTE: param (void *data) is casted from an integer identifying which heap is OOM:
    //      * 0: FreeRTOS's heap;
    //      * 1: Link Layer's heap;
    //      * 2: Link Layer's task pool.
    // if this callback is not defined, CPU enters a dead loop
    PLATFORM_CB_EVT_HEAP_OOM,

    // platform callback for output or save a trace item
    // NOTE: param (void *data) is casted from platform_trace_evt_t *
    PLATFORM_CB_EVT_TRACE,

    // platform callback for hardware exceptions
    // NOTE: param (void *data) is casted from platform_exception_id_t
    PLATFORM_CB_EVT_EXCEPTION,

    // platform callback for customized IDLE procedure
    // developers can setup this callback to implement customized IDLE procedure.
    // a typical IDLE procedure is: `__DSB(); __WFI(); __ISB();`
    // Note on ING918: not available. ignored.
    PLATFORM_CB_EVT_IDLE_PROC,

    // Take over HCI and isolate the built-in Host completely
    // when defined:
    //      * HCI events and ACL data are passed to this callback;
    //      * `PLATFORM_CB_EVT_PROFILE_INIT` is ignored.
    // Note: param (void *data) is casted from (const platform_hci_recv_t *).
    PLATFORM_CB_EVT_HCI_RECV,

    PLATFORM_CB_EVT_MAX
} platform_evt_callback_type_t;

typedef uint32_t (*f_platform_evt_cb)(void *data, void *user_data);
typedef uint32_t (*f_platform_irq_cb)(void *user_data);

typedef struct platform_hci_recv
{
    uint32_t hci_type;      // `HCI_ACL_DATA_PACKET` or `HCI_EVENT_PACKET`
    uint16_t conn_handle;   // connection handle for `HCI_ACL_DATA_PACKET`
    const uint8_t *buff;    // data of ACL or event
    uint16_t len_of_hci;    // length of `buff`
                            // for HCI_ACL_DATA_PACKET:
                            //      * `buff` points to a full HCI ACL Data packet,
                            //      * `len_of_hci` = length of the whole HCI ACL Data packet
                            //      where full HCI ALC Data packet = Handle | pbbc | Data Total Length | Data.
                            // for HCI_EVENT_PACKET:
                            //      * `buff` points to a full HCI Event packet,
                            //      * `len_of_hci` = length of the whole HCI Event packet
                            //      where full HCI Event packet = Event Code | Param Total Length | Event Parameters.
    const void *handle;     // handle for freeing data or event. See `platform_hci_interf_t`.
} platform_hci_recv_t;

typedef struct platform_evt_cb_info
{
    f_platform_evt_cb  f;
    void              *user_data;
} platform_evt_cb_info_t;

typedef struct platform_irq_cb_info
{
    f_platform_irq_cb  f;
    void              *user_data;
} platform_irq_cb_info_t;

typedef struct
{
    platform_evt_cb_info_t callbacks[PLATFORM_CB_EVT_MAX];
} platform_evt_cb_table_t;

typedef struct
{
    platform_irq_cb_info_t callbacks[PLATFORM_CB_IRQ_MAX];
} platform_irq_cb_table_t;

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

// Exception ID
typedef enum
{
    PLATFORM_EXCEPTION_NMI          = 0,
    PLATFORM_EXCEPTION_HARD_FAULT   = 1,    // this will not be reported in `PLATFORM_CB_EVT_EXCEPTION`
    PLATFORM_EXCEPTION_MPU_FAULT    = 2,
    PLATFORM_EXCEPTION_BUS_FAULT    = 3,
    PLATFORM_EXCEPTION_USAGE_FAULT  = 4,
} platform_exception_id_t;

// A trace item is has an ID
typedef enum
{
    PLATFORM_TRACE_ID_EVENT                 = 0,
    PLATFORM_TRACE_ID_HCI_CMD               = 1,
    PLATFORM_TRACE_ID_HCI_EVENT             = 2,
    PLATFORM_TRACE_ID_HCI_ACL               = 3,
    PLATFORM_TRACE_ID_LLCP                  = 4,
    PLATFORM_TRACE_ID_RAW                   = 5,
    PLATFORM_TRACE_ID_EVENT_ERROR           = 6,
    PLATFORM_TRACE_ID_SM                    = 8,
} platform_trace_item_t;

/**
 ****************************************************************************************
 * @brief Output a block of raw data to TRACE. ID is PLATFORM_TRACE_ID_RAW
 *
 * @param[in] buffer        data buffer
 * @param[in] byte_len      length of data buffer in bytes
 ****************************************************************************************
 */
void platform_trace_raw(const void *buffer, const int byte_len);

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
 * Once registered, the corresponding interrupt is enabled. After waking up from sleep
 * modes, interrupts enabled previously are disabled again, which is also the default
 * interrupts enable/disable state.
 *
 * Note: "Enabling" an interrupt here is from CPU's point of view.
 *
 * @param[in] type          the irq
 * @param[in] f             the callback function
 * @param[in] user_data     user data that will be passed into callback function `f`
 ****************************************************************************************
 */
void platform_set_irq_callback(platform_irq_callback_type_t type, f_platform_irq_cb f, void *user_data);

/**
 ****************************************************************************************
 * @brief Enable/disable interrupt requests
 *
 * Note: "Enabling" an interrupt here is from CPU's point of view.
 *
 * @param[in] type          the irq
 * @param[in] flag          enable(1)/disable(0)
 ****************************************************************************************
 */
void platform_enable_irq(platform_irq_callback_type_t type, uint8_t flag);

/**
 ****************************************************************************************
 * @brief register callback function table for all platform events
 *
 * Instead of configure callback functions one by one, this function registers a
 * table for ALL events.
 *
 * DO NOT use this if `platform_set_evt_callback` is used.
 *
 * @param[in] table         callback function table
 ****************************************************************************************
 */
void platform_set_evt_callback_table(const platform_evt_cb_table_t *table);

/**
 ****************************************************************************************
 * @brief register callback function table for all platform interrupt requests
 *
 * Instead of configure callback functions one by one, this function registers a
 * table for ALL interrupt requests. When using this API, interrupts can be enabled
 * using corresponding IRQ enable/disable functions of MCU.
 *
 * DO NOT use this if `platform_set_irq_callback` is used.
 *
 * @param[in] table         callback function table
 ****************************************************************************************
 */
void platform_set_irq_callback_table(const platform_irq_cb_table_t *table);

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
    int bytes_free;                // total free bytes at present
    int bytes_minimum_ever_free;   // mininum of bytes_free from startup
} platform_heap_status_t;

/**
 ****************************************************************************************
 * @brief Get status of built-in RTOS's heap
 *
 * @param[out]  status              heap status
 ****************************************************************************************
 */
// void platform_get_heap_status(platform_heap_status_t *status);
// WARNING: ^^^ this API is not available in this release


/**
 ****************************************************************************************
 * @brief Install a new stack for ISR
 *
 * In case apps need a much larger stack than the default one in ISR, a new stack can be
 * installed to repleace the default one.
 *
 * This function is only allowed to be called in `app_main`. The new stack is put into
 * use after `app_main` returns.
 *
 *
 * @param[in]  top                  stack top (must be 4-bytes aligned)
 ****************************************************************************************
 */
// void platform_install_isr_stack(void *top);
// WARNING: ^^^ this API is not available in this release


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

/**
 ****************************************************************************************
 * @brief Write value to the persistent register, of which the value is kept even
 *        in power saving mode.
 *
 * For ING918: the least FOUR significant bits of `value` are saved;
 * For ING916: the least FIVE significant bits of `value` are saved.
 *
 * @param[in] value              value
 ****************************************************************************************
 */
void platform_write_persistent_reg(const uint8_t value);

/**
 ****************************************************************************************
 * @brief Read value from the persistent register, of which the value is kept even
 *        in power saving mode.
 *
 * @return                       value that has been written. (Initial value: 0)
 ****************************************************************************************
 */
uint8_t platform_read_persistent_reg(void);

/**
 ****************************************************************************************
 * @brief Shutdown the whole system, and power on again after a duration
 *        specified by duration_cycles or by external wake up source.
 *        Optionally, a portion of SYS memory can be retained during shutdown.
 *
 * External wake up source:
 *      ING918: EXT_INT;
 *      ING916: GPIOs that are configured as DEEPER sleep wake up source.
 *
 * Retainable RAM:
 *      ING918: starting from 0x20000000, 64KiB
 *      ING916: starting from 0x20000000, 16KiB
 *
 * @param[in] duration_cycles       Duration before power on again (measured in cycles of real time clock)
 *                                  Minimum value: 825 cycles (about 25.18ms)
 *                                  When = 0: only power on when external wake up source is asserted
 * @param[in] p_retention_data      Pointer to the start of data to be retained
 * @param[in] data_size             Size of the data to be retained
 ****************************************************************************************
 */
void platform_shutdown(const uint32_t duration_cycles, const void *p_retention_data, const uint32_t data_size);

typedef enum
{
    LL_FLAG_DISABLE_CTE_PREPROCESSING   = 1,    // disable internal CTE preprocessing
    LL_FLAG_LEGACY_ONLY_INITIATING      = 4,    // only do initiating to legacy devices
    LL_FLAG_LEGACY_ONLY_SCANNING        = 8,    // only do scanning for legacy devices
    LL_FLAG_REDUCE_INSTANT_ERRORS       = 16,   // reduce report instance passed errors
    LL_FLAG_DISABLE_RSSI_FILTER         = 64,   // disable internal RSSI filter
    LL_FLAG_RSSI_AFTER_CRC              =128,   // only read RSSI from packages with correct CRC
} ll_cfg_flag_t;

typedef enum
{
    PLATFORM_CFG_LOG_HCI,       // (ING918 only) flag is ENABLE or DISABLE. default: DISABLE
    PLATFORM_CFG_POWER_SAVING,  // flag is ENABLE or DISABLE. default: DISABLE
    PLATFORM_CFG_TRACE_MASK,    // flag is bitmap of platform_trace_item_t. default: 0
    PLATFORM_CFG_RT_RC_EN,         // Enable/Disable internal real time RC clock. Default: Enable
    PLATFORM_CFG_RC32K_EN = PLATFORM_CFG_RT_RC_EN,
    PLATFORM_CFG_RT_OSC_EN,     // Enable/Disable external real time crystal oscillator. Default: Enable
    PLATFORM_CFG_OSC32K_EN = PLATFORM_CFG_RT_OSC_EN,
    PLATFORM_CFG_RT_CLK,        // real time clock selection. flag is platform_rt_clk_src_t. default: PLATFORM_RT_RC
                                // Note 1: When modifying this configuration, both RT_RC and RT_OSC should be ENABLED.
                                // Note 2: Unused clock can be disabled.
    PLATFORM_CFG_32K_CLK = PLATFORM_CFG_RT_CLK,
    PLATFORM_CFG_RT_CLK_ACC,    // Configure real time clock accuracy in ppm.
    PLATFORM_CFG_32K_CLK_ACC = PLATFORM_CFG_RT_CLK_ACC,
    PLATFORM_CFG_RT_CLK_CALI_PERIOD, // real time clock auto-calibration period in seconds. Default: 3600 * 2
    PLATFORM_CFG_32K_CALI_PERIOD = PLATFORM_CFG_RT_CLK_CALI_PERIOD,
    PLATFORM_CFG_PS_DBG_0,      // debugging parameter
    PLATFORM_CFG_DEEP_SLEEP_TIME_REDUCTION, // sleep time reduction (deep sleep mode) in us. (default: ~550us)
    PLATFORM_CFG_PS_DBG_1 = PLATFORM_CFG_DEEP_SLEEP_TIME_REDUCTION, // obsoleted
    PLATFORM_CFG_SLEEP_TIME_REDUCTION,      // sleep time reduction (other sleep mode) in us. (default: ~550us)
    PLATFORM_CFG_PS_DBG_2 = PLATFORM_CFG_SLEEP_TIME_REDUCTION,      // obsoleted
    PLATFORM_CFG_LL_DBG_FLAGS,  // Link layer flags (combination of `ll_cfg_flag_t`)
    PLATFORM_CFG_LL_LEGACY_ADV_INTERVAL,    // Link layer legacy advertising intervals for high duty cycle (higher 16bits)
                                            //      and normal duty cylce (lower 16bits) in micro seconds
                                            // Default:   high duty cycle: 1250
                                            //          normal duty cycle: 1500
    PLATFORM_CFG_RTOS_ENH_TICK,             // Enhanced Ticks. Default: DISABLE
                                            // When enabled: IRQ's impact on accuracy of RTOS ticks is reduced
                                            // Note: this feature has negative impact on power consumption.
    PLATFORM_CFG_LL_DELAY_COMPENSATION,     // When system runs at a lower frequency,
                                            // more time (in us) is needed to run Link layer.
                                            // For example, if ING916 runs at 24MHz, configure this to 2500
    PLATFORM_CFG_24M_OSC_TUNE,              // 24M OSC tunning (not available for ING918)
                                            // For ING916: values may vary in 0x16~0x2d, etc.
    PLATFORM_CFG_ALWAYS_CALL_WAKEUP,        // always trigger `PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP` no matter if deep sleep
                                            // procedure is completed or aborted (failed).
                                            // Default for ING918: Disabled(0) for backward compatibility
                                            // Default for ING916: Enabled(1)
    PLATFORM_CFG_PS_DBG_3,
    PLATFORM_CFG_PS_DBG_4,                  // Debugging parameters for ING916. Default (0)
                                            // Bit [0]: `platform_shutdown` uses DEEPER SLEEP (1) or SLEEP (0)
    PLATFORM_CFG_FAST_DEEP_SLEEP_TIME_REDUCTION, // sleep time reduction (fast deep sleep mode) in us.
                                                 // Requirement: <= PLATFORM_CFG_DEEP_SLEEP_TIME_REDUCTION
                                                 // When equal to PLATFORM_CFG_DEEP_SLEEP_TIME_REDUCTION, fast deep sleep mode is not used
                                                 // Only available for ING916 (default: ~2000us)
    PLATFORM_CFG_AUTO_REDUCE_CLOCK_FREQ,    // automatic reduce CPU clock frequency in these circumstances:
                                            // * the default IDLE procedure
                                            // * when entering sleep modes
                                            // Only available for ING916 (default: Enabled)
    PLATFORM_CFG_PS_DBG_5,                  // Reserved debugging parameter
} platform_cfg_item_t;

typedef enum
{
    PLATFORM_RT_OSC,           // external crystal oscillator
    PLATFORM_32K_OSC = PLATFORM_RT_OSC,
    PLATFORM_RT_RC,            // internal RC clock
    PLATFORM_32K_RC = PLATFORM_RT_RC,
} platform_rt_clk_src_t;

#define platform_32k_clk_src_t platform_rt_clk_src_t

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

typedef enum
{
    PLATFORM_INFO_RT_OSC_STATUS,        // Read status of real time crystal oscillator. (0: not OK; Non-0: OK)
                                        // "OK" means running.
                                        // For ING916: this clock become running **after** selected as real time clock
                                        //             clock source.
    PLATFORM_INFO_OSC32K_STATUS = PLATFORM_INFO_RT_OSC_STATUS,
    PLATFORM_INFO_RT_CLK_CALI_VALUE,    // Read current real time clock calibration result.
    PLATFORM_INFO_32K_CALI_VALUE = PLATFORM_INFO_RT_CLK_CALI_VALUE,
    PLATFOFM_INFO_IRQ_NUMBER = 50,      // Get the underline IRQ number of a platform IRQ
                                        // for example, platform_read_info(PLATFOFM_INFO_IRQ_NUMBER + PLATFORM_CB_IRQ_UART0)
    PLATFOFM_INFO_NUMBER = 255,
} platform_info_item_t;

/**
 ****************************************************************************************
 * @brief read platform information.
 *
 * @param[in] item          Information item
 * @return                  Information.
 ****************************************************************************************
 */
uint32_t platform_read_info(const platform_info_item_t item);

/**
 ****************************************************************************************
 * @brief Calibrate real-time clock and get the calibration value.
 *
 * Real time clock auto-calibration timer is also reset, which means that next auto-calibration
 * is supposed to be carried out after `PLATFORM_CFG_RT_CLK_CALI_PERIOD` seconds.
 *
 * @return                  Calibration value.
 ****************************************************************************************
 */
uint32_t platform_calibrate_rt_clk(void);

/**
 ****************************************************************************************
 * @brief Tune internal the real-time RC clock with `value`.
 *
 * @param[in] value          Value used to tune the clock (returned by `platform_rt_rc_auto_tune`)
 ****************************************************************************************
 */
void platform_rt_rc_tune(uint16_t value);

/**
 ****************************************************************************************
 * @brief Automatically tune the internal real time RC clock, and get the tuning value.
 *
 * This is equivalent to `platform_rt_rc_auto_tune2(frequency)`.
 *
 * @return                  Value used to tune the clock
 ****************************************************************************************
 */
uint16_t platform_rt_rc_auto_tune(void);

#define platform_calibrate_32k      platform_calibrate_rt_clk
#define platform_32k_rc_tune        platform_rt_rc_tune
#define platform_32k_rc_auto_tune   platform_rt_rc_auto_tune

/**
 ****************************************************************************************
 * @brief Automatically tune the internal real-time RC clock to a target frequency, and
 * get the tuning value.
 *
 * Note: This operation costs ~250ms.
 *
 * @param[in]   target_frequency    target frequency of tuning (Hz)
 * @return                          Value used to tune the clock
 ****************************************************************************************
 */
uint16_t platform_rt_rc_auto_tune2(uint32_t target_frequency);

/**
 ****************************************************************************************
 * @brief generate random bytes by using true hardware random-number generator
 *
 * @param[out] bytes            random data output
 * @param[in]  len              byte number of random data
 ****************************************************************************************
 */
void platform_hrng(uint8_t *bytes, const uint32_t len);

/**
 ****************************************************************************************
 * @brief generate a pseudo random integer by internal PRNG whose seed initialized by HRNG
 *        at startup.
 *
 * @return                     a pseudo random integer in range of 0 to RAND_MAX
 ****************************************************************************************
 */
int platform_rand(void);

/**
 ****************************************************************************************
 * @brief Read the internal timer counting from initialization.
 *        Note: This timer restarts after shutdown, while RTC timer does not.
 *
 * @return                     internal timer counting at 1us.
 ****************************************************************************************
 */
uint64_t platform_get_us_time(void);

/**
 ****************************************************************************************
 * @brief the printf function
 *
 * @param[in]  format           format string
 * @param[in]  ...              arguments
 ****************************************************************************************
 */
void platform_printf(const char *format, ...);

/**
 ****************************************************************************************
 * @brief Set public device address
 *
 * NOTE: This is for debug only.
 *
 * @param[in]  addr             device address in little-endian.
 ****************************************************************************************
 */
void sysSetPublicDeviceAddr(const unsigned char *addr);

// set rf source
// 0: use external crystal
// 1: use internal clock loopback
void platform_set_rf_clk_source(const uint8_t source);

void platform_set_rf_init_data(const uint32_t *rf_init_data);

void platform_set_rf_power_mapping(const int16_t *rf_power_mapping);

/**
 ****************************************************************************************
 * @brief Patch RF initialization data
 *
 * @param[in]  data             patch data
 ****************************************************************************************
 */
void platform_patch_rf_init_data(const void *data);

/**
 ****************************************************************************************
 * @brief Pre-suppress ticks and sleep processing
 *
 * @param[in]  expected_ticks   expected ticks to sleep
 * @return                      adjusted ticks to sleep
 ****************************************************************************************
 */
uint32_t platform_pre_suppress_ticks_and_sleep_processing(uint32_t expected_ticks);

/**
 ****************************************************************************************
 * @brief Preprocessing for tickless sleep
 ****************************************************************************************
 */
void platform_pre_sleep_processing(void);

/**
 ****************************************************************************************
 * @brief Postprocessing for tickless sleep
 ****************************************************************************************
 */
void platform_post_sleep_processing(void);

/**
 ****************************************************************************************
 * @brief Hook for idle task got resumed
 ****************************************************************************************
 */
void platform_os_idle_resumed_hook(void);

typedef enum
{
    PLATFORM_TASK_CONTROLLER,
    PLATFORM_TASK_HOST,
    PLATFORM_TASK_RTOS_TIMER,
} platform_task_id_t;

/**
 ****************************************************************************************
 * @brief Get RTOS handle of a specific platform task
 *
 * @param[in]   id              task identifier
 * @return                      task handle if such task is known to platform bundles else 0.
 *                              0 is returned
 *                              For FreeRTOS bundles, this is casted from `TaskHandle_t`;
 *                              For NoOS bundles, this is casted from `gen_handle_t`.
 ****************************************************************************************
 */
uintptr_t platform_get_task_handle(platform_task_id_t id);

/**
 ****************************************************************************************
 * @brief Init controller
 *
 * For NoOS bundles, and `task_create` is NULL in generic OS driver:
 *
 *     To use raw packet APIs, controller needs to be initialized, and call
 *     `platform_controller_run()` continously.
 ****************************************************************************************
 */
void platform_init_controller(void);

/**
 ****************************************************************************************
 * @brief Run controller
 *
 * Controller will do its pending jobs, and return after all pending jobs are done.
 ****************************************************************************************
 */
void platform_controller_run(void);

typedef void (* f_platform_timer_callback)(void);

/**
 ****************************************************************************************
 * @brief Setup a single-shot platform timer
 *
 * Note: 1. Comparing to hardware timers, this timer can be thought as "running" during
 *          power saving;
 *       1. Comparing to RTOS software timers, this timer is software + hardware too,
 *       1. Comparing to RTOS software timers, this timer may be more accurate in some
 *          circumstance;
 *       1. This will always succeed, except when running out of memory;
 *       1. `callback` is also the identifier of the timer, below two lines defines only
 *          a timer expiring after 200 units but not two separate timers:
 *          ```c
 *          platform_set_timer(f, 100);
 *          platform_set_timer(f, 200);
 *          ```
 *
 * To configure a timer at an absolute time, see `platform_set_abs_timer` and
 * `platform_get_timer_counter`.
 *
 * `platform_set_timer(f, 100)` is equivalent to:
 *
 * ```c
 * platform_set_abs_timer(f, platform_get_timer_counter() + 100);
 * ```
 *
 * `platform_set_timer(f, 0)` is equivalent to:
 *
 * ```c
 * platform_delete_timer(f);
 * ```
 *
 * @param[in]  callback         the callback function when the timer expired, and is
 *                              called in a RTOS task (if existing) not an ISR
 * @param[in]  delay            time delay before the timer expires (unit: 625us)
 *                              Range: 0~0x7fffffff
 *                              When `delay` == 0, the timer is cleared
 ****************************************************************************************
 */
void platform_set_timer(f_platform_timer_callback callback, uint32_t delay);

/**
 ****************************************************************************************
 * @brief Read the counter of platform timer
 *
 * @return                      current counter (full 32 bits)
 ****************************************************************************************
 */
uint32_t platform_get_timer_counter(void);

/**
 ****************************************************************************************
 * @brief Setup a single-shot platform timer triggered at an absolute time
 *
 * @param[in]  callback         the callback function when the timer expired, and is
 *                              called in a RTOS task (if existing) not an ISR
 * @param[in]  abs_time         when `platform_get_timer_counter() == abs_time`, callback is invoked.
 *                              If `abs_time` just passes `platform_get_timer_counter()`, `callback` is invoked immediately.
 ****************************************************************************************
 */
void platform_set_abs_timer(f_platform_timer_callback callback, uint32_t abs_time);

/**
 ****************************************************************************************
 * @brief Delete a previously configured platform timer
 *
 * @param[in]  callback         the callback function identifying the timer
 ****************************************************************************************
 */
void platform_delete_timer(f_platform_timer_callback callback);

typedef void * platform_us_timer_handle_t;

/**
 ****************************************************************************************
 * @brief Callback function of microsecond (us) resolution timer
 *
 * @param[in]   timer_handle    handle of this timer
 * @param[in]   time_us         internal timer counter when invoke this callback
 * @param[in]   param           user parameter
 * @return                      (must be NULL)
 ****************************************************************************************
 */
typedef void * (* f_platform_us_timer_callback)(platform_us_timer_handle_t timer_handle,
    uint64_t time_us, void *param);

/**
 ****************************************************************************************
 * @brief Setup a single-shot platform timer with microsecond (us) resolution
 *
 * Although `abs_time` is in microsecond (us), callback is **not guaranteed**
 * to be invoked with such resolution.
 *
 * This type of timers are much like `platform_set_timer`, except that:
 * 1. resolution is higher;
 * 2. callback is invoked in the context of an ISR
 *
 * CAUTION: DO NOT call `platform_create_us_timer` again in `callback`.
 *
 * @param[in]  abs_time         when `platform_get_us_time() == abs_time`, callback is invoked.
 * @param[in]  callback         the callback function
 * @param[in]  param            user parameter
 * @return                      a non-NULL value when succeeded. Otherwise, NULL.
 ****************************************************************************************
 */
platform_us_timer_handle_t platform_create_us_timer(uint64_t abs_time,
    f_platform_us_timer_callback callback, void *param);

/**
 ****************************************************************************************
 * @brief Cancel a platform timer previously created by `platform_create_us_timer`
 *
 * When a timer not needed any more, use this API to cancel it.
 *
 * @param[in]  timer_handle     handle of the timer
 * @return                      0 if succeeded else non-0
 *                              Note: non-0 means the callback function of the timer
 *                                    is executing.
 ****************************************************************************************
 */
int platform_cancel_us_timer(platform_us_timer_handle_t timer_handle);

/**
 ****************************************************************************************
 * @brief Install a new RTOS stack for a specific platform task
 *
 * For NoOS variants, RTOS stacks can be replaced (modify its size, etc) when implementing
 * the generic OS interface.
 *
 * @param[in]   id              task identifier
 * @param[in]   start           start (lowest) address of the new stack
 * @param[in]   size            size of the new stack in bytes
 ****************************************************************************************
 */
// void platform_install_task_stack(platform_task_id_t id, void *start, uint32_t size);
// WARNING: ^^^ this API is not available in this release


/**
 ****************************************************************************************
 * @brief Get generic OS driver
 *
 * For NoOS variants, driver provided by app is returned;
 * For RTOS variants, an emulated driver is returned.
 *
 * @return                       driver pointer casted from `const gen_os_driver_t *`
 ****************************************************************************************
 */
const void *platform_get_gen_os_driver(void);

/**
 ****************************************************************************************
 * @brief Get current task in which is calling this API
 *
 * Only available for RTOS variants
 *
 * @return                       PLATFORM_TASK_CONTROLLER or PLATFORM_TASK_HOST or other
 *                               when it is neither in Host or Controller task, another value
 *                               is returned.
 ****************************************************************************************
 */
// platform_task_id_t platform_get_current_task(void);
// WARNING: ^^^ this API is not available in this release


typedef struct platform_hci_link_layer_interf
{
    /**
     * @brief try to send HCI command to Controller
     *
     * @param[in]   opcode      op code
     * @param[in]   param       parameters
     * @param[in]   param_len   length of paramters
     * @return                  0 if OK else command is not sent
     */
    int (*send_hci_command)(uint16_t opcode, const uint8_t *param, uint16_t param_len);

    /**
     * @brief try to send ACL data to Controller
     *
     * @param[in]   conn_handle     connection handle
     * @param[in]   bc_pb_flag      BC/PB flag
     * @param[in]   total_len       total length of data
     * @param[in]   ...             [const uint8_t *data, int size]..
     *                              where sum(size) shall be `total_len`
     * @return                      0 if OK else data is not sent
     */
    int (*send_acl_data)(uint16_t conn_handle, uint8_t bc_pb_flag, uint16_t total_len, ...);

    /**
     * @brief call this API after an HCI event is processed in `PLATFORM_CB_EVT_HCI_RECV`
     *        callback.
     *
     * @param[in]   handle          `handle` in `platform_hci_recv_t`
     */
    void (*hci_event_processed)(const void *param);

    /**
     * @brief call this API after an ACL data is processed in `PLATFORM_CB_EVT_HCI_RECV`
     *        callback.
     *
     * @param[in]   conn_handle     `conn_handle` in `platform_hci_recv_t`
     * @param[in]   handle          `handle` in `platform_hci_recv_t`
     */
    void (*acl_data_processed)(const uint16_t conn_handle, const void *handle);
} platform_hci_link_layer_interf_t;

/**
 ****************************************************************************************
 * @brief Get link layer driver API
 *
 * This driver is only available when `PLATFORM_CB_EVT_HCI_RECV` is defined.
 *
 * @return                       driver pointer casted from `const link_layer_driver_t *`
 ****************************************************************************************
 */
const platform_hci_link_layer_interf_t *platform_get_link_layer_interf(void);

typedef void (*f_platform_function)(void *user_data);

/**
 ****************************************************************************************
 * @brief Call a function on a separate dedicated stack
 *
 * @param[in]   f               the function to be called
 * @param[in]   user_data       user data passed to `f`
 * @param[in]   start           start (lowest) address of the dedicated stack
 * @param[in]   size            size of the dedicated stack in bytes
 ****************************************************************************************
 */
void platform_call_on_stack(f_platform_function f, void *user_data,
                            void *stack_start, uint32_t stack_size);

#ifdef __cplusplus
}
#endif

#endif
