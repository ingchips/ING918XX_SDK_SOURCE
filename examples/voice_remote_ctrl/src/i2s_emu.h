#ifndef _i2s_emu_h
#define _i2s_emu_h

// ----------------------------------------------------------------------------
// I2S emulating.
//
// This driver uses following peripherals to emulate I2S interface:
// SPI 0, PWM channel 0 & 1, Timer 1.
// 
// Note: This driver supports I2S microphone, 16kHz sampling rate
//       (64 SCK cycles per sample)
// ----------------------------------------------------------------------------

#include "ingsoc.h"

#ifndef WS_PIN

    #define WS_PIN              GIO_GPIO_8
    #define BCLK_PIN            GIO_GPIO_9
    #define SPI_MIC_MISO        GIO_GPIO_12

    #define MIC_PORT            AHB_SSP0
    #define INTERNAL_TIMER      APB_TMR1
#endif

#include "audio.h"

typedef void (*f_pcm_sample_cb)(pcm_sample_t sample);

void i2s_emu_setup(f_pcm_sample_cb f, int sample_rate);

void i2s_emu_start(void);
void i2s_emu_stop(void);

#endif
