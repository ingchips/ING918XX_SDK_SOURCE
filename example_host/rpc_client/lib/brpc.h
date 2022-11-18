#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct block
{
    struct block *next;
    int len;
    uint8_t buf[0];
};

void brpc_init(void);
void comm_append_block(int len, const void *data);
void brpc_rx_byte(void *user_data, uint8_t c);

#ifdef __cplusplus
}
#endif
