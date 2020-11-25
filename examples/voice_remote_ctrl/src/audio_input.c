#include "audio.h"

extern void audio_rx_sample(pcm_sample_t sample);

#if (AUDIO_INPUT == 0)
#include "audio_input_sim.c"
#elif (AUDIO_INPUT == 2)
#include "audio_input_i2s.c"
#else
#include "audio_input_adc.c"
#endif
