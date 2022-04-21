#include "blink.h"
#include "peripheral_pwm.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#define LIGHT_PERA_THRESHOLD            (PWM_CLOCK_FREQ / 1000)
#define SINGLE_PERA_THRESHOLD           (PWM_CLOCK_FREQ / 5)
#define MORSE_PERA_THRESHOLD            (PWM_CLOCK_FREQ / 2)

static void blink_simple(const uint8_t pwm_channel_index, const uint32_t pera)
{
    PWM_SetPeraThreshold(pwm_channel_index, pera);
    PWM_SetMultiDutyCycleCtrl(pwm_channel_index, 0);
    PWM_SetHighThreshold(pwm_channel_index, 0, pera >> 1);
    PWM_SetMode(pwm_channel_index, PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE);
    PWM_SetMask(pwm_channel_index, 0, 0);
    PWM_Enable(pwm_channel_index, 1); 
    PWM_HaltCtrlEnable(pwm_channel_index, 0);
}

void blink_morse(const uint8_t pwm_channel_index, const char *pattern)
{
    int i = 0;;
    PWM_SetPeraThreshold(pwm_channel_index, MORSE_PERA_THRESHOLD);
    while (pattern[i])
    {
        switch (pattern[i])
        {
        case '-':
            PWM_SetHighThreshold(pwm_channel_index, i, MORSE_PERA_THRESHOLD >> 2);
            break;
        case '.':
            PWM_SetHighThreshold(pwm_channel_index, i, (MORSE_PERA_THRESHOLD * 3) >> 2);
            break;
        default:
            break;
        }
        i++;
    }
    PWM_SetHighThreshold(pwm_channel_index, i, MORSE_PERA_THRESHOLD + 1);
    PWM_SetMultiDutyCycleCtrl(pwm_channel_index, i);
    
    PWM_SetMode(pwm_channel_index, PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE);
    PWM_SetMask(pwm_channel_index, 0, 0);
    PWM_Enable(pwm_channel_index, 1); 
    PWM_HaltCtrlEnable(pwm_channel_index, 0);
}

void blink_style(const uint8_t pwm_channel_index, const blink_style_t style)
{
    PWM_HaltCtrlEnable(pwm_channel_index, 1);
    PWM_Enable(pwm_channel_index, 0);
    switch (style)
    {
    case BLINK_ON:
        PWM_HaltCtrlCfg(pwm_channel_index, 1, 0);
        break;
    case BLINK_OFF:
        PWM_HaltCtrlCfg(pwm_channel_index, 0, 1);
        break;
    case BLINK_SINGLE:
        PWM_SetPeraThreshold(pwm_channel_index, SINGLE_PERA_THRESHOLD);
        PWM_SetMultiDutyCycleCtrl(pwm_channel_index, 0);        // do not use multi duty cycles
        PWM_SetHighThreshold(pwm_channel_index, 0, SINGLE_PERA_THRESHOLD >> 1);
        PWM_SetMode(pwm_channel_index, PWM_WORK_MODE_SINGLE_WITHOUT_DIED_ZONE);
        PWM_SetMask(pwm_channel_index, 0, 0);
        PWM_Enable(pwm_channel_index, 1); 
        PWM_HaltCtrlEnable(pwm_channel_index, 0);
        break;
    case BLINK_FAST:
        blink_simple(pwm_channel_index, PWM_CLOCK_FREQ / 10);
        break;
    case BLINK_NORMAL:
        blink_simple(pwm_channel_index, PWM_CLOCK_FREQ / 5);
        break;
    case BLINK_SLOW:
        blink_simple(pwm_channel_index, PWM_CLOCK_FREQ / 1);
        break;
    case BLINK_MORSE_A:
        blink_morse(pwm_channel_index, ".-");
        break;
    case BLINK_MORSE_D:
        blink_morse(pwm_channel_index, "-..");
        break;
    case BLINK_MORSE_E:
        blink_morse(pwm_channel_index, ".");
        break;
    case BLINK_MORSE_I:
        blink_morse(pwm_channel_index, "..");
        break;
    case BLINK_MORSE_K:
        blink_morse(pwm_channel_index, "-.-");
        break;
    case BLINK_MORSE_M:
        blink_morse(pwm_channel_index, "--");
        break;
    case BLINK_MORSE_N:
        blink_morse(pwm_channel_index, "-.");
        break;
    case BLINK_MORSE_O:
        blink_morse(pwm_channel_index, "---");
        break;
    case BLINK_MORSE_R:
        blink_morse(pwm_channel_index, ".-.");
        break;
    case BLINK_MORSE_S:
        blink_morse(pwm_channel_index, "...");
        break;
    case BLINK_MORSE_T:
        blink_morse(pwm_channel_index, "-");
        break;
    case BLINK_MORSE_U:
        blink_morse(pwm_channel_index, "..-");
        break;
    case BLINK_MORSE_W:
        blink_morse(pwm_channel_index, ".--");
        break;
    }
}

void blink_lightness(const uint8_t pwm_channel_index, const uint8_t lightness)
{
    PWM_HaltCtrlEnable(pwm_channel_index, 1);
    PWM_Enable(pwm_channel_index, 0);
    PWM_SetPeraThreshold(pwm_channel_index, LIGHT_PERA_THRESHOLD);
    PWM_SetMultiDutyCycleCtrl(pwm_channel_index, 0);        // do not use multi duty cycles
    PWM_SetHighThreshold(pwm_channel_index, 0, (LIGHT_PERA_THRESHOLD << 8) / lightness);
    PWM_SetMode(pwm_channel_index, PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE);
    PWM_SetMask(pwm_channel_index, 0, 0);
    PWM_Enable(pwm_channel_index, 1); 
    PWM_HaltCtrlEnable(pwm_channel_index, 0);
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#define LIGHT_PERA_THRESHOLD            (PWM_CLOCK_FREQ / 1000)
#define SINGLE_PERA_THRESHOLD           (PWM_CLOCK_FREQ / 5)

static void blink_simple(const uint8_t pwm_channel_index, const uint32_t pera)
{
    PWM_SetPeraThreshold(pwm_channel_index, pera);
    PWM_SetHighThreshold(pwm_channel_index, 0, pera >> 1);
    PWM_SetMode(pwm_channel_index, PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE);
    PWM_SetMask(pwm_channel_index, 0, 0);
    PWM_Enable(pwm_channel_index, 1); 
    PWM_HaltCtrlEnable(pwm_channel_index, 0);
}

void blink_lightness(const uint8_t pwm_channel_index, const uint8_t lightness)
{
    PWM_HaltCtrlEnable(pwm_channel_index, 1);
    PWM_Enable(pwm_channel_index, 0);
    PWM_SetPeraThreshold(pwm_channel_index, LIGHT_PERA_THRESHOLD);
    PWM_SetHighThreshold(pwm_channel_index, 0, (LIGHT_PERA_THRESHOLD << 8) / lightness);
    PWM_SetMode(pwm_channel_index, PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE);
    PWM_SetMask(pwm_channel_index, 0, 0);
    PWM_Enable(pwm_channel_index, 1); 
    PWM_HaltCtrlEnable(pwm_channel_index, 0);
}

void blink_style(const uint8_t pwm_channel_index, const blink_style_t style)
{
    PWM_HaltCtrlEnable(pwm_channel_index, 1);
    PWM_Enable(pwm_channel_index, 0);
    switch (style)
    {
    case BLINK_ON:
        PWM_HaltCtrlCfg(pwm_channel_index, 1, 0);
        break;
    case BLINK_OFF:
        PWM_HaltCtrlCfg(pwm_channel_index, 0, 1);
        break;
    case BLINK_SINGLE:
        PWM_SetPeraThreshold(pwm_channel_index, SINGLE_PERA_THRESHOLD);
        PWM_SetHighThreshold(pwm_channel_index, 0, SINGLE_PERA_THRESHOLD >> 1);
        PWM_SetMode(pwm_channel_index, PWM_WORK_MODE_SINGLE_WITHOUT_DIED_ZONE);
        PWM_SetMask(pwm_channel_index, 0, 0);
        PWM_Enable(pwm_channel_index, 1); 
        PWM_HaltCtrlEnable(pwm_channel_index, 0);
        break;
    case BLINK_FAST:
        blink_simple(pwm_channel_index, PWM_CLOCK_FREQ / 10);
        break;
    case BLINK_NORMAL:
        blink_simple(pwm_channel_index, PWM_CLOCK_FREQ / 5);
        break;
    case BLINK_SLOW:
        blink_simple(pwm_channel_index, PWM_CLOCK_FREQ / 1);
        break;
    default:
        break;
    }
}

#else

#error unknown or unsupported chip family

#endif
