#ifndef _IO_DEF_H
#define _IO_DEF_H

#include <stdint.h>

#define IO_TYPE_UART        0
#define IO_TYPE_L2          1

typedef struct
{
    uint8_t ble_addr[6]; 
#if (IO_TYPE == IO_TYPE_L2)
    uint8_t mac_addr[6];
#endif
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

