#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "stdbool.h"
#include "RF_TEST.H"
#include "app_config.h"
#include "ingsoc.h"

/**************************************************************************
 * 1. Define ENABLE_RF_TX_RX_TEST:
 *    RF_TX = GPIO_2
 *    RF_RX = GPIO_6   (LED3)
 *    UART_TX = GPIO_0 (TLED)
 *    UART_RX = GPIO_3
 * 2. Undefine ENABLE_RF_TX_RX_TEST:
 *    UART_TX = GPIO_2
 *    UART_RX = GPIO_3
 **/

#define RF_IO_TX_IND        GIO_GPIO_2 //RF tx
#define RF_IO_RX_IND        GIO_GPIO_6 //RF rx


static void rf_tx_rx_gpio_init(void){

    PINCTRL_SetPadMux(RF_IO_TX_IND, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(RF_IO_TX_IND, 0);
    GIO_SetDirection(RF_IO_TX_IND, GIO_DIR_OUTPUT);
    GIO_WriteValue(RF_IO_TX_IND, 0);

    PINCTRL_SetPadMux(RF_IO_RX_IND, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(RF_IO_RX_IND, 0);
    GIO_SetDirection(RF_IO_RX_IND, GIO_DIR_OUTPUT);
    GIO_WriteValue(RF_IO_RX_IND, 0);

}

#define w32(a,b) *(volatile uint32_t*)(a) = (uint32_t)(b);
#define r32(a)   *(volatile uint32_t*)(a)

static void set_gpio_2_6_for_rf_tx_rx(void)
{
    w32(0x4007005c, 0x82);
    w32(0x40070044, ((r32(0x40070044)) | (0xf<<8) | (0xf<<24)));
    w32(0x40090064, 0x400);
}

/* API START. */
void IngRfTest_btstack_ready(void){
    set_gpio_2_6_for_rf_tx_rx();
}

void IngRfTest_init(void){
    rf_tx_rx_gpio_init();
}
