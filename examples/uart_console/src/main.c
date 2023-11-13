#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include <stdio.h>
#include "trace.h"
#include "eflash.h"
#include "uart_console.h"

uint32_t cb_hard_fault(hard_fault_info_t *info, void *_)
{
    platform_printf("HARDFAULT:\nPC : 0x%08X\nLR : 0x%08X\nPSR: 0x%08X\n"
                    "R0 : 0x%08X\nR1 : 0x%08X\nR2 : 0x%08X\nP3 : 0x%08X\n"
                    "R12: 0x%08X\n",
                    info->pc, info->lr, info->psr,
                    info->r0, info->r1, info->r2, info->r3, info->r12);
    for (;;);
}

uint32_t cb_assertion(assertion_info_t *info, void *_)
{
    platform_printf("[ASSERTION] @ %s:%d\n",
                    info->file_name,
                    info->line_no);
    trace_full_dump(puts, 0);
    for (;;);
}

#define PRINT_PORT    APB_UART0
#define TRACE_PORT    APB_UART1

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
    config.rxfifo_waterlevel = 4;
    config.txfifo_waterlevel = 1;
    config.ClockFrequency    = freq;
    config.BaudRate          = baud;

    apUART_Initialize(PRINT_PORT, &config, (1 << bsUART_RECEIVE_INTENAB) | (1 << bsUART_TIMEOUT_INTENAB));

#ifdef TRACE_TO_UART
    //config.BaudRate          = 921600;
    apUART_Initialize(TRACE_PORT, &config, 1 << bsUART_TRANSMIT_INTENAB);
#endif
}

void setup_peripherals(void)
{
#ifdef TRACE_TO_UART
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_UART1)
                            | (1 << SYSCTRL_ClkGate_APB_PinCtrl));
    PINCTRL_SetPadMux(19, IO_SOURCE_UART1_TXD);
#endif

    config_uart(OSC_CLK_FREQ, 115200);
}

void console_rx_data(const char *d, uint8_t len);

uint32_t uart_isr(void *user_data)
{
    uint32_t status;

    while(1)
    {
        status = apUART_Get_all_raw_int_stat(APB_UART0);
        if (status == 0)
            break;

        APB_UART0->IntClear = status;

        while (apUART_Check_RXFIFO_EMPTY(APB_UART0) != 1)
        {
            char c = APB_UART0->DataRead;
            console_rx_data(&c, 1);
        }
    }
    return 0;
}

#if (defined TRACE_TO_AIR)
trace_air_t trace_ctx = {0};
#elif (defined TRACE_TO_UART)
trace_uart_t trace_ctx = {.port = TRACE_PORT};
#elif (defined TRACE_TO_FLASH)
trace_flash_t trace_ctx = {0};
#else
trace_rtt_t trace_ctx = {0};
#endif

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    platform_set_evt_callback(PLATFORM_CB_EVT_HARD_FAULT, (f_platform_evt_cb)cb_hard_fault, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ASSERTION, (f_platform_evt_cb)cb_assertion, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);
    platform_set_irq_callback(PLATFORM_CB_IRQ_UART0, uart_isr, NULL);

    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    setup_peripherals();
    uart_console_start();
    printf("system started, type ? for help\n");

#if (defined TRACE_TO_AIR)
    trace_air_init(&trace_ctx, USER_MSG_ID_TRACE, 244);
    platform_set_evt_callback(PLATFORM_CB_EVT_TRACE, (f_platform_evt_cb)cb_trace_air, &trace_ctx);
#elif (defined TRACE_TO_UART)
    trace_uart_init(&trace_ctx);
    platform_set_irq_callback(PLATFORM_CB_IRQ_UART1, (f_platform_irq_cb)trace_uart_isr, &trace_ctx);
    platform_set_evt_callback(PLATFORM_CB_EVT_TRACE, (f_platform_evt_cb)cb_trace_uart, &trace_ctx);
#elif (defined TRACE_TO_FLASH)
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    trace_flash_init(&trace_ctx, 0x2e000, EFLASH_PAGE_SIZE * 11);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    trace_flash_init(&trace_ctx, 0x2100000, 0x100000);
#else
    #error unknown or unsupported chip family
#endif
    platform_set_evt_callback(PLATFORM_CB_EVT_TRACE, (f_platform_evt_cb)cb_trace_flash, &trace_ctx);
#else
    trace_rtt_init(&trace_ctx);
    platform_set_evt_callback(PLATFORM_CB_EVT_TRACE, (f_platform_evt_cb)cb_trace_rtt, &trace_ctx);
#endif

    platform_config(PLATFORM_CFG_TRACE_MASK, 0x3f);
    return 0;
}

