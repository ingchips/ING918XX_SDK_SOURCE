#ifndef _PROFILESTASK_H_
#define _PROFILESTASK_H_

#include <stdint.h>

typedef enum
{
    APP_PAIRING,
    APP_WAIT_CONN,
    APP_CONN
} app_state_t;

extern void app_state_changed(const app_state_t state);

uint32_t setup_profile(void *data, void *user_data);

void start_talking(void);

#endif


