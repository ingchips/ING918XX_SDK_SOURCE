#ifndef _PROFILESTASK_H_
#define _PROFILESTASK_H_

#include <stdint.h>

enum app_state
{
    APP_PAIRING,
    APP_ADV,
    APP_CONN,
    APP_IDLE
};

uint32_t setup_profile(void *data, void *user_data);

void app_timer_callback(void);

#endif


