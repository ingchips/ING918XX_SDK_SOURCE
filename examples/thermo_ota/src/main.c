#define OPTIONAL_RF_CLK
#include "profilestask.h"

#include "ingsoc.h"

#include "bme280.h"
#include "iic.h"
#include <stdio.h>

#include "platform_api.h"

struct bme280_t bme280_data;

uint32_t cb_putc(char *c, void *dummy)
{
    uint8_t ch = (uint8_t)*c;
    UART_TypeDef* pBase;
    
#if(USE_UART1)
    pBase = APB_UART1;
#else
    pBase = APB_UART0;
#endif
    
    while (apUART_Check_TXFIFO_FULL(pBase) == 1);
    UART_SendData(pBase, ch);

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
    UART_0.two_stop_bits     = 0;   //used 2 stop bit ,    0
    UART_0.receive_en        = 1;
    UART_0.transmit_en       = 1;
    UART_0.UART_en           = 1;
    UART_0.cts_en            = 0;
    UART_0.rts_en            = 0;
    UART_0.rxfifo_waterlevel = 1;    //UART_FIFO_ONE_SECOND;
    UART_0.txfifo_waterlevel = 1;    //UART_FIFO_ONE_SECOND;
    UART_0.ClockFrequency    = freq;
    UART_0.BaudRate          = baud;

    apUART_Initialize(APB_UART0, &UART_0, 0);

#if(USE_UART1)
    UART_0.BaudRate          = 115200;
    apUART_Initialize_1(APB_UART1, &UART_0, 0);
#endif
}

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_I2C0));

#ifndef SIMULATION   
    PINCTRL_SetPadMux(6, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(7, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(14, IO_SOURCE_I2C0_SCL_O);
    PINCTRL_SetPadMux(15, IO_SOURCE_I2C0_SDO);
    PINCTRL_SelI2cSclIn(I2C_PORT_0, 14);
    i2c_init(I2C_PORT_0);

    printf("sensor init...");
    if (bme280_init(&bme280_data)==0)
        printf("failed\n");
    else
        printf("OK\n");
#endif
}

uint32_t setup_profile(void *, void *);

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    setup_peripherals();

    // platform_config(PLATFORM_CFG_LOG_HCI, PLATFORM_CFG_ENABLE);
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    return 0;
}

