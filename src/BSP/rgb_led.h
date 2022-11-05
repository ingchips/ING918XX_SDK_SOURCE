#ifndef _rgb_led_h
#define _rgb_led_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static void tlc59731_write(uint32_t value);
static void ws2881_write(uint32_t value);

#ifdef __cplusplus
}
#endif

#endif
