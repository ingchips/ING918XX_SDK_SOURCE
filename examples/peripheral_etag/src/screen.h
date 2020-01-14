#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#if ((HEIGHT != 128) && (HEIGHT != 152))
#error "HEIGHT is wrong. valid values: 128/152"
#endif

#define IMAGE_PIXELS                (296 * HEIGHT)         // width * height

#define IMAGE_BYTE_SIZE             (IMAGE_PIXELS / 8)

typedef struct
{
    const uint8_t *black_white;
    const uint8_t *red_white;
} queue_msg_t;

#endif
