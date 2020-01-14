#ifndef _CMDS_H
#define _CMDS_H
#include <stdint.h>

void console_rx_data(const char *d, const uint8_t len);
uint8_t *console_get_clear_tx_data(uint16_t *len);

void print_addr(const uint8_t *addr);

#define iprintf     printf //platform_printf
#define dprintf     printf //platform_printf

#endif
