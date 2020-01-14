##  Note: lights should be driven by high level (`1`) of `a` output from PWM, or
##        low level (`0`) of `b` output from PWM.
##  blink styles

type
  blink_style_t* {.size: sizeof(cint).} = enum
    BLINK_ON,                 ##  always on
    BLINK_OFF,                ##  always off
    BLINK_SINGLE,             ##  on for a little while
    BLINK_FAST,               ##  blink fast
    BLINK_NORMAL,             ##  blink normal
    BLINK_SLOW,               ##  blink slow
    BLINK_MORSE_A,            ##  using multi duty cycle feature, we can blink (some) morse codes (note: timing does not follow standards.)
    BLINK_MORSE_D, BLINK_MORSE_E, BLINK_MORSE_I, BLINK_MORSE_K, BLINK_MORSE_M,
    BLINK_MORSE_N, BLINK_MORSE_O, BLINK_MORSE_R, BLINK_MORSE_S, BLINK_MORSE_T,
    BLINK_MORSE_U, BLINK_MORSE_W


proc blink_style*(pwm_channel_index: uint8; style: blink_style_t) {.
    importc: "blink_style", header: "blink.h".}
proc blink_lightness*(pwm_channel_index: uint8; lightness: uint8) {.
    importc: "blink_lightness", header: "blink.h".}