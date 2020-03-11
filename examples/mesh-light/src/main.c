#include "ingsoc.h"
#include "platform_api.h"
#include "peripheral_pwm.h"
#include <stdio.h>
#include "profile.h"

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

__weak void __aeabi_assert()
{
    printf("assert\n");
    for (;;);
}

void config_uart(uint32_t freq, uint32_t baud)
{
    UART_sStateStruct UART_0;

    UART_0.word_length       = UART_WLEN_8_BITS;
    UART_0.parity            = UART_PARITY_NOT_CHECK;
    UART_0.fifo_enable       = 1;
    UART_0.two_stop_bits     = 0;   //used 2 stop bit ,    0
    UART_0.receive_en        = 1;
    UART_0.transmit_en       = 1;
    UART_0.UART_en           = 1;
    UART_0.cts_en            = 0;
    UART_0.rts_en            = 0;
    UART_0.rxfifo_waterlevel = 1;   //UART_FIFO_ONE_SECOND;
    UART_0.txfifo_waterlevel = 1;   //UART_FIFO_ONE_SECOND;
    UART_0.ClockFrequency    = freq;
    UART_0.BaudRate          = baud;

    apUART_Initialize(PRINT_UART, &UART_0, 0);
}

#define CHANNEL_RED     4
#define CHANNEL_GREEN   0
#define CHANNEL_BLUE    6

#define PERA_THRESHOLD (OSC_CLK_FREQ / 1000)

void set_led_color(uint8_t r, uint8_t g, uint8_t b)
{
#ifndef SIMULATION      
#define TO_PERCENT(v) (((uint32_t)(v) * 100) >> 8)

    PWM_SetHighThreshold(CHANNEL_RED   >> 1, 0, PERA_THRESHOLD / 100 * TO_PERCENT(r));
    PWM_SetHighThreshold(CHANNEL_GREEN >> 1, 0, PERA_THRESHOLD / 100 * TO_PERCENT(g >> 1));  // GREEN & BLUE led seems too bright
    PWM_SetHighThreshold(CHANNEL_BLUE  >> 1, 0, PERA_THRESHOLD / 100 * TO_PERCENT(b >> 1));
#endif    
}

static void setup_channel(uint8_t channel_index)
{
    PWM_HaltCtrlEnable(channel_index, 1);
    PWM_Enable(channel_index, 0);
    PWM_SetPeraThreshold(channel_index, PERA_THRESHOLD);
    PWM_SetMultiDutyCycleCtrl(channel_index, 0);        // do not use multi duty cycles
    PWM_SetHighThreshold(channel_index, 0, PERA_THRESHOLD / 2);
    PWM_SetMode(channel_index, PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE);
    PWM_SetMask(channel_index, 0, 0);
    PWM_Enable(channel_index, 1); 
    PWM_HaltCtrlEnable(channel_index, 0);
}

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
    
#ifndef SIMULATION    
    PINCTRL_SetPadMux(CHANNEL_RED, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(CHANNEL_RED, 1);
    PINCTRL_SetPadMux(CHANNEL_GREEN, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(CHANNEL_GREEN, 1);
    PINCTRL_SetPadMux(CHANNEL_BLUE, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(CHANNEL_BLUE, 1);
    
    setup_channel(CHANNEL_RED   >> 1);
    setup_channel(CHANNEL_GREEN >> 1);
    setup_channel(CHANNEL_BLUE  >> 1);
    
    set_led_color(50, 50, 50);
#endif
}

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    setup_peripherals();

    // setup putc handle
    //platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);
    // platform_config(PLATFORM_CFG_LOG_HCI, PLATFORM_CFG_ENABLE);

    return 0;
}

