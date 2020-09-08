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

#define INPUT_REPORT_KEYS_MAX   6

#pragma pack (push, 1)
typedef struct kb_report
{
    uint8_t modifier;
    uint8_t reserved;
    uint8_t codes[INPUT_REPORT_KEYS_MAX];
} kb_report_t;
#pragma pack (pop)

#endif


