#ifndef _AUDIO_H
#define _AUDIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// References for ADPCM:
// 1. Text Books on Communication Systems
// 2. http://www.cs.columbia.edu/~hgs/audio/dvi/IMA_ADPCM.pdf

typedef int16_t pcm_sample_t;

typedef void (*adpcm_encode_output_cb_f)(uint8_t output, void *param);
typedef void (*adpcm_decode_output_cb_f)(pcm_sample_t output, void* param);

typedef struct adpcm_state_s
{
    pcm_sample_t predicated;
    int8_t index;
} adpcm_state_t;

typedef struct adpcm_enc_s
{
    adpcm_encode_output_cb_f callback;
    void   *param;
    adpcm_state_t state;
    uint8_t  output;
    uint8_t  output_index;
} adpcm_enc_t;

typedef struct adpcm_dec_s
{
    adpcm_decode_output_cb_f callback;
    void* param;
    adpcm_state_t state;
} adpcm_dec_t;

void adpcm_enc_init(adpcm_enc_t* adpcm, adpcm_encode_output_cb_f callback, void *param);
void adpcm_dec_init(adpcm_dec_t* adpcm, adpcm_decode_output_cb_f callback, void* param);

void adpcm_encode(adpcm_enc_t *adpcm, pcm_sample_t sample);
void adpcm_decode(adpcm_dec_t *adpcm, uint8_t data);

void adpcm_set_dec_state(adpcm_dec_t *adpcm, const adpcm_state_t *state);

#ifdef __cplusplus
}
#endif

#endif
