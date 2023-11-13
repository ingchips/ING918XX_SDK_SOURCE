#ifndef _LED_TEST_H_
#define _LED_TEST_H_
#include <stdint.h>
#include "peripheral_gpio.h"

#define PIN_LED_9          GIO_GPIO_18

#define LED_ON(x)          GIO_WriteValue(x, 0)
#define LED_OFF(x)         GIO_WriteValue(x, 1)
#define LED_TOGGLE(x)      GIO_ToggleBits(1<<x)

void LED_Test_Init(void);

#endif

