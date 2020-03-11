#define OPTIONAL_RF_CLK

#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include <stdio.h>

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

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
 //   *(uint32_t*)(0x4007005c) = 0x81;
 //   *(uint32_t*)(0x40090064) = 0x400;
}

#define w32(a,b) *(volatile uint32_t*)(a) = (uint32_t)(b)
#define r32(a)   (*(volatile uint32_t*)(a))

uint32_t on_deep_sleep_wakeup(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);   

    w32(0x40004000,0x0);//ie0
    w32(0x40004004,0x0);//ie1
    w32(0x40004008,0x0);//is0
    w32(0x4000400c,0x0);//is1
    w32(0x40004010,0x0);//pe0
    w32(0x40004014,0x0);//pe1
    w32(0x40004018,0x0);//ps0
    w32(0x4000401c,0x0);//ps1
    w32(0x40004030,0x0);//ds0
    w32(0x40004034,0x0);//ds1

    w32(0x40070058,0xffffffff);//peri input select
    w32(0x40070048,0x0);//gpio output enable
    w32(0x4007004c,0x0);//gpio output enable
    w32(0x40070050,0x0);//gpio output enable

    w32(0x40070054,0xffffffc3);//peri input select
    w32(0x40070044,0x400);//gpio output enable

    w32(0x40070050,0x0);//gpio output enable

    w32(0x40040094,r32(0x40040094) | 0x1);        //gpio output enable
    w32(0x40040090,r32(0x40040090) | (0x1 << 16));// adc sleep

    setup_peripherals();

    return 0;
}

uint32_t query_deep_sleep_allowed(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);    
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

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    platform_set_rf_clk_source(0);

    setup_peripherals();
    sysSetPublicDeviceAddr((const unsigned char *)(0x2a010));
    
    platform_set_rf_power_mapping(power_mapping);
    
    //platform_config(PLATFORM_CFG_POWER_SAVING, 0);
    //platform_config(PLATFORM_CFG_LOG_HCI, 1);

    // setup deep sleep handlers
    platform_set_evt_callback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, on_deep_sleep_wakeup, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, query_deep_sleep_allowed, NULL);

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);
    
    // setup putc handle
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);    
    
    return 0;
}
