#ifndef _PROFILESTASK_H_
#define _PROFILESTASK_H_

#include <stdint.h>

uint32_t setup_profile(void *data, void *user_data);

void app_timer_callback(void);

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


