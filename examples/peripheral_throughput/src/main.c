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

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    #define KB_KEY_1        GIO_GPIO_1
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    #define KB_KEY_1        GIO_GPIO_6
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
    #define KB_KEY_1        GIO_GPIO_6
#else
    #error unknown or unsupported chip family
#endif

extern uint8_t loopback_mode;

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_GPIO0)
                              | (1 << SYSCTRL_ClkGate_APB_PinCtrl));
    setup_keys();
    loopback_mode = GIO_ReadValue(KB_KEY_1) ? 0 : 1;
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

    //platform_config(PLATFORM_CFG_LOG_HCI, PLATFORM_CFG_ENABLE);

    // setup deep sleep handlers
    platform_set_evt_callback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, on_deep_sleep_wakeup, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, query_deep_sleep_allowed, NULL);
    // setup putc handle
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    setup_peripherals();

    platform_printf("Press Key1 & Reset to enter LOOPBACK mode.\n");
    platform_printf("Current mode: %s\n", loopback_mode ? "LOOPBACK" : "Throughput");

    return 0;
}



