#include <string.h>
#include <stdlib.h>
#include "audio_service.h"
#include "audio_encoder.h"
#include "ingsoc.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

#include "platform_api.h"
#include "log.h"

extern void audio_input_setup(void);
extern void audio_input_start(void);
extern void audio_input_stop(void);

audio_enc_t audio_t;
audio_encoder_t aud_enc_t;

static adpcm_priv_t adpcm_priv=
{
    .voice_buf_block_num = 4100 / 150,
    .voice_buf_block_size = 150,

    .sample_buf_num = 2,
    .sample_buf_size = 20,
};

static sbc_priv_t sbc_priv= {0};

uint8_t data_buffer[VOICE_BUF_BLOCK_NUM][VOICE_BUF_BLOCK_SIZE] = {0};
uint16_t block_index;
uint16_t byte_index;
uint16_t seq_cnt;
int8_t mic_dig_gain = 0;

#define SAMPLE_BUF_LEN  128
#define SAMPLE_BUF_CNT  2

pcm_sample_t sample_buf[SAMPLE_BUF_CNT][SAMPLE_BUF_LEN];

int sample_buf_index = 0;
int sample_index = 0;

extern void audio_trigger_send(void);

void enc_output_cb(uint8_t output, void *param)
{
    data_buffer[block_index][byte_index] = output;
    byte_index++;
    if (byte_index >= VOICE_BUF_BLOCK_SIZE)
    {
        block_index++;
        audio_trigger_send();
        if (block_index >= VOICE_BUF_BLOCK_NUM)
            block_index = 0;
        byte_index = 0;
    }
}

#define QUEUE_LENGTH    30
#define ITEM_SIZE       sizeof(int16_t) 
static StaticQueue_t xStaticSampleQueue;
static uint8_t ucQueueStorageArea[ITEM_SIZE * QUEUE_LENGTH];
QueueHandle_t xSampleQueue;

#if (OVER_SAMPLING == 2)
#define FIR_LEN         7
#define FILTER_GAIN     1
#elif (OVER_SAMPLING == 1)
#define FIR_LEN         1
#define FILTER_GAIN     1
#endif

typedef struct
{
    int8_t h[FIR_LEN];
    pcm_sample_t x[FIR_LEN - 1];
} fir_t;

#define TO_INT8(v)       ((int8_t)(v * 127 * FILTER_GAIN))

#if (OVER_SAMPLING == 2)
fir_t fir = { .h = {TO_INT8(-0.0133217), 
                    0,
                    TO_INT8(0.26318),
                    TO_INT8(0.5),
                    TO_INT8(0.26318),
                    0,
                    TO_INT8(-0.0133217)
                    } };
#elif (OVER_SAMPLING == 1)
fir_t fir = { .h = {TO_INT8(1.0)} };
#endif

void fir_push(fir_t *fir, pcm_sample_t x)
{
    int i;
    for (i = FIR_LEN - 1; i >= 2; i--)
    {
        fir->x[i - 1] = fir->x[i - 2];
    }
#if (FIR_LEN > 1)
    fir->x[0] = x;
#endif
}

pcm_sample_t fir_push_run(fir_t *fir, pcm_sample_t x)
{
    int i;
    int32_t r = fir->h[0] * x;
    
    for (i = FIR_LEN - 1; i >= 1; i--)
    {
        r += fir->h[i] * fir->x[i - 1];
        fir->x[i - 1] = fir->x[i - 2];
    }
#if (FIR_LEN > 1)
    fir->x[0] = x;
#endif
    return r >> 7;
}

void audio_start(void)
{
    LOG_PRINTF(LOG_LEVEL_DEBUG,"Start audio input."); 
    sample_buf_index = 0;
    sample_index = 0;
#if (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_ADPCM)
    adpcm_enc_init(&enc, enc_output_cb, 0);
#endif
    block_index = 0;
    byte_index = 0;
    audio_input_start();
}

void audio_stop(void)
{
    LOG_PRINTF(LOG_LEVEL_DEBUG,"Stop audio input.");
    xQueueReset(xSampleQueue);
    audio_input_stop();
}

//将两种编码器整合在一起
static void audio_task(void *pdata)
{
#if (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_SBC)
    int size, codesize, framelen, encodelen;
    uint8_t encoded;

    codesize = sbc_get_codesize(&sbc);
    framelen = sbc_get_frame_length(&sbc); 

    pcm_sample_t *inp;
    uint8_t *outp;
    outp = malloc(framelen * sizeof(uint8_t*));
#endif

#if (OVER_SAMPLING_MASK != 0)
    int oversample_cnt = 0;
#endif
    pcm_sample_t *buf;
    for (;;)
    {
        int16_t index;
        int i;
        
        if (xQueueReceive(xSampleQueue, &index, portMAX_DELAY ) != pdPASS)
            continue;

        buf = sample_buf[index];
        
        for (i = 0; i < audio_t.sample_buf_size; i++)
        {
            pcm_sample_t sample = buf[i];
#if (OVER_SAMPLING_MASK != 0)
            oversample_cnt = (oversample_cnt + 1) & OVER_SAMPLING_MASK;
            if (oversample_cnt != 0)
            {
                fir_push(&fir, sample);
                continue;
            }
            else
                sample = fir_push_run(&fir, sample);
#endif

#if (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_ADPCM)
            adpcm_encode(&enc, sample);
#endif
        }

#if (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_SBC)
        encodelen = aud_enc_t.encoder(&sbc, inp, codesize, outp, framelen);
        if(encodelen == codesize) 
        {
            for (int i=0; i<framelen; i++) {
                enc_output_cb(*(outp + i), 0); 
		        printf("%c ",*(outp+i));	
            } 
            printf("\n");
        }
#endif
    }
}


uint16_t audio_get_curr_block(void)
{
    return block_index;
}

uint8_t *audio_get_block_buff(uint16_t index)
{
    return data_buffer[index];
}

void audio_rx_sample(pcm_sample_t sample)
{
    BaseType_t xHigherPriorityTaskWoke = pdFALSE;

    // digital gain
    if (mic_dig_gain > 0)
        sample <<= mic_dig_gain;
    else if (mic_dig_gain < 0)  
        sample >>= -mic_dig_gain;

    sample_buf[sample_buf_index][sample_index] = sample;
    sample_index++;
    if (sample_index >= audio_t.sample_buf_size)
    {
        xQueueSendFromISR(xSampleQueue, &sample_buf_index, &xHigherPriorityTaskWoke);
        sample_buf_index++;
        if (sample_buf_index >= audio_t.sample_buf_num)
            sample_buf_index = 0;
        sample_index = 0;
    }
}

static void enc_state_init(audio_enc_t *audio, audio_encoder_t *enc_t);

void audio_init(void)
{
    LOG_PRINTF(LOG_LEVEL_INFO,"Initializing audio encoder...");
    //sbc struct init.
    enc_state_init(&audio_t, &aud_enc_t);

    xSampleQueue = xQueueCreateStatic(QUEUE_LENGTH,
                                 ITEM_SIZE,
                                 ucQueueStorageArea,
                                 &xStaticSampleQueue); 
    xTaskCreate(audio_task,
               "b",
               1024,
               NULL,
               (configMAX_PRIORITIES - 14),
               NULL);
    
    audio_input_setup();
    LOG_PRINTF(LOG_LEVEL_INFO,"Initialization completed.");
    // audio_start();
}

static void enc_state_init(audio_enc_t *audio, audio_encoder_t *enc_t)
{
#if (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_ADPCM)
    LOG_PRINTF_TAB(LOG_LEVEL_INFO,"Encoder-->[ADPCM]");
    LOG_PRINTF_TAB(LOG_LEVEL_INFO,"Configure encode's parameter...");
    audio->voice_buf_block_num = adpcm_priv.voice_buf_block_num;
    audio->voice_buf_block_size = adpcm_priv.voice_buf_block_size;
    audio->sample_buf_num = adpcm_priv.sample_buf_num;
    audio->sample_buf_size = adpcm_priv.sample_buf_size;
    enc_t->encoder = adpcm_encode;
    LOG_PRINTF_TAB(LOG_LEVEL_INFO,"Parameter configured successfully.");

#elif (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_SBC)
    LOG_PRINTF_TAB(LOG_LEVEL_INFO,"Encoder-->[SBC]");
    sbc_enc_init(&sbc, enc_output_cb, 0L);
    sbc_priv.voice_buf_block_size = sbc_get_frame_length(&sbc);
    sbc_priv.voice_buf_block_num = 4100 / sbc_priv.voice_buf_block_size;
    sbc_priv.sample_buf_size = sbc_get_codesize(&sbc);
    LOG_PRINTF_TAB(LOG_LEVEL_INFO,"Configure encode's parameter...");
    audio->voice_buf_block_num = sbc_priv.voice_buf_block_num;
    audio->voice_buf_block_size = sbc_priv.voice_buf_block_size;
    audio->sample_buf_num = sbc_priv.sample_buf_num;
    audio->sample_buf_size = sbc_priv.sample_buf_size;
    LOG_PRINTF_TAB(LOG_LEVEL_INFO,"Parameter configured successfully.");

    enc_t->encoder = sbc_encode;
#endif
}