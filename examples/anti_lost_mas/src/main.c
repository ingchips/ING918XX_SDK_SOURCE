#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include <stdio.h>
#include "blink.h"

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

    apUART_Initialize(PRINT_PORT, &config, 0);
}

static TimerHandle_t timer;

#define BUZZ_IO     8
#define LED_IO      9

#include "board.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
    PINCTRL_DisableAllInputs();

    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_PWM));

    PINCTRL_SetPadMux(LED_IO, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(LED_IO, 1);

    setup_buzzer();
}

void adv_received()
{
    blink_style(BUZZ_IO >> 1, BLINK_OFF);
    blink_style(LED_IO >> 1, BLINK_SINGLE);
    xTimerReset(timer,  portMAX_DELAY);
}

void vTimerCallback(TimerHandle_t _)
{
    set_buzzer_freq(500);
}
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
    PINCTRL_DisableAllInputs();

    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_PWM)
                              | (1 << SYSCTRL_ClkGate_APB_GPIO0));

    PINCTRL_SetPadMux(LED_IO, IO_SOURCE_GPIO);
    PINCTRL_SetPadMux(BUZZ_IO, IO_SOURCE_GPIO);
    GIO_SetDirection(LED_IO, GIO_DIR_OUTPUT);
    GIO_SetDirection(BUZZ_IO, GIO_DIR_OUTPUT);
    GIO_WriteValue(LED_IO, 1);
    GIO_WriteValue(BUZZ_IO, 1);
}

void adv_received()
{
    PINCTRL_SetPadMux(LED_IO, IO_SOURCE_PWM0_B);
    PINCTRL_SetPadMux(BUZZ_IO, IO_SOURCE_GPIO);
    blink_style(0, BLINK_SINGLE);
    xTimerReset(timer,  portMAX_DELAY);
}

void vTimerCallback(TimerHandle_t _)
{
    PINCTRL_SetPadMux(LED_IO, IO_SOURCE_GPIO);
    PINCTRL_SetPadMux(BUZZ_IO, IO_SOURCE_PWM0_A);
    PWM_SetupSimple(0, 500, 50);
}
#else
#error unknown or unsupported chip family
#endif

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
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    timer = xTimerCreate("t",
                 pdMS_TO_TICKS(10 * 1000),
                 pdFALSE,
                 NULL,
                 vTimerCallback);
    xTimerStart(timer, 0);

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    // setup handlers
    platform_set_evt_callback(PLATFORM_CB_EVT_HARD_FAULT, (f_platform_evt_cb)cb_hard_fault, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, on_deep_sleep_wakeup, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, query_deep_sleep_allowed, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    setup_peripherals();

    // platform_config(PLATFORM_CFG_LOG_HCI, PLATFORM_CFG_ENABLE);
    return 0;
}



