#include "audio.h"
#include <string.h>

const int8_t indexTable[16] = { -1, -1, -1, -1, 2, 4, 6, 8, /* Table of index changes */
                                -1, -1, -1, -1, 2, 4, 6, 8 };

const int16_t stepsizeTable[89] = { 7, 8, 9, 10, 11, 12, 13, 14, /* quantizer lookup table */
    16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658,
    724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
    3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635,
    13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767 };

void adpcm_enc_init(adpcm_enc_t* adpcm, adpcm_encode_output_cb_f callback, void *param)
{
    memset(adpcm, 0, sizeof(*adpcm));
    adpcm->callback = callback;
    adpcm->param = param;
}

void adpcm_dec_init(adpcm_dec_t* adpcm, adpcm_decode_output_cb_f callback, void* param)
{
    memset(adpcm, 0, sizeof(*adpcm));
    adpcm->callback = callback;
    adpcm->param = param;
}

static void adpcm_update(adpcm_state_t* state, const uint8_t sample)
{
    int32_t diff;
    int16_t step_size = stepsizeTable[state->index];
    int32_t predicated = state->predicated;

    /* compute new sample estimate predictedSample */
    diff = ((sample & 0x7) * step_size) >> 2; // calculate difference = (newSample + 1/2) * stepsize/4 
    diff += step_size >> 3;
    if (sample & 0x8)
        diff = -diff;

    /* adjust predicted sample based on calculated difference: */
    predicated += diff;
    if (predicated > 32767)
        predicated = 32767;
    else if (predicated < -32768)
        predicated = -32768;
    state->predicated = predicated;

    // update stepsize
    state->index += indexTable[sample];
    if (state->index < 0) /* check for index underflow */
        state->index = 0;
    else if (state->index > 88) /* check for index overflow */
        state->index = 88;
}

void adpcm_encode(adpcm_enc_t *adpcm, pcm_sample_t sample)
{
    int32_t diff = (int32_t)sample - adpcm->state.predicated;
    uint8_t new_sample = 0;
    uint8_t mask = 4;
    int16_t temp_step_size = stepsizeTable[adpcm->state.index];
    uint8_t i;

    if (diff < 0)
    {
        new_sample = 8;
        diff = -diff;
    }

    for (i = 0; i < 3; i++)
    {
        if (diff >= temp_step_size)
        {
            new_sample |= mask;
            diff -= temp_step_size;
        }
        temp_step_size >>= 1;
        mask >>= 1;
    }

    /* 4-bit newSample can be stored at this point */
    if (adpcm->output_index)
    {
        adpcm->callback((adpcm->output << 4) | new_sample, adpcm->param);
        adpcm->output_index = 0;
    }
    else
    {
        adpcm->output_index++;
        adpcm->output = new_sample;
    }

    adpcm_update(&adpcm->state, new_sample);
}

void adpcm_decode(adpcm_dec_t *adpcm, uint8_t data)
{
    adpcm_update(&adpcm->state, data >> 4);
    adpcm->callback(adpcm->state.predicated, adpcm->param);
    adpcm_update(&adpcm->state, data & 0xF);
    adpcm->callback(adpcm->state.predicated, adpcm->param);
}

void adpcm_set_dec_state(adpcm_dec_t *adpcm, const adpcm_state_t *state)
{
    adpcm->state = *state;
}
