#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include <stdio.h>
#include "rf_util.h"
#include "board.h"

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

#include "impl_led.c"
void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);

    setup_rgb_led();
}

const static rgb_t rgb0 = { .r = 0, .g = 0, .b = 0 };
const static rgb_t rgb1 = { .r = 50, .g = 0, .b = 0 };

void set_led_color(uint8_t r, uint8_t g, uint8_t b)
{
    rgb_t rgb = { .r = r, .g = g, .b = b };
    set_rbg_breathing(rgb, rgb);
}

void start_led_breathing(void)
{
    set_rbg_breathing(rgb0, rgb1);
}

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    rf_enable_powerboost();

    setup_peripherals();

    // setup putc handle
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);
    
    setup_rgb_breathing();
    start_led_breathing();
    
    return 0;
}
