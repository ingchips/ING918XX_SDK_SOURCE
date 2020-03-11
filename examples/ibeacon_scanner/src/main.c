#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

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

#define PERA_THRESHOLD (OSC_CLK_FREQ / 2)

static void setup_channel(uint8_t channel_index)
{
    PWM_HaltCtrlEnable(channel_index, 1);
    PWM_Enable(channel_index, 0);
    PWM_SetPeraThreshold(channel_index, PERA_THRESHOLD);
    PWM_SetMultiDutyCycleCtrl(channel_index, 0);        // do not use multi duty cycles
    PWM_SetHighThreshold(channel_index, 0, PERA_THRESHOLD / 2);
    PWM_SetMode(channel_index, PWM_WORK_MODE_SINGLE_WITHOUT_DIED_ZONE);
    PWM_SetMask(channel_index, 0, 0);
    PWM_Enable(channel_index, 1); 
    PWM_HaltCtrlEnable(channel_index, 0);
}

void blink(const uint8_t led_id)
{
    const static uint8_t led_pins[] = {PIN_RED, PIN_RED, PIN_BLUE, PIN_GREEN};
    uint8_t channel_index = led_pins[led_id] >> 1;
    PWM_Enable(channel_index, 0);
    PWM_Enable(channel_index, 1); 
}

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
    
    PINCTRL_SetPadMux(PIN_RED, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(PIN_RED, 1);
    PINCTRL_SetPadMux(PIN_GREEN, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(PIN_GREEN, 1);
    PINCTRL_SetPadMux(PIN_BLUE, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(PIN_BLUE, 1);
    
    setup_channel(PIN_RED   >> 1);
    setup_channel(PIN_GREEN >> 1);
    setup_channel(PIN_BLUE  >> 1);
}

static void watchdog_task(void *pdata)
{
    // Watchdog will timeout after 20sec
    //¢TMR_WatchDogEnable(TMR_CLK_FREQ * 50);
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(9000));
        printf("w\n");
        TMR_WatchDogRestart();
    }
}

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    setup_peripherals();
    
    // platform_config(PLATFORM_CFG_LOG_HCI, PLATFORM_CFG_ENABLE);
    
    // setup putc handle
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);
    
    xTaskCreate(watchdog_task,
               "w",
               configMINIMAL_STACK_SIZE,
               NULL,
               (configMAX_PRIORITIES - 1),
               NULL);
    return 0;
}

