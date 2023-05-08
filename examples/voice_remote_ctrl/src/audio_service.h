#ifndef _AUDIO_SERVICE_H
#define _AUDIO_SERVICE_H

#include <stdint.h>

#include "app_cfg.h"

#define AUDIO_CODEC_ALG_ADPCM     0
#define AUDIO_CODEC_ALG_SBC       1
#define AUDIO_CODEC_ALG   AUDIO_CODEC_ALG_ADPCM

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

static void (*audio_enc_task)(void *pdata);

typedef int16_t sample_t;

typedef struct
{
    void (*audio_dev_start)(void);
    void (*audio_dev_stop)(void);

    int voice_buf_block_num;
    int voice_buf_block_size;

    int (*create_input_buf)(int buf_row, int buf_column, void **buff);
    int (*create_output_buf)(int buf_row, int buf_column, void **buff);

    uint8_t **data_buffer;
    int  **sample;
    
    int sample_buf_num;
    int sample_buf_size;
}audio_enc_t;

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

typedef struct
{
    //编码器类型
    enum{
        ADPCM_ENCODER,
        SBC_ENCODER,
        LC3_ENCODER,
    }type;

    //编码器函数
    void (*encoder)(void *enc, void *in_data, int in_len, void *out_data, int out_len);


}audio_encoder_t;

#endif
