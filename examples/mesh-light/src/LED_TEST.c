#include "LED_TEST.h"
#include "peripheral_pinctrl.h"
#include "peripheral_gpio.h"


void LED_Test_Init(void)
{
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_GPIO0));
    PINCTRL_SetPadMux(PIN_LED_9, IO_SOURCE_GPIO);
    GIO_SetDirection(PIN_LED_9, GIO_DIR_OUTPUT);
    LED_OFF(PIN_LED_9);
}
