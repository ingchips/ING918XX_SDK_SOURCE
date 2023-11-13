#ifndef _AUDIO_SBC_H
#define _AUDIO_SBC_H

#include <stdint.h>

#include "../libsbc/include/sbc.h"

typedef int16_t pcm_sample_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*sbc_encode_output_cb_f)(uint8_t output, void *param);

typedef struct
{
    struct sbc_frame frame;
    sbc_t sbc;
    sbc_encode_output_cb_f callback;
    void *param;
    uint8_t output[SBC_MAX_SAMPLES];
} sbc_enc_ctx_t;


void sbc_enc_init(sbc_enc_ctx_t *sbc, sbc_encode_output_cb_f callback, void *param);

int sbc_get_n_of_samples(sbc_enc_ctx_t *sbc);

void sbc_enc(sbc_enc_ctx_t *sbc, const pcm_sample_t *input, int input_size);

#ifdef __cplusplus
}
#endif

#endif
