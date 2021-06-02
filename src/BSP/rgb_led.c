#include "rgb_led.h"
#include "ingsoc.h"

#define PIN_SDI   GIO_GPIO_0

// CPU clok: PLL_CLK_FREQ  48000000
// 1 cycle = 21ns
// 48 cycles per us
// Tcycle = 2us --> ~100 cycles
static void delay(int cycles)
{
    int i;
    for (i = 0; i < cycles; i++)
    {
#ifdef __GNUC__
        __asm("NOP");
#else
        __nop();
#endif
    }
}

static void tlc59731_write(uint32_t value)
{
    int8_t i;

    #define pulse()                     \
        { GIO_WriteValue(PIN_SDI, 1);   \
        delay(1);                       \
        GIO_WriteValue(PIN_SDI, 0); } while (0)

    for( i = 0; i < 32; i++ )
    {
        uint32_t bit = value & ( 0x80000000 >> i );
        pulse();

        if (bit)
        {
            delay(10);
            pulse();
            delay(78);
        }
        else
            delay(90);
    }
    delay(100 * 8);
}

void set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t cmd = (0x3a << 24) | (g << 16) | (r << 8) | b;
    tlc59731_write(cmd);
}

void setup_rgb_led()
{
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_GPIO)
                              | (1 << SYSCTRL_ClkGate_APB_PWM));
    PINCTRL_SetPadMux(PIN_SDI, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(PIN_SDI, 0);
    GIO_SetDirection(PIN_SDI, GIO_DIR_OUTPUT);
    GIO_WriteValue(PIN_SDI, 0);

    set_rgb_led_color(50, 50, 50);
}
