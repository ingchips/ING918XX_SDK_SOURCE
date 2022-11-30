#include <stdio.h>
#include <string.h>
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "trace.h"
#include "eflash.h"
#include "kv_storage.h"
#include "brpc.h"
#include "uart_driver.h"


#define PRINT_PORT    APB_UART1
#define  COMM_PORT    APB_UART0
#define COMM_ISR_ID   PLATFORM_CB_IRQ_UART0

#define PIN_UART1_TX   4
#define PIN_UART1_RX   5

uint32_t cb_hard_fault(hard_fault_info_t *info, void *_);
uint32_t cb_assertion(assertion_info_t *info, void *_);
uint32_t cb_heap_out_of_mem(uint32_t tag, void *_);
uint32_t cb_exception(uint32_t tag, void *_);

uint32_t cb_putc(char *c, void *dummy)
{
    while (apUART_Check_TXFIFO_FULL(PRINT_PORT) == 1);
    UART_SendData(PRINT_PORT, (uint8_t)*c);
    return 0;
}

int fputc(int ch, FILE *f)
{
    cb_putc((char *)&ch, NULL);
    return ch;
}

void config_uart(uint32_t freq, uint32_t baud)
{
    UART_sStateStruct config;

    config.word_length       = UART_WLEN_8_BITS;
    config.parity            = UART_PARITY_NOT_CHECK;
    config.fifo_enable       = 1;
    config.two_stop_bits     = 0;
    config.receive_en        = 1;
    config.transmit_en       = 1;
    config.UART_en           = 1;
    config.cts_en            = 0;
    config.rts_en            = 0;
    config.rxfifo_waterlevel = 1;
    config.txfifo_waterlevel = 1;
    config.ClockFrequency    = freq;
    config.BaudRate          = baud;

    apUART_Initialize(PRINT_PORT, &config, 0);
    apUART_Initialize(COMM_PORT, &config, (1 << bsUART_RECEIVE_INTENAB) | (1 << bsUART_TRANSMIT_INTENAB));
}

void setup_peripherals(void)
{
    SYSCTRL_ClearClkGateMulti(0
                            | (1 << SYSCTRL_ClkGate_APB_TMR0)
                            | (1 << SYSCTRL_ClkGate_APB_UART1));

    // uart1:
    PINCTRL_SetPadMux(PIN_UART1_RX, IO_SOURCE_GENERAL);
    PINCTRL_SelUartRxdIn(UART_PORT_1, PIN_UART1_RX);
    PINCTRL_SetPadMux(PIN_UART1_TX, IO_SOURCE_UART1_TXD);

    config_uart(OSC_CLK_FREQ, 115200);

    PINCTRL_EnableAllAntSelPins();
}

uint32_t on_deep_sleep_wakeup(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);
    setup_peripherals();
    return 0;
}

uint32_t query_deep_sleep_allowed(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);
    // TODO: return 0 if deep sleep is not allowed now; else deep sleep is allowed
    return 0;
}

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#define DB_FLASH_ADDRESS  0x42000
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
#define DB_FLASH_ADDRESS  0x2100000
#else
#error unknown or unsupported chip family
#endif

int db_write_to_flash(const void *db, const int size)
{
    platform_printf("write to flash, size = %d\n", size);
    program_flash(DB_FLASH_ADDRESS, (const uint8_t *)db, size);
    return KV_OK;
}

int read_from_flash(void *db, const int max_size)
{
    memcpy(db, (void *)DB_FLASH_ADDRESS, max_size);
    return KV_OK;
}

trace_rtt_t trace_ctx = {0};

int app_main()
{
    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    // setup handlers
    platform_set_evt_callback(PLATFORM_CB_EVT_HARD_FAULT, (f_platform_evt_cb)cb_hard_fault, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ASSERTION, (f_platform_evt_cb)cb_assertion, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_HEAP_OOM, (f_platform_evt_cb)cb_heap_out_of_mem, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_EXCEPTION, (f_platform_evt_cb)cb_exception, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, on_deep_sleep_wakeup, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, query_deep_sleep_allowed, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    setup_peripherals();

    kv_init(db_write_to_flash, read_from_flash);

    uart_driver_init(COMM_PORT, NULL, (f_uart_rx_byte)brpc_rx_byte);
    platform_set_irq_callback(COMM_ISR_ID, uart_driver_isr, NULL);

    trace_rtt_init(&trace_ctx);
    platform_set_evt_callback(PLATFORM_CB_EVT_TRACE, (f_platform_evt_cb)cb_trace_rtt, &trace_ctx);
    // TODO: config trace mask
    platform_config(PLATFORM_CFG_TRACE_MASK, 0);

    return 0;
}

