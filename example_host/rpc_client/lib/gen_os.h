#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void gen_os_init(void);

void sleep_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif
