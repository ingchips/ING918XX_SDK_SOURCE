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

#if (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_ADPCM)
#include "audio_adpcm.h"
static adpcm_enc_t adpcm;
#elif (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_SBC)
#include "audio_sbc.h"

static const struct sbc_frame frame_param =
{
#if (EFFECTIVE_SAMPLING_RATE == 16000)
    .freq = SBC_FREQ_16K,
#elif
    #error unsupported EFFECTIVE_SAMPLING_RATE
#endif
    .mode = SBC_MODE_MONO,
    .nsubbands = 4,
    .nblocks = 8,
    .bam = SBC_BAM_LOUDNESS,
    .bitpool = 16
};

static sbc_enc_ctx_t sbc;
#else
#error unknown AUDIO_CODEC_ALG
#endif

extern void audio_input_setup(void);
extern void audio_input_start(void);
extern void audio_input_stop(void);

audio_encoder_t aud_enc_t;
void *enc = NULL;

uint8_t data_buffer[VOICE_BUF_BLOCK_NUM][VOICE_BUF_BLOCK_SIZE] = {0};
uint16_t block_index;
uint16_t byte_index;
uint16_t seq_cnt;
int8_t mic_dig_gain = 0;

#define SAMPLE_BUF_LEN  32
#define SAMPLE_BUF_CNT  15

pcm_sample_t sample_buf[SAMPLE_BUF_CNT][SAMPLE_BUF_LEN];

#if (OVER_SAMPLING_MASK != 0)
pcm_sample_t down_sampling_buf[SAMPLE_BUF_LEN];
#endif

int sample_buf_index = 0;
int sample_index = 0;

extern void audio_trigger_send(void);

void enc_output_cb(uint8_t output, void *param)
{
    data_buffer[block_index][byte_index] = output;
    byte_index++;

    if (byte_index >= VOICE_BUF_BLOCK_SIZE)
    {
        byte_index = 0;
        block_index++;
        if (block_index >= VOICE_BUF_BLOCK_NUM)
            block_index = 0;

        audio_trigger_send();
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

static void enc_state_reset(void);

void audio_start(void)
{
    LOG_PRINTF(LOG_LEVEL_DEBUG,"Start audio input.");
    xQueueReset(xSampleQueue);
    sample_buf_index = 0;
    sample_index = 0;
    block_index = 0;
    byte_index = 0;
    enc_state_reset();
    audio_input_start();
}

void audio_stop(void)
{
    LOG_PRINTF(LOG_LEVEL_DEBUG,"Stop audio input.");
    audio_input_stop();
    xQueueReset(xSampleQueue);
}

static void audio_task(void *pdata)
{
    int input_size;
    input_size = aud_enc_t.sample_buf.size;

#if (OVER_SAMPLING_MASK != 0)
    int oversample_cnt = 0;
    int down_sample_index = 0;
    pcm_sample_t *down_sampling = down_sampling_buf;
#endif
    pcm_sample_t *buf;
    for (;;)
    {
        int16_t index;

        if (xQueueReceive(xSampleQueue, &index, portMAX_DELAY ) != pdPASS)
            continue;

        buf = sample_buf[index];

#if (OVER_SAMPLING_MASK != 0)
        int i;
        for (i = 0; i < aud_enc_t.sample_buf.size; i++)
        {
            pcm_sample_t sample = buf[i];
            oversample_cnt = (oversample_cnt + 1) & OVER_SAMPLING_MASK;
            if (oversample_cnt != 0)
            {
                fir_push(&fir, sample);
                continue;
            }
            else
            {
                sample = fir_push_run(&fir, sample);
                down_sampling[down_sample_index++] = sample;
            }

            if (down_sample_index >= input_size)
            {
                aud_enc_t.encoder(enc, down_sampling, input_size);
                down_sample_index = 0;
            }
        }
#else
        aud_enc_t.encoder(enc, buf, input_size);
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
    if (sample_index >= aud_enc_t.sample_buf.size)
    {
        xQueueSendFromISR(xSampleQueue, &sample_buf_index, &xHigherPriorityTaskWoke);
        sample_buf_index++;
        if (sample_buf_index >= aud_enc_t.sample_buf.num)
            sample_buf_index = 0;
        sample_index = 0;
    }
}

#define STACK_DEPTH     (2000/4)

static StackType_t uxStackBuffer[STACK_DEPTH];
static StaticTask_t xTaskBuffer;

static void enc_state_init(audio_encoder_t *enc_t);

void audio_init(void)
{
    LOG_PRINTF(LOG_LEVEL_INFO,"Initializing audio encoder...");
    //sbc struct init.
    enc_state_init(&aud_enc_t);

    xSampleQueue = xQueueCreateStatic(QUEUE_LENGTH,
                                 ITEM_SIZE,
                                 ucQueueStorageArea,
                                 &xStaticSampleQueue);

    xTaskCreateStatic(audio_task,
               "b",
               STACK_DEPTH,
               NULL,
               2,
               uxStackBuffer,
               &xTaskBuffer);

    audio_input_setup();
    LOG_PRINTF(LOG_LEVEL_INFO,"Initialization completed.");
}

static void enc_state_reset(void)
{
#if (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_ADPCM)

#elif (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_SBC)
    memset(&sbc, 0, sizeof(sbc));
    sbc.frame = frame_param;
    sbc_enc_init(&sbc, enc_output_cb, 0);
#endif
}

static void enc_state_init(audio_encoder_t *enc_t)
{
#if (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_ADPCM)
    LOG_PRINTF_TAB(LOG_LEVEL_INFO,"Encoder-->[ADPCM]");
    LOG_PRINTF_TAB(LOG_LEVEL_INFO,"Configure encode's parameter...");
    enc = &adpcm;
    adpcm_enc_init(enc, enc_output_cb, 0);
    enc_t->type = ADPCM_ENCODER;
    enc_t->sample_buf.num = SAMPLE_BUF_CNT;
    enc_t->sample_buf.size = SAMPLE_BUF_LEN;
    enc_t->encoder = (fun_encoder)adpcm_encode;
    LOG_PRINTF_TAB(LOG_LEVEL_INFO,"Parameter configured successfully.");

#elif (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_SBC)
    enc_t->type = SBC_ENCODER;
    LOG_PRINTF_TAB(LOG_LEVEL_INFO,"Encoder-->[SBC]");
    enc = &sbc;
    enc_state_reset();
    LOG_PRINTF_TAB(LOG_LEVEL_INFO,"Configure encode's parameter...");
    enc_t->sample_buf.num = SAMPLE_BUF_CNT;
    enc_t->sample_buf.size = sbc_get_n_of_samples(&sbc);
    LOG_PRINTF_TAB(LOG_LEVEL_INFO,"Parameter configured successfully.");
    enc_t->encoder = (fun_encoder)sbc_enc;
#endif
    if (SAMPLE_BUF_LEN > enc_t->sample_buf.size)
    {
        LOG_PRINTF_TAB(LOG_LEVEL_INFO, "SAMPLE_BUF_LEN (%d) is larger than required (%d)",
            SAMPLE_BUF_LEN, enc_t->sample_buf.size);
    }
    else if (SAMPLE_BUF_LEN < enc_t->sample_buf.size)
    {
        LOG_PRINTF_TAB(LOG_LEVEL_ERROR, "SAMPLE_BUF_LEN (%d) is smaller than required (%d)",
            SAMPLE_BUF_LEN, enc_t->sample_buf.size);
    }
}
