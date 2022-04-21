#ifndef _BLINK_H
#define _BLINK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Note: lights should be driven by high level (`1`) of `a` output from PWM, or 
//       low level (`0`) of `b` output from PWM.


// blink styles
typedef enum
{
    BLINK_ON,       // always on
    BLINK_OFF,      // always off
    BLINK_SINGLE,   // on for a little while
    BLINK_FAST,     // blink fast
    BLINK_NORMAL,   // blink normal
    BLINK_SLOW,     // blink slow
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)    
    BLINK_MORSE_A,  // using multi duty cycle feature, we can blink (some) morse codes (note: timing does not follow standards.)
    BLINK_MORSE_D,
    BLINK_MORSE_E,
    BLINK_MORSE_I,
    BLINK_MORSE_K,
    BLINK_MORSE_M,
    BLINK_MORSE_N,
    BLINK_MORSE_O,
    BLINK_MORSE_R,
    BLINK_MORSE_S,
    BLINK_MORSE_T,
    BLINK_MORSE_U,
    BLINK_MORSE_W,
#endif
} blink_style_t;

void blink_style(const uint8_t pwm_channel_index, const blink_style_t style);

void blink_lightness(const uint8_t pwm_channel_index, const uint8_t lightness);

#ifdef __cplusplus
}
#endif

#endif
