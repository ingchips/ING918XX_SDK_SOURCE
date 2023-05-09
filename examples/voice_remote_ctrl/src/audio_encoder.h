#ifndef _AUDIO_ENCODER_H_
#define _AUDIO_ENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_CODEC_ALG_ADPCM     0
#define AUDIO_CODEC_ALG_SBC       1
#define AUDIO_CODEC_ALG_LC3       2 
#define AUDIO_CODEC_ALG   AUDIO_CODEC_ALG_SBC

#if (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_ADPCM)
#include "audio_adpcm.h"
static adpcm_enc_t enc;
#elif (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_SBC)
#include "audio_sbc.h"
static sbc_t sbc;
#elif (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_LC3)
#error Please look forward to.
#endif

typedef struct
{
    int voice_buf_block_num;
    int voice_buf_block_size;

    uint8_t **data_buffer;
    int  **sample;
    
    int sample_buf_num;
    int sample_buf_size;
}audio_enc_t;

extern audio_enc_t audio_t;

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
    //±‡¬Î∆˜¿‡–Õ
    enum{
        ADPCM_ENCODER,
        SBC_ENCODER,
        LC3_ENCODER,
    }type;
    
    void (*encoder)(void *enc, void *input, int input_size, void *output, int output_size);
    void (*decoder)(void *dec, void *input, int input_size, void *output, int output_size);

}audio_encoder_t;

#ifdef __cplusplus
}
#endif

#endif  //_AUDIO_ENCODER_H_