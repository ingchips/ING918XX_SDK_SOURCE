#ifndef _AUDIO_ENCODER_H_
#define _AUDIO_ENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define AUDIO_CODEC_ALG_ADPCM     0
#define AUDIO_CODEC_ALG_SBC       1
#define AUDIO_CODEC_ALG_LC3       2 

#ifndef AUDIO_CODEC_ALG
#define AUDIO_CODEC_ALG   AUDIO_CODEC_ALG_ADPCM
#endif

#if (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_ADPCM)
#include "audio_adpcm.h"
static adpcm_enc_t adpcm;
#elif (AUDIO_CODEC_ALG == AUDIO_CODEC_ALG_SBC)
#include "audio_sbc.h"
static sbc_t sbc;
#else
#error unknown AUDIO_CODEC_ALG
#endif

typedef struct
{
    int num;
    int size;
}sample_buf_t;

typedef void (* fun_encoder)(void *enc, void *input, int input_size, void *output, int output_size);

typedef struct
{
    enum
    {
        ADPCM_ENCODER,
        SBC_ENCODER,
        LC3_ENCODER,
    }type;

    sample_buf_t sample_buf;

    fun_encoder encoder;
}audio_encoder_t;

#ifdef __cplusplus
}
#endif

#endif  //_AUDIO_ENCODER_H_
