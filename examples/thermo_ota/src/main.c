#define OPTIONAL_RF_CLK
#include "profilestask.h"
#include "ingsoc.h"
#include <stdio.h>
#include <string.h>

#include "platform_api.h"
#include "ing_uecc.h"
#include "peripheral_gpio.h"
#include "iic.h"
#include "board.h"

uint32_t cb_putc(char *c, void *dummy)
{
    uint8_t ch = (uint8_t)*c;
    UART_TypeDef * pBase = APB_UART0;
    
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
}

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
}

uint32_t setup_profile(void *, void *);

int ecc_rng(uint8_t *dest, unsigned size)
{
    platform_hrng(dest, size);
    return 1;
}

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);
    setup_peripherals();

#ifdef SECURE_FOTA
    uECC_set_rng(ecc_rng);
#endif

    // platform_config(PLATFORM_CFG_LOG_HCI, PLATFORM_CFG_ENABLE);
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    return 0;
}

