#include <stdio.h>
#include <string.h>
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "trace.h"
#include "uart_driver.h"
#include "rf_util.h"

static uint32_t cb_hard_fault(hard_fault_info_t *info, void *_)
{
    platform_printf("HARDFAULT:\nPC : 0x%08X\nLR : 0x%08X\nPSR: 0x%08X\n"
                    "R0 : 0x%08X\nR1 : 0x%08X\nR2 : 0x%08X\nP3 : 0x%08X\n"
                    "R12: 0x%08X\n",
                    info->pc, info->lr, info->psr,
                    info->r0, info->r1, info->r2, info->r3, info->r12);
    for (;;);
}

static uint32_t cb_assertion(assertion_info_t *info, void *_)
{
    platform_printf("[ASSERTION] @ %s:%d\n",
                    info->file_name,
                    info->line_no);
    for (;;);
}

static uint32_t cb_heap_out_of_mem(uint32_t tag, void *_)
{
    platform_printf("[OOM] @ %d\n", tag);
    for (;;);
}

#define COMM_PORT    APB_UART0

uint32_t cb_putc(char *c, void *dummy)
{
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

    apUART_Initialize(COMM_PORT, &config, UART_INTBIT_RECEIVE | UART_INTBIT_TRANSMIT);
}

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
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

trace_rtt_t trace_ctx = {0};

extern void rx_hci_byte(void *user_data, uint8_t c);

int app_main()
{
#ifndef BQTF_TEST
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    switch (platform_read_persistent_reg())
    {
    case POWER_MODE_BOOSTED:
        rf_enable_powerboost();
        break;
    case POWER_MODE_NORMAL:
        SYSCTRL_SetBuckDCDCOutput(SYSCTRL_BUCK_DCDC_OUTPUT_1V500);
        SYSCTRL_SetLDOOutputRF(SYSCTRL_LDO_RF_OUTPUT_1V300);
        break;
    default:
        platform_write_persistent_reg(POWER_MODE_NORMAL);
        break;
    }
#endif
#endif

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    // setup handlers
    platform_set_evt_callback(PLATFORM_CB_EVT_HARD_FAULT, (f_platform_evt_cb)cb_hard_fault, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ASSERTION, (f_platform_evt_cb)cb_assertion, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_HEAP_OOM, (f_platform_evt_cb)cb_heap_out_of_mem, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, on_deep_sleep_wakeup, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, query_deep_sleep_allowed, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    setup_peripherals();

    uart_driver_init(COMM_PORT, NULL, rx_hci_byte);
    platform_set_irq_callback(PLATFORM_CB_IRQ_UART0, uart_driver_isr, NULL);

    trace_rtt_init(&trace_ctx);
    platform_set_evt_callback(PLATFORM_CB_EVT_TRACE, (f_platform_evt_cb)cb_trace_rtt, &trace_ctx);
    platform_config(PLATFORM_CFG_TRACE_MASK, 0);
    return 0;
}

