#ifndef _PROFILESTASK_H_
#define _PROFILESTASK_H_

#include <stdint.h>
#include "ingsoc.h"

uint32_t setup_profile(void *data, void *user_data);

#define PRINT_PORT    APB_UART0
#define COMM_PORT     APB_UART1

#define PIN_UART_RX   GIO_GPIO_4

#define SPI_CH	        SPI_PORT_0
#define SPI_SSP         AHB_SSP0

#define SPI_PIN_CS      GIO_GPIO_16
#define SPI_PIN_MOSI    GIO_GPIO_17
#define SPI_PIN_SCK     GIO_GPIO_18
#define SPI_PIN_MISO    GIO_GPIO_19

#endif


