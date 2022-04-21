#define OPTIONAL_RF_CLK
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "io_interf.h"
#include "blink.h"
#include "rf_util.h"

uint32_t cb_hard_fault(hard_fault_info_t *info, void *_)
{
    platform_printf("HARDFAULT:\nPC : 0x%08X\nLR : 0x%08X\nPSR: 0x%08X\n"
                    "R0 : 0x%08X\nR1 : 0x%08X\nR2 : 0x%08X\nP3 : 0x%08X\n"
                    "R12: 0x%08X\n",
                    info->pc, info->lr, info->psr, 
                    info->r0, info->r1, info->r2, info->r3, info->r12);
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

#if (IO_TYPE == IO_TYPE_UART_STR)
    apUART_Initialize(PRINT_PORT, &config, 1 << bsUART_RECEIVE_INTENAB);
#else
    apUART_Initialize(PRINT_PORT, &config, 0);
#endif
}

#define LED_PIN         6

void show_state(const io_state_t state)
{
    switch (state)
    {
    case STATE_SCANNING:
        blink_style(LED_PIN >> 1, BLINK_SLOW);
        break;
    case STATE_CONNECTING:
        blink_style(LED_PIN >> 1, BLINK_NORMAL);
        break;
    case STATE_DISCOVERING:
        blink_style(LED_PIN >> 1, BLINK_FAST);
        break;
    case STATE_RX:
    case STATE_TX:
        blink_style(LED_PIN >> 1, BLINK_SINGLE);
        break;
    default:
        blink_style(LED_PIN >> 1, BLINK_OFF);
        break;
    }
}

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);

    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_PWM));

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    PINCTRL_SetPadMux(LED_PIN, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(LED_PIN, 1);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    PINCTRL_SetPadMux(LED_PIN, IO_SOURCE_PWM6_B);
#else
    #error unknown or unsupported chip family
#endif

    io_interf_setup_peripherals();
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

int app_main()
{
    /// RF power boost
    rf_enable_powerboost();

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    // setup handlers
    platform_set_evt_callback(PLATFORM_CB_EVT_HARD_FAULT, (f_platform_evt_cb)cb_hard_fault, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, on_deep_sleep_wakeup, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, query_deep_sleep_allowed, NULL);    
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    setup_peripherals();
    
    io_interf_init();

    return 0;
}
