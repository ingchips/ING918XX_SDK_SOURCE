#if (AUDIO_CODEC_ALG == 1)

#include "audio_sbc.h"

#include "app_cfg.h"

#include "../libsbc/src/bits.c"
#include "../libsbc/src/sbc.c"

int sbc_get_n_of_samples(sbc_enc_ctx_t *sbc)
{
    return sbc->frame.nblocks * sbc->frame.nsubbands;
}

void sbc_enc(sbc_enc_ctx_t *sbc, const pcm_sample_t *input, int input_size)
{
    int i;
    int len;

    if (input_size != sbc_get_n_of_samples(sbc))
        while (1);

    sbc_encode(&sbc->sbc,
        input + 0, 1,
        input + 1, 2,   // ignored
        &sbc->frame,
        sbc->output, sizeof(sbc->output));

    len = sbc_get_frame_size(&sbc->frame);
    for (i = 0; i < len; i++)
        sbc->callback(sbc->output[i], sbc->param);
}

void sbc_enc_init(sbc_enc_ctx_t *sbc, sbc_encode_output_cb_f callback, void *param)
{
    sbc->callback = callback;
    sbc->param = param;
}

#endif
