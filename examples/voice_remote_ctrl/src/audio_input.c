#include "audio_encoder.h"

extern void audio_rx_sample(pcm_sample_t sample);

#if (AUDIO_INPUT == 0)
#include "audio_input_sim.c"
#elif (AUDIO_INPUT == 1)
#include "audio_input_adc.c"
#elif (AUDIO_INPUT == 2)
#include "audio_input_i2s.c"
#elif (AUDIO_INPUT == 3)
#include "audio_input_pdm.c"
#elif (AUDIO_INPUT == 4)
#include "audio_input_asdm.c"
#else
#error unknown AUDIO_INPUT
#endif
