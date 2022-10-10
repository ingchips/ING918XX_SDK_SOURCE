#include "rgb_led.h"
#include "ingsoc.h"

//PIN_RGB_LED must be GPIO0.
#ifndef PIN_RGB_LED
#define PIN_RGB_LED   GIO_GPIO_0
#endif

#define LED_TLC59731    0
#define LED_WS2881      1

//default old driver:tlc59731
#ifndef RGB_LED
#define RGB_LED     LED_TLC59731
#endif

// CPU clok: PLL_CLK_FREQ  48000000
// 1 cycle = 21ns
// 48 cycles per us
// Tcycle = 2us --> ~100 cycles
static void delay(int cycles)
{
    int i;
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    cycles *= SYSCTRL_GetHClk() / 48000000;
#endif
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
        { GIO_WriteValue(PIN_RGB_LED, 1);   \
        delay(1);                       \
        GIO_WriteValue(PIN_RGB_LED, 0); } while (0)

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

static void ws2881_write(uint32_t value)
{
    int8_t i;

    for( i = 0; i < 24; i++ )
    {
        uint32_t bit = value & ( 0x00800000 >> i);

        if (bit)
        {
            GIO_SetBits();
        }
        else
        {
            GIO_ClearBits();
        }
 
    }
    delay(100 * 8);
}

void set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t cmd = (0x3a << 24) | (b << 16) | (r << 8) | g;
#if(RGB_LED == LED_TLC59731)
    tlc59731_write(cmd);
#elif(RGB_LED == LED_WS2881)
    ws2881_write(cmd);
#endif
}

void setup_rgb_led()
{
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_GPIO0) | (1 << SYSCTRL_ClkGate_APB_GPIO1));
    PINCTRL_SetPadMux(PIN_RGB_LED, IO_SOURCE_GPIO);

    GIO_SetDirection(PIN_RGB_LED, GIO_DIR_OUTPUT);
    GIO_WriteValue(PIN_RGB_LED, 0);

    set_rgb_led_color(50, 50, 50);
}
