#ifndef _SERVICE_IBEACON_H
#define _SERVICE_IBEACON_H

#include <stdint.h>

#pragma pack (push, 1)
typedef struct ibeacon_adv
{
    uint16_t apple_id;
    uint16_t id;
    uint8_t  uuid[16];
    uint16_t major;
    uint16_t minor;
    int8_t   ref_power;
} ibeacon_adv_t;
#pragma pack (pop)

#define APPLE_COMPANY_ID        0x004C
#define IBEACON_ID              0x1502

void print_uuid(uint8_t *uuid);
double estimate_distance(int8_t ref_power, int8_t rssi);

#endif

