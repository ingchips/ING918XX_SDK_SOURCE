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

void sys_aligned_read_mem(uint32_t addr, int len, void *p);
uint32_t sys_aligned_read(uint32_t addr);
void sys_aligned_write_mem(uint32_t addr, void *p, int len);
void sys_aligned_write(uint32_t addr, uint32_t value);

#ifdef __cplusplus
}
#endif
