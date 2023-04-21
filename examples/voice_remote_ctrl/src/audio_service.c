#include <string.h>
#include <stdlib.h>
#include "audio_service.h"
#include "audio_sbc.h" 
#include "audio_adpcm.h"
#include "ingsoc.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

#include "platform_api.h"
#include "log.h"

//音频算法选择
#define AUDIO_CODEC_ALGORITHM_ADPCM     0
#define AUDIO_CODEC_ALGORITHM_SBC       1
#define AUDIO_CODEC_ALGORITHM_LC3       2
#define AUDIO_CODEC_ALGORITHM   AUDIO_CODEC_ALGORITHM_ADPCM

extern void audio_input_setup(void);
extern void audio_input_start(void);
extern void audio_input_stop(void);

static adpcm_enc_t enc;
static sbc_t sbc;

audio_enc_t audio_t;

//adpcm缓存参数结构体定义
static adpcm_priv_t adpcm_priv=
{
    .voice_buf_block_num = 4100 / 150,
    .voice_buf_block_size = 150,

    .sample_buf_num = 2,
    .sample_buf_size = 20,
};

//sbc缓存参数结构体定义
static sbc_priv_t sbc_priv=
{
    .voice_buf_block_num = 20,
    .voice_buf_block_size = 60,

    .sample_buf_num = 2,
    .sample_buf_size = 32,    
};

uint8_t data_buffer[VOICE_BUF_BLOCK_NUM][VOICE_BUF_BLOCK_SIZE] = {0};
uint16_t block_index;
uint16_t byte_index;
uint16_t seq_cnt;
int8_t mic_dig_gain = 0;

#define SAMPLE_BUF_LEN  128

//输入二维数组
pcm_sample_t sample_buf[2][SAMPLE_BUF_LEN];
//pcm_sample_t **sample_buf = NULL;

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
    LOG_PRINTF(LOG_LEVEL_INFO,"函数调用:启动/重启音频输入."); 
    sample_buf_index = 0;
    sample_index = 0;
#if (AUDIO_CODEC_ALGORITHM == AUDIO_CODEC_ALGORITHM_ADPCM)
    adpcm_enc_init(&enc, enc_output_cb, 0);
#endif
    block_index = 0;
    byte_index = 0;
    audio_input_start();
}

void audio_stop(void)
{
    LOG_PRINTF(LOG_LEVEL_INFO,"函数调用: 停止音频输入.");
#if defined(KB_TEST) 
    xQueueReset(xSampleQueue);
#endif
    audio_input_stop();
}

static void audio_adpcm_task(void *pdata)
{
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
            adpcm_encode(&enc, sample);
        }
    }
}

static void audio_sbc_task(void *pdata)
{
    int size, codesize, framelen, encodelen;
    uint8_t encoded;

    //获取编码一个frame所需的原始数据量
    codesize = sbc_get_codesize(&sbc);
    //一帧编码后的数据长度
    framelen = sbc_get_frame_length(&sbc);

    LOG_PRINTF(LOG_LEVEL_INFO,"codesize = %d framelen = %d\r\n", codesize, framelen); 

    sbc_sample_t *inp, *outp;
    outp = malloc(framelen * sizeof(sbc_sample_t));
    //可能需要换一个API来使用，直接申请RTOS堆的空间

#if (OVER_SAMPLING_MASK != 0)
    int oversample_cnt = 0;
#endif

    for(;;)
    {
        int16_t index;
        int i;

        if (xQueueReceive(xSampleQueue, &index, portMAX_DELAY ) != pdPASS)
            continue;
        if (xQueueIsQueueFullFromISR(xSampleQueue) != pdFALSE)
            LOG_PRINTF(LOG_LEVEL_ERROR,"xSampleQueue is Full.");
        
        inp = (sbc_sample_t *)(sample_buf[index]);    //获取单行数首地址        
        
        for (i = 0; i < audio_t.sample_buf_size; i++)
        {
            sbc_sample_t sample = inp[i];
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
        }
        encodelen = sbc_encode(&sbc, inp, codesize, outp, framelen);
        if(encodelen == codesize) 
        {
            for (int i=0; i<framelen; i++) {
                enc_output_cb((uint8_t)(*(outp + i)), 0); 
            } 
        }
 
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
#if (AUDIO_CODEC_ALGORITHM == AUDIO_CODEC_ALGORITHM_ADPCM)
    // digital gain
    if (mic_dig_gain > 0)
        sample <<= mic_dig_gain;
    else if (mic_dig_gain < 0)  
        sample >>= -mic_dig_gain;
#elif (AUDIO_CODEC_ALGORITHM == AUDIO_CODEC_ALGORITHM_SBC) 
    int codesize = sbc_get_codesize(&sbc);
#endif

    sample_buf[sample_buf_index][sample_index] = sample;
    sample_index++;
    if (sample_index >= audio_t.sample_buf_size)
    {
        //该函数用于在中断服务程序中向队列尾部发送一个消息
        xQueueSendFromISR(xSampleQueue, &sample_buf_index, &xHigherPriorityTaskWoke);
        sample_buf_index++;
        if (sample_buf_index >= audio_t.sample_buf_num)
            sample_buf_index = 0;
        sample_index = 0;
    }
}

/**
 * @brief 输入参数[row,column],创建可变长的二维数组
 */
int create_input_data_buff(int row, int column, pcm_sample_t **buff)
{
    LOG_PRINTF(LOG_LEVEL_INFO,"%s函数调用: 创建输入[%d,%d]二维数组",__func__, row, column);
    buff = (pcm_sample_t **)malloc(sizeof(pcm_sample_t *)*row);

    if (NULL == buff) {
        LOG_PRINTF(LOG_LEVEL_ERROR,"memory error in row!");
        return -1;
    }
    
    for (int i=0; i < row; i++) {   
		buff[i] = (pcm_sample_t *)malloc(sizeof(pcm_sample_t)*column);
        
        if (NULL == buff[i]) {
            LOG_PRINTF(LOG_LEVEL_ERROR,"memory error in column!");
            return -1;
        }

        for (int j=0; j<column; j++) {
            buff[i][j] = 0;
        }
    }
    
    platform_printf("\r\n");
    for (int i=0; i<row; i++) {
        for (int j=0; j<column; j++) {
            platform_printf("[%d] ",buff[i][j]);
        }
        platform_printf("\r\n");
    }

	return 0;
}

#if defined(KB_TEST)
#define CTRL_KEY    GIO_GPIO_6

uint32_t gpio_isr(void *user_data)
{
    uint32_t current = ~GIO_ReadAll();

    // report which keys are pressed
    if (current & (1 << CTRL_KEY))
    {
        static int last = 0;

        if (last)
        {
            audio_input_stop();
            last = 0;
        }
        else
        {
            audio_input_start();
            last = 1;
        }
    }
    
    GIO_ClearAllIntStatus();
    return 0;
}
#endif

static void enc_state_init(audio_enc_t *audio);
static void audio_task_register();

void audio_init(void)
{
    //结构体初始化
    enc_state_init(&audio_t);
    LOG_PRINTF_TAB(LOG_LEVEL_INFO,"编码器状态初始化.");
    
    //创建音频输ru数组
    //create_input_data_buff(audio_t.sample_buf_num, audio_t.sample_buf_size, sample_buf);
    //platform_printf("...OK\r\n\n");

    //创建音频输入数组
    //platform_printf("...OK\r\n\n");

    //注册task函数
    audio_task_register();
    LOG_PRINTF_TAB(LOG_LEVEL_INFO,"音频任务函数注册.");

    xSampleQueue = xQueueCreateStatic(QUEUE_LENGTH,
                                 ITEM_SIZE,
                                 ucQueueStorageArea,
                                 &xStaticSampleQueue); 
    xTaskCreate(audio_enc_task,
               "b",
               1024,
               NULL,
               (configMAX_PRIORITIES - 14),
               NULL);
    
    audio_input_setup();
    LOG_PRINTF_TAB(LOG_LEVEL_INFO,"初始化配置.");
#if defined(KB_TEST)
    PINCTRL_SetPadMux(CTRL_KEY, IO_SOURCE_GENERAL);
    GIO_SetDirection(CTRL_KEY, GIO_DIR_INPUT);
    GIO_ConfigIntSource(CTRL_KEY, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE,
                        GIO_INT_EDGE);
    platform_set_irq_callback(PLATFORM_CB_IRQ_GPIO, gpio_isr, NULL);
#endif
}

static void enc_state_init(audio_enc_t *audio)
{
    audio->audio_dev_start = audio_start;
    audio->audio_dev_stop = audio_stop;

#if (AUDIO_CODEC_ALGORITHM == AUDIO_CODEC_ALGORITHM_ADPCM)
    LOG_PRINTF(LOG_LEVEL_INFO,"编码器选择-->ADPCM");
    LOG_PRINTF(LOG_LEVEL_INFO,"编码器参数表如下：");
    audio->voice_buf_block_num = adpcm_priv.voice_buf_block_num;
    audio->voice_buf_block_size = adpcm_priv.voice_buf_block_size;
    audio->sample_buf_num = adpcm_priv.sample_buf_num;
    audio->sample_buf_size = adpcm_priv.sample_buf_size;
    LOG_PRINTF(LOG_LEVEL_INFO,"block_num=[%d] block_size=[%d] buf_num=[%d] buf_size=[%d]",audio->voice_buf_block_num, 
                                    audio->voice_buf_block_size, audio->sample_buf_num, audio->sample_buf_size);
#elif (AUDIO_CODEC_ALGORITHM == AUDIO_CODEC_ALGORITHM_SBC)
    LOG_PRINTF(LOG_LEVEL_INFO,"Encoder select 'SBC'");
    sbc_init(&sbc, 0L);
    sbc_priv.voice_buf_block_size = sbc_get_frame_length(&sbc);
    sbc_priv.voice_buf_block_num = 4100 / sbc_priv.voice_buf_block_size;
    sbc_priv.sample_buf_size = sbc_get_codesize(&sbc);
    LOG_PRINTF(LOG_LEVEL_INFO,"编码器参数表如下：");
    audio->voice_buf_block_num = sbc_priv.voice_buf_block_num;
    audio->voice_buf_block_size = sbc_priv.voice_buf_block_size;
    audio->sample_buf_num = sbc_priv.sample_buf_num;
    audio->sample_buf_size = sbc_priv.sample_buf_size;
    LOG_PRINTF(LOG_LEVEL_INFO,"block_num=[%d]  block_size=[%d]\r\nbuf_num=[%d]  buf_size=[%d]",audio->voice_buf_block_num, 
                                    audio->voice_buf_block_size, audio->sample_buf_num, audio->sample_buf_size);
#elif (AUDIO_CODEC_ALGORITHM == AUDIO_CODEC_ALGORITHM_LC3)
    #warning Please look forward to.
#endif
}

static void audio_task_register()
{
#if (AUDIO_CODEC_ALGORITHM == AUDIO_CODEC_ALGORITHM_ADPCM)
    audio_enc_task = audio_adpcm_task;
    LOG_PRINTF(LOG_LEVEL_INFO,"%s函数调用: 注册adpcm编码器task函数",__func__); 
#elif (AUDIO_CODEC_ALGORITHM == AUDIO_CODEC_ALGORITHM_SBC)
    audio_enc_task = audio_sbc_task;
    LOG_PRINTF(LOG_LEVEL_INFO,"%s函数调用: 注册adpcm编码器task函数",__func__); 
#endif  
}