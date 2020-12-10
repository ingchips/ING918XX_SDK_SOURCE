#ifndef _CMDS_H
#define _CMDS_H
#include <stdint.h>

typedef struct block_value
{
    int len;
    uint8_t value[100];
} block_value_t;

void console_rx_data(const char *d, const uint8_t len);
uint8_t *console_get_clear_tx_data(uint16_t *len);

#endif
