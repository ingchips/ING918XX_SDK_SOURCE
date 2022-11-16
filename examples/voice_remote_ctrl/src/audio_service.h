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

//task函数，批量实时调用编码函数
static void (*audio_enc_task)(void *pdata);

typedef int16_t sample_t;

struct audio_enc_state
{
    void (*audio_dev_start)(void);
    void (*audio_dev_stop)(void);

    int voice_buf_block_num;
    int voice_buf_block_size;
    int (*create_input_buff)(int buf_row, int buf_column, sample_t **buff);
    int (*create_output_buff)(int buf_row, int buf_column, uint8_t **buff);

    uint8_t **data_buffer;
    int  **sample;
    
    int sample_buf_num;
    int sample_buf_size;
};


//函数接口初始化
typedef struct audio_enc_state audio_enc_t;

extern audio_enc_t audio_t;

void audio_start(void);
void audio_stop(void);

uint16_t audio_get_curr_block(void);
uint8_t *audio_get_block_buff(uint16_t index);

struct adpcm_enc_priv
{
    int voice_buf_block_num;
    int voice_buf_block_size;

    int sample_buf_num;
    int sample_buf_size;
};

typedef struct adpcm_enc_priv adpcm_priv_t;

struct sbc_enc_priv
{
    int voice_buf_block_num;
    int voice_buf_block_size;

    int sample_buf_num;
    int sample_buf_size;
};
typedef struct sbc_enc_priv sbc_priv_t;

#endif
