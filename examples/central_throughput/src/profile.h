#ifndef _PROFILESTASK_H_
#define _PROFILESTASK_H_

#include <stdint.h>

uint32_t setup_profile(void *data, void *user_data);

#ifdef USE_DISPLAY
typedef struct app_status
{
    uint8_t connected;
    uint8_t peer[6];
    int s2m_bps;
    int m2s_bps;
    int rssi;
} app_status_t;

extern app_status_t app_status;
extern void app_state_updated(void);
#endif

#endif


