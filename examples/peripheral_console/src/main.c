#define OPTIONAL_RF_CLK

#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include <stdio.h>
#include "power_ctrl.h"
#include "key_detector.h"

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

uint32_t on_lle_reset(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);
    *(uint32_t *)(0x40090064) = 0x400 | (0x01 << 8);
    *(uint32_t *)(0x4007005c) = 0x80;

    return 0;
}

uint32_t gpio_isr(void *user_data)
{
    GIO_ClearAllIntStatus();
    key_detector_start_on_demand();
    return 0;
}

void setup_peripherals(void)
{
    int i;
    SYSCTRL_SetClkGateMulti((1 << SYSCTRL_ClkGate_APB_UART0));
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_GPIO)
                              | (1 << SYSCTRL_ClkGate_APB_PinCtrl));
    config_uart(OSC_CLK_FREQ, 115200);
    
    PINCTRL_SetPadMux(KEY_PIN, IO_SOURCE_GENERAL);
    GIO_SetDirection(KEY_PIN, GIO_DIR_INPUT);
    PINCTRL_Pull(KEY_PIN, PINCTRL_PULL_DOWN);
    GIO_ConfigIntSource(KEY_PIN, GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE, GIO_INT_EDGE);
    platform_set_irq_callback(PLATFORM_CB_IRQ_GPIO, gpio_isr, NULL);
    
    for (i = 8; i <= 15; i++)
    {
        if (i != KEY_PIN)
            PINCTRL_SetPadMux(i, IO_SOURCE_DEBUG_BUS);
    }
    on_lle_reset(NULL, NULL);
}

uint32_t on_deep_sleep_wakeup(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);
    power_ctrl_deep_sleep_wakeup();
    setup_peripherals();
    key_detector_start_on_demand();
    return 1;
}

uint32_t query_deep_sleep_allowed(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);
    power_ctrl_before_deep_sleep();
    return 1;
}

// To calibration Tx power preciously, we can use a "fake" power mapping table,
// then measure Tx power using testers.
const int16_t power_mapping[] = {
    0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200,
    1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300,
    2400, 2500, 2600, 2700, 2800, 2900, 3000, 3100, 3200, 3300, 3400,
    3500, 3600, 3700, 3800, 3900, 4000, 4100, 4200, 4300, 4400, 4500,
    4600, 4700, 4800, 4900, 5000, 5100, 5200, 5300, 5400, 5500, 5600,
    5700, 5800, 5900, 6000, 6100, 6200, 6300};

extern void on_key_event(key_press_event_t evt);

int app_main()
{
    power_ctrl_init();

#ifdef USE_OSC32K
    platform_config(PLATFORM_CFG_32K_CLK, PLATFORM_32K_OSC);
#else
    platform_config(PLATFORM_CFG_OSC32K_EN, PLATFORM_CFG_DISABLE);
    platform_config(PLATFORM_CFG_32K_CLK_ACC, 500);
#endif

    key_detect_init(on_key_event);
    setup_peripherals();

    platform_set_rf_power_mapping(power_mapping);

    // setup deep sleep handlers
    platform_set_evt_callback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, on_deep_sleep_wakeup, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, query_deep_sleep_allowed, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_LLE_INIT, on_lle_reset, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    // setup putc handle
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    platform_config(PLATFORM_CFG_POWER_SAVING, PLATFORM_CFG_ENABLE);

    return 0;
}
