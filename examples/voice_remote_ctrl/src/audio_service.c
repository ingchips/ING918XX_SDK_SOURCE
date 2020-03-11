#include "audio_service.h"
#include "audio.h"
#include "ingsoc.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

static adpcm_enc_t enc;

uint8_t data_buffer[VOICE_BUF_BLOCK_NUM][VOICE_BUF_BLOCK_SIZE] = {0};
uint16_t block_index;
uint16_t byte_index;
int8_t mic_dig_gain = 0;

extern void audio_trigger_send(void);

void write_header()
{
    data_buffer[block_index][0] = enc.state.predicated & 0xff;
    data_buffer[block_index][1] = enc.state.predicated >> 8;
    data_buffer[block_index][2] = enc.state.index;
    data_buffer[block_index][3] = VOICE_BUF_BLOCK_SIZE - 4;
    byte_index = 4;
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
#ifdef DEV_BOARD
        byte_index = 0;
#else
        write_header();
#endif
    }
}

#ifdef SIMULATION
const pcm_sample_t pcm[] = 
#ifdef DEV_BOARD
#include "../data/tts_sample.m"
#else
#include "../data/itu_female_16k.m"
#endif
;
uint16_t pcm_index = 0;
#endif

#define QUEUE_LENGTH    30
#define ITEM_SIZE       sizeof(int16_t) 
static StaticQueue_t xStaticSampleQueue;
static uint8_t ucQueueStorageArea[ITEM_SIZE * QUEUE_LENGTH];
QueueHandle_t xSampleQueue;

#define FIR_LEN    7

typedef struct
{
    int8_t h[FIR_LEN];
    pcm_sample_t x[FIR_LEN - 1];
} fir_t;

pcm_sample_t fir_push(fir_t *fir, pcm_sample_t x)
{
    int i;
    pcm_sample_t r = fir->h[0] * x;
    
    for (i = FIR_LEN - 1; i >= 1; i--)
    {
        r += fir->h[i] * fir->x[i - 1];
        fir->x[i - 1] = fir->x[i - 2];
    }
    fir->x[0] = x;
    return r >> 7;
}

uint32_t audio_sample_isr(void *user_data)
{
    pcm_sample_t sample;
    BaseType_t xHigherPriorityTaskWoke = pdFALSE;

    TMR_IntClr(APB_TMR1);

#ifdef SIMULATION
    sample = pcm[pcm_index++];
    if (pcm_index >= sizeof(pcm) / sizeof(pcm[0])) pcm_index = 0;    
#else
    sample = ADC_ReadChannelData(0) - 512;
#endif
    
    // digital gain
    if (mic_dig_gain > 0)
        sample <<= mic_dig_gain;
    else if (mic_dig_gain < 0)
        sample >>= -mic_dig_gain;
    
    xQueueSendFromISR(xSampleQueue, &sample, &xHigherPriorityTaskWoke);
    
    return 0;
}

void audio_start(void)
{
    adpcm_enc_init(&enc, enc_output_cb, 0);
    block_index = 0;
#ifdef DEV_BOARD
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
    for (;;)
    {
        int16_t sample;
        if (xQueueReceive(xSampleQueue, &sample, portMAX_DELAY ) != pdPASS)
            continue;
        adpcm_encode(&enc, sample);
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
