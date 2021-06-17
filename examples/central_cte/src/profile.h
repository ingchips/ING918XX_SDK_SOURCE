#ifndef _PROFILESTASK_H_
#define _PROFILESTASK_H_

#include <stdint.h>

#define PAT_NUMBER  3

typedef struct
{
    int8_t len;
    int8_t def;
    uint8_t ant_ids[75];
} pattern_info_t;

typedef struct
{
    uint8_t slot_duration;
    uint8_t iq_select;
    uint8_t channel;
    pattern_info_t patterns[PAT_NUMBER];
} settings_t;

uint32_t setup_profile(void *data, void *user_data);

#endif


