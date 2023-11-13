#ifndef uart_driver_h
#define uart_driver_h

#include <stdint.h>
#include "ingsoc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (* f_uart_rx_byte)(void *user_data, uint8_t c);

/**
 * @brief Init this simple UART driver
 *
 * @param[in]   port            UART port
 * @param[in]   user_data       user data passed to `rx_byte_cb`
 * @param[in]   rx_byte_cb      callback for receiving bytes from UART
*/
void uart_driver_init(UART_TypeDef *port, void *user_data, f_uart_rx_byte rx_byte_cb);

/**
 * @brief Flush UART Tx data
*/
void driver_flush_tx(void);

/**
 * @brief Append data to the Tx data queue
 *
 * @param[in]   data        data
 * @param[in]   len         length of data in bytes
 * @return                  0 if data is written to the queue otherwise non-0
*/
uint32_t driver_append_tx_data(const void *data, int len);

/**
 * @brief ISR for the UART
 *
 * @param[in]   user_data   (not used)
*/
uint32_t uart_driver_isr(void *user_data);

/**
 * @brief Get current empty size of Tx data queue
 *
 * @return                  free size in bytes
*/
int driver_get_free_size(void);

#ifdef __cplusplus
}
#endif

#endif
