#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "blink.h"

#define PRINT_UART    APB_UART0

uint32_t cb_putc(char *c, void *dummy)
{
    while (apUART_Check_TXFIFO_FULL(PRINT_UART) == 1);
    UART_SendData(PRINT_UART, (uint8_t)*c);
    return 0;
}

int fputc(int ch, FILE *f)
{
    cb_putc((char *)&ch, NULL);
    return ch;
}

void config_uart(uint32_t freq, uint32_t baud)
{
    UART_sStateStruct UART_0;

    UART_0.word_length       = UART_WLEN_8_BITS;
    UART_0.parity            = UART_PARITY_NOT_CHECK;
    UART_0.fifo_enable       = 1;
    UART_0.two_stop_bits     = 0;
    UART_0.receive_en        = 1;
    UART_0.transmit_en       = 1;
    UART_0.UART_en           = 1;
    UART_0.cts_en            = 0;
    UART_0.rts_en            = 0;
    UART_0.rxfifo_waterlevel = 1;
    UART_0.txfifo_waterlevel = 1;
    UART_0.ClockFrequency    = freq;
    UART_0.BaudRate          = baud;

    apUART_Initialize(PRINT_UART, &UART_0, 0);
}

#define PIN_RED     5
#define PIN_GREEN   1
#define PIN_BLUE    7

void blink(const uint8_t led_id)
{
    const static uint8_t led_pins[] = {PIN_RED, PIN_RED, PIN_BLUE, PIN_GREEN};
    uint8_t channel_index = led_pins[led_id] >> 1;
    blink_style(channel_index, BLINK_SINGLE); 
}

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);

    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_PWM));

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    PINCTRL_SetPadMux(PIN_RED, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(PIN_RED, 1);
    PINCTRL_SetPadMux(PIN_GREEN, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(PIN_GREEN, 1);
    PINCTRL_SetPadMux(PIN_BLUE, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(PIN_BLUE, 1);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    #error WIP
#else
    #error unknown or unsupported chip family
#endif
}

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    setup_peripherals();

    // setup putc handle
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    return 0;
}

