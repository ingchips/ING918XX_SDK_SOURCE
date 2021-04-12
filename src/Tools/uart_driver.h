#ifndef uart_driver_h
#define uart_driver_h

#include <stdint.h>
#include "ingsoc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (* f_uart_rx_byte)(void *user_data, uint8_t c);

void uart_driver_init(UART_TypeDef *port, void *user_data, f_uart_rx_byte rx_byte_cb);

void driver_flush_tx(void);

uint32_t driver_append_tx_data(const void *data, int len);

uint32_t uart_driver_isr(void *user_data);

#ifdef __cplusplus
}
#endif

#endif
