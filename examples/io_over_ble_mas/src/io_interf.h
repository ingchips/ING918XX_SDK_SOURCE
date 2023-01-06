#ifndef _IO_DEF_H
#define _IO_DEF_H

#include <stdint.h>

#define IO_TYPE_UART_STR    0   // string-oriented transmission
#define IO_TYPE_UART_BIN    1   // binary-oriented transmission
#define IO_TYPE_LOOPBACK    2   // loopback (only one can be set to loopback)
#define IO_TYPE_USB_BIN     3   // binary-oriented transmission

typedef struct
{
    uint8_t ble_addr[6]; 
} node_info_t;

typedef struct
{
    node_info_t master;
    node_info_t slave;
} pair_config_t;

typedef enum
{
    STATE_SCANNING,
    STATE_ADV,
    STATE_CONNECTED,
    STATE_CONNECTING,
    STATE_DISCOVERING,
    STATE_RX,
    STATE_TX
} io_state_t;

void io_interf_setup_peripherals(void);
void io_interf_init(void);

#ifdef IS_MASTER
#define HANDLE_FUNC         handle_output
#else
#define HANDLE_FUNC         handle_input
#endif

void HANDLE_FUNC(const uint8_t *data, int len);

#endif

