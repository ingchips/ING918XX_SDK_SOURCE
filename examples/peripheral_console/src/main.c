#define OPTIONAL_RF_CLK

#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include <stdio.h>

//#define ING91881B

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

void setup_peripherals(void)
{
    int i;
    SYSCTRL_SetClkGateMulti((1 << SYSCTRL_ClkGate_APB_UART0));
    config_uart(OSC_CLK_FREQ, 115200);
    
    for (i = 8; i <= 15; i++)
        PINCTRL_SetPadMux(i, IO_SOURCE_DEBUG_BUS);
    on_lle_reset(NULL, NULL);
}

#define w32(a,b) *(volatile uint32_t*)(a) = (uint32_t)(b)
#define r32(a)   (*(volatile uint32_t*)(a))

uint32_t on_deep_sleep_wakeup(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);
#ifdef ING91881B
    w32(0x40040020,(r32(0x40040020)|((0x1<<15))));
#endif
    setup_peripherals();
    return 1;
}

uint32_t query_deep_sleep_allowed(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);
#ifdef ING91881B
    w32(0x40040020,(r32(0x40040020)&(~(0x1<<15))));
#endif
    return 1;
}

void patch_power_saving(void)
{
    w32(0x40040050,((r32(0x40040050)&((~(0x7ff<<16))))|(0x3<<16)));//dly_sys_ctrl_slp2act
    w32(0x40040064,((r32(0x40040064)&((~(0x3ff<<7))))|(0x8<<7)));//dly_en_ldo_slp2act
    w32(0x40040054,((r32(0x40040054)&((~(0xff<<20))))|(0x2<<20)));//dly_rf_slp2act
    w32(0x40040058,((r32(0x40040058)&((~(0xf<<13))))|(0x0<<13)));//dly_rf_sw2anaiso_slp2act
    w32(0x40040058,((r32(0x40040058)&((~(0xf<<9))))|(0x1<<9)));//dly_rf_sw2iso_slp2act
    w32(0x40040058,((r32(0x40040058)&((~(0xf<<5))))|(0x1<<5)));//dly_rf_sw2cg_slp2act
    w32(0x40040058,((r32(0x40040058)&((~(0x1f<<0))))|(0x2<<0)));//dly_rf_sw2rst_slp2act
    w32(0x40040084,((r32(0x40040084)&((~(0x7<<9))))|(0x0<<9)));//dly_pwrrdy_slp2act
    w32(0x40040050,((r32(0x40040050)&((~(0xf<<12))))|(0x1<<12)));//dly_sys_sw2iso_slp2act
    w32(0x40040050,((r32(0x40040050)&((~(0xf<<8))))|(0x3<<8)));//dly_sys_sw2cg_slp2act
    w32(0x40040050,((r32(0x40040050)&((~(0xf<<4))))|(0x3<<4)));//dly_sys_sw2rst_slp2act
    w32(0x40040054,((r32(0x40040054)&((~(0xf))))|(0x1)));//dly_lle_slp2act
    w32(0x40040054,((r32(0x40040054)&((~(0xf<<16))))|(0x1<<16)));//dly_lle_sw2iso_slp2act
    w32(0x40040054,((r32(0x40040054)&((~(0xf<<8))))|(0x3<<8)));//dly_lle_sw2rst_slp2act
    w32(0x40040054,((r32(0x40040054)&((~(0xf<<12))))|(0x3<<12)));//dly_lle_sw2cg_slp2act
    w32(0x4004004c,((r32(0x4004004c)&((~(0xf<<12))))|(0x2<<12)));//dly_rf_iso2digsw_act2slp
    w32(0x40040060,((r32(0x40040060)&((~(0xff<<7))))|(0x3<<7)));//dly_en_ldo_act2slp
    w32(0x40040048,((r32(0x40040048)&((~(0xff))))|(0x2)));//dly_lle_act2slp
    w32(0x40040048,((r32(0x40040048)&((~(0xf<<8))))|(0x1<<8)));//dly_lle_iso2sw_act2slp
    w32(0x40040048,((r32(0x40040048)&((~(0xf<<16))))|(0x1<<16)));//dly_lle_iso2rst_act2slp
    w32(0x40040044,((r32(0x40040044)&((~(0x7ff<<16))))|(0x1<<16)));//dly_sys_ctrl_act2slp
    w32(0x40040044,((r32(0x40040044)&((~(0xf<<12))))|(0x2<<12)));//dly_sys_iso2rst_act2slp
    w32(0x40040080,((r32(0x40040080)&((~(0xf<<16))))|(0x1<<16)));//dly_eflash_ctrl_act2slp
    w32(0x40040018,((r32(0x40040018)&((~(0xfful<<24))))|(0x3<<24)));//dly_vsel_strobe_rar_act2slp
    w32(0x40040018,((r32(0x40040018)&((~(0xff<<16))))|(0x2<<16)));//dly_unlock_rar_act2slp
    w32(0x40040018,((r32(0x40040018)&((~(0xf<<8))))|(0x1<<8)));//dly_vsel_in_rar_act2slp
    w32(0x40040060,((r32(0x40040060)&((~(0x1f<<2))))|(0x16<<2)));//vsel_ldo 1.8v

    w32(0x40040014,(r32(0x40040014) & (~(0x1f << 8))) | (0x8 << 8)); // slp voltage 0.9v
    w32(0x40040020,(r32(0x40040020) & (~(0x1f << 8))) | (0x8 << 8)); // act voltage 0.9

    platform_config(PLATFORM_CFG_PS_DBG_0, 0x0010ff00);
    platform_config(PLATFORM_CFG_DEEP_SLEEP_TIME_REDUCTION, 800);
    platform_config(PLATFORM_CFG_SLEEP_TIME_REDUCTION, 600);

    w32(0x40040020,(r32(0x40040020)&(~(0x1<<15))));
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

//#define USE_OSC32K

int app_main()
{
#ifdef ING91881B
    patch_power_saving();
#endif

#ifdef USE_OSC32K
    platform_config(PLATFORM_CFG_32K_CLK, PLATFORM_32K_OSC);
#else
    platform_config(PLATFORM_CFG_OSC32K_EN, PLATFORM_CFG_DISABLE);
    platform_config(PLATFORM_CFG_32K_CLK_ACC, 500);
#endif

    setup_peripherals();
    sysSetPublicDeviceAddr((const unsigned char *)(0x24010));

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
