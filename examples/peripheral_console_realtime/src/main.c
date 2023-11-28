#include <stdio.h>
#include <string.h>
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "port_gen_os_driver.h"
#include "profile.h"

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

#define PRINT_PORT    APB_UART0

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

ADDITIONAL_ATTRIBUTE void config_uart(uint32_t freq, uint32_t baud)
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
}

#define PIN_BUZZER 8

ADDITIONAL_ATTRIBUTE void setup_peripherals(void)
{
    SYSCTRL_SetClkGateMulti((1 << SYSCTRL_ClkGate_APB_UART0));

#ifdef LISTEN_TO_POWER_SAVING
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_GPIO0)
                              | (1 << SYSCTRL_ClkGate_APB_PinCtrl));

    PINCTRL_SetPadMux(PIN_BUZZER, IO_SOURCE_GPIO);
    GIO_SetDirection(PIN_BUZZER, GIO_DIR_OUTPUT);
    GIO_WriteValue(PIN_BUZZER, 1);
#endif
}

ADDITIONAL_ATTRIBUTE uint32_t on_deep_sleep_wakeup(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);
#ifdef USE_POWER_LIB
    power_ctrl_deep_sleep_wakeup();
#endif

#ifdef LISTEN_TO_POWER_SAVING
    setup_peripherals();
#endif
    return 0;
}

ADDITIONAL_ATTRIBUTE uint32_t query_deep_sleep_allowed(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);
    if (IS_DEBUGGER_ATTACHED())
        return 0;

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#ifdef USE_POWER_LIB
    power_ctrl_before_deep_sleep();
#endif
    return PLATFORM_ALLOW_DEEP_SLEEP;
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    return PLATFORM_ALLOW_DEEP_SLEEP | PLATFORM_ALLOW_BLE_ONLY_SLEEP;
#endif
}

uintptr_t app_main()
{
    platform_config(PLATFORM_CFG_RT_OSC_EN, PLATFORM_CFG_DISABLE);
    platform_config(PLATFORM_CFG_RT_CLK_ACC, 500);

#ifdef USE_POWER_LIB
    power_ctrl_init();
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    #define HCLK_DIV SYSCTRL_CLK_PLL_DIV_5

    platform_config(PLATFORM_CFG_DEEP_SLEEP_TIME_REDUCTION, 4100 - 400);
    platform_config(PLATFORM_CFG_LL_DELAY_COMPENSATION, 155);

    SYSCTRL_EnableConfigClocksAfterWakeup(1,
        PLL_HW_DEF_LOOP,
        HCLK_DIV,
        SYSCTRL_CLK_PLL_DIV_2,
        0);

    SYSCTRL_EnableSlowRC(0, SYSCTRL_SLOW_RC_24M);
    SYSCTRL_SelectHClk(HCLK_DIV);
    SYSCTRL_SelectFlashClk(SYSCTRL_CLK_PLL_DIV_2);

    SYSCTRL_SelectMemoryBlocks(SYSCTRL_RESERVED_MEM_BLOCKS);
#endif

    // setup handlers
    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_HARD_FAULT, (f_platform_evt_cb)cb_hard_fault, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ASSERTION, (f_platform_evt_cb)cb_assertion, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_HEAP_OOM, (f_platform_evt_cb)cb_heap_out_of_mem, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, on_deep_sleep_wakeup, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, query_deep_sleep_allowed, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    setup_peripherals();
    platform_config(PLATFORM_CFG_POWER_SAVING, 1);

    return (uintptr_t)os_impl_get_driver();
}

const char welcome_msg[] = "Built without RTOS";
