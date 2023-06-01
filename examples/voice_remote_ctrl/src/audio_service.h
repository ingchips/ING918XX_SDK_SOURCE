#ifndef _AUDIO_SERVICE_H
#define _AUDIO_SERVICE_H

#include <stdint.h>

#include "app_cfg.h"

#if (APP_TYPE == APP_ING)
// 4KB per sec
#define VOICE_BUF_BLOCK_SIZE  150   // this is indicated in GATT voice information
#define VOICE_BUF_BLOCK_NUM   (4100 / VOICE_BUF_BLOCK_SIZE)     // total buffer: 4.1KB
#elif (APP_TYPE == APP_ANDROID)
#define VOICE_BUF_BLOCK_SIZE  (128 + 6)
#define VOICE_BUF_BLOCK_NUM   (4000 / VOICE_BUF_BLOCK_SIZE)
#elif (APP_TYPE == APP_MIBOXS)
#define VOICE_BUF_BLOCK_SIZE  60
#define VOICE_BUF_BLOCK_NUM   (4000 / VOICE_BUF_BLOCK_SIZE)
#elif (APP_TYPE == APP_CUSTOMER)
// 4KB per sec
#define VOICE_BUF_BLOCK_SIZE  128   // this is indicated in GATT voice information
#define VOICE_BUF_BLOCK_NUM   (4000 / VOICE_BUF_BLOCK_SIZE)     // total buffer: 4KB
#endif

void audio_init(void);
uint32_t audio_sample_isr(void *user_data);

typedef int16_t sample_t;

void audio_start(void);
void audio_stop(void);

uint16_t audio_get_curr_block(void);
uint8_t *audio_get_block_buff(uint16_t index);

#endif
