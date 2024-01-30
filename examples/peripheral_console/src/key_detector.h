#ifndef key_detector_h
#define key_detector_h

#include "peripheral_gpio.h"

#ifndef KEY_PIN

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
#define KEY_PIN     GIO_GPIO_9
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#define KEY_PIN     GIO_GPIO_4
#endif
#endif

#ifndef VALUE_PRESSED
#define VALUE_PRESSED     1
#endif

typedef enum
{
    KEY_LONG_PRESSED = 0,
    KEY_PRESSED_SINGLE,
    KEY_PRESSED_DOUBLE,
    KEY_PRESSED_TRIPPLE
} key_press_event_t;

typedef void (*f_key_event_cb)(key_press_event_t evt); 

void key_detect_init(f_key_event_cb cb);
void key_detector_start_on_demand(void);

#endif
