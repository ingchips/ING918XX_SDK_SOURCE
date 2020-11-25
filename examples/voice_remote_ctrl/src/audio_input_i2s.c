#include "i2s_emu.h"

#include "app_cfg.h"

#if (SAMPLING_RATE != 16000)
#error "only 16kHz is supported"
#endif

void audio_input_setup(void)
{
    i2s_emu_setup(audio_rx_sample, SAMPLING_RATE);
}

void audio_input_start(void)
{
    i2s_emu_start();
}

void audio_input_stop(void)
{
    i2s_emu_stop();
}
