#ifndef _CMDS_H
#define _CMDS_H
#include <stdint.h>

typedef enum
{
    DIR_UNDEFINED,
    DIR_S_TO_M,
    DIR_M_TO_S
} tpt_dir_t;

void console_rx_data(const char *d, const uint8_t len);
uint8_t *console_get_clear_tx_data(uint16_t *len);

#endif
