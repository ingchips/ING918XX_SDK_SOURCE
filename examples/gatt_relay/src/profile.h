#ifndef _PROFILESTASK_H_
#define _PROFILESTASK_H_

#include <stdint.h>

uint32_t setup_profile(void *data, void *user_data);

typedef enum
{
    APP_SCANNING,
    APP_CONNECTING,
    APP_CLONING,
    APP_ADVERTISING,
    APP_CONNECTED
} app_status_t;

#endif


