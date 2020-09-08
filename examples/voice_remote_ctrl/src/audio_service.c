#include "audio_service.h"
#include "audio.h"
#include "ingsoc.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

#include "platform_api.h"

static adpcm_enc_t enc;

uint8_t data_buffer[VOICE_BUF_BLOCK_NUM][VOICE_BUF_BLOCK_SIZE] = {0};
uint16_t block_index;
uint16_t byte_index;
uint16_t seq_cnt;
int8_t mic_dig_gain = 0;

#ifdef SIMULATION
uint8_t oversample_cnt = 0;
#endif

#define SAMPLE_BUF_LEN  20

int sample_buf_index = 0;
int sample_index = 0;
pcm_sample_t sample_buf[2][SAMPLE_BUF_LEN];

extern void audio_trigger_send(void);

void write_header()
{
#if (APP_TYPE == APP_ANDROID)
    data_buffer[block_index][0] = seq_cnt & 0xff;
    data_buffer[block_index][1] = seq_cnt >> 8;
    data_buffer[block_index][2] = 0;
    data_buffer[block_index][3] = enc.state.predicated & 0xff;
    data_buffer[block_index][4] = enc.state.predicated >> 8;
    data_buffer[block_index][5] = enc.state.index;
    byte_index = 6;
#else   
    data_buffer[block_index][0] = enc.state.predicated & 0xff;
    data_buffer[block_index][1] = enc.state.predicated >> 8;
    data_buffer[block_index][2] = enc.state.index;
    data_buffer[block_index][3] = VOICE_BUF_BLOCK_SIZE - 4;
    byte_index = 4;
#endif
}

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
#if (APP_TYPE == APP_ING)
        byte_index = 0;
#else
        write_header();
#endif
    }
}

#ifdef SIMULATION
const pcm_sample_t pcm[] = 
#include "../data/itu_female_16k.m"
;
uint16_t pcm_index = 0;
#endif

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

uint32_t audio_sample_isr(void *user_data)
{    
    pcm_sample_t sample;
    BaseType_t xHigherPriorityTaskWoke = pdFALSE;    

    TMR_IntClr(APB_TMR1);

#ifdef SIMULATION
    oversample_cnt = (oversample_cnt + 1) & OVER_SAMPLING_MASK;
    if (oversample_cnt != 0) return 0;
    if (pcm_index < sizeof(pcm) / sizeof(pcm[0]))
    {
        sample = pcm[pcm_index++];
    }
    else
        sample = 0;
#else
    sample = ADC_ReadChannelData(ADC_CHANNEL_ID) - 512;
#endif
    
    // digital gain
    if (mic_dig_gain > 0)
        sample <<= mic_dig_gain;
    else if (mic_dig_gain < 0)
        sample >>= -mic_dig_gain;
    
    sample_buf[sample_buf_index][sample_index] = sample;
    sample_index++;
    if (sample_index >= SAMPLE_BUF_LEN)
    {
        xQueueSendFromISR(xSampleQueue, &sample_buf_index, &xHigherPriorityTaskWoke);
        sample_buf_index++;
        if (sample_buf_index >= 2)
            sample_buf_index = 0;
        sample_index = 0;
    }

    return 0;
}

void audio_start(void)
{
    sample_buf_index = 0;
    sample_index = 0;
#ifdef SIMULATION
    pcm_index = 0;
    oversample_cnt = 0;
#endif
    adpcm_enc_init(&enc, enc_output_cb, 0);
    block_index = 0;    
#if (APP_TYPE == APP_ING)
    byte_index = 0;
#else
    write_header();
#endif
    TMR_Enable(APB_TMR1);
}

void audio_stop(void)
{
    TMR_Disable(APB_TMR1);
    xQueueReset(xSampleQueue);
}

static void audio_task(void *pdata)
{
    int oversample_cnt = 0;
    pcm_sample_t *buf;
    for (;;)
    {
        int16_t index;
        int i;
        
        if (xQueueReceive(xSampleQueue, &index, portMAX_DELAY ) != pdPASS)
            continue;

        buf = sample_buf[index];
        
        for (i = 0; i < SAMPLE_BUF_LEN; i++)
        {
            pcm_sample_t sample = buf[i];
#ifndef SIMULATION
            oversample_cnt = (oversample_cnt + 1) & OVER_SAMPLING_MASK;
            if (oversample_cnt != 0)
            {
                fir_push(&fir, sample);
                continue;
            }
            else
                sample = fir_push_run(&fir, sample);
#endif

            adpcm_encode(&enc, sample);
        }
    }
}

void audio_init(void)
{
    xSampleQueue = xQueueCreateStatic(QUEUE_LENGTH,
                                 ITEM_SIZE,
                                 ucQueueStorageArea,
                                 &xStaticSampleQueue); 
    xTaskCreate(audio_task,
               "b",
               150,
               NULL,
               (configMAX_PRIORITIES - 1),
               NULL);    
}

uint16_t audio_get_curr_block(void)
{
    return block_index;
}

uint8_t *audio_get_block_buff(uint16_t index)
{
    return data_buffer[index];
}
