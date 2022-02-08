#include <stdio.h>
#include <string.h>
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "trace.h"
#include "blink.h"
#include "rgb_led.h"

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

#define MAIN_LED        GIO_GPIO_6
#define STATUS_LED      GIO_GPIO_9

enum
{
    POWER_OFF = 0,
    POWER_ON
};

enum
{
    COLOR_RED = 0,
    COLOR_GREEN,
    COLOR_BLUE
};

typedef struct qcloud_light
{
    char power_state;
    uint32_t brightness;
    char color;
    uint32_t color_temp;
    char name[65];
} qcloud_light_t;

qcloud_light_t a_light = {.name = "A Light"};

void update_led(void)
{
    int v = 255 * a_light.brightness / 100;
    if (POWER_OFF == a_light.power_state)
    {
        set_rgb_led_color(0, 0, 0);
        return;
    }
    
    switch (a_light.color)
    {
    case COLOR_RED:
        set_rgb_led_color(v, 0, 0);
        break;
    case COLOR_GREEN:
        set_rgb_led_color(0, v, 0);
        break;
    default:
        set_rgb_led_color(0, 0, v);
        break;
    }
}

void property_power_switch(const char cmd);

int ble_property_power_switch_set(const char *data, uint16_t len)
{
    a_light.power_state = data[0];
    update_led();
    return 0;
}

int ble_property_power_switch_get(char *data, uint16_t buf_len)
{
    data[0] = a_light.power_state;
    return sizeof(uint8_t);
}

int ble_property_brightness_set(const char *data, uint16_t len)
{
    a_light.brightness = data[3];
    update_led();
    return 0;
}

int ble_property_brightness_get(char *data, uint16_t buf_len)
{
    memset(data, 0, 3);
    data[3] = a_light.brightness;
    return sizeof(uint32_t);
}

int ble_property_color_set(const char *data, uint16_t len)
{
    a_light.color = data[1];
    update_led();
    return 0;
}

int ble_property_color_get(char *data, uint16_t buf_len)
{
    data[0] = 0;
    data[1] = a_light.color;
    return sizeof(uint16_t);
}

int ble_property_color_temp_set(const char *data, uint16_t len)
{
    a_light.color_temp = data[3];
    update_led();
    return 0;
}

int ble_property_color_temp_get(char *data, uint16_t buf_len)
{
    memset(data, 0, 3);
    data[3] = a_light.color_temp;
    return sizeof(uint32_t);
}

int ble_property_name_set(const char *data, uint16_t len)
{
    memcpy(a_light.name, data, len);
    a_light.name[len] = '\0';
    return 0;
}

int ble_property_name_get(char *data, uint16_t buf_len)
{
    int len = strlen(a_light.name);
    memcpy(data, a_light.name, len);
    return len;
}

void setup_peripherals(void)
{
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_UART0)
                              | (1 << SYSCTRL_ClkGate_APB_PinCtrl)
                              | (1 << SYSCTRL_ClkGate_APB_WDT)
                              | (1 << SYSCTRL_ClkGate_APB_PWM));
    config_uart(OSC_CLK_FREQ, 115200);

    setup_rgb_led();
    update_led();

    // Watchdog will timeout after 20sec
    TMR_WatchDogEnable(TMR_CLK_FREQ * 10);
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

static void watchdog_task(void *pdata)
{
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(9000));
        TMR_WatchDogRestart();
    }
}

trace_rtt_t trace_ctx = {0};


int app_main()
{
    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    // setup handlers
    platform_set_evt_callback(PLATFORM_CB_EVT_HARD_FAULT, (f_platform_evt_cb)cb_hard_fault, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ASSERTION, (f_platform_evt_cb)cb_assertion, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, on_deep_sleep_wakeup, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, query_deep_sleep_allowed, NULL);    
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    setup_peripherals();
    xTaskCreate(watchdog_task,
           "w",
           configMINIMAL_STACK_SIZE,
           NULL,
           (configMAX_PRIORITIES - 1),
           NULL);

    trace_rtt_init(&trace_ctx);
    platform_set_evt_callback(PLATFORM_CB_EVT_TRACE, (f_platform_evt_cb)cb_trace_rtt, &trace_ctx);
    platform_config(PLATFORM_CFG_TRACE_MASK, 0);
    return 0;
}
