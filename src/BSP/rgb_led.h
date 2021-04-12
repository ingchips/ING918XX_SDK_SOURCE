#ifndef _rgb_led_h
#define _rgb_led_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b);
void setup_rgb_led(void);

#ifdef __cplusplus
}
#endif

#endif
