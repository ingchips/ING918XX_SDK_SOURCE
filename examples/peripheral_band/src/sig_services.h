#ifndef SIG_SERICES_H
#define SIG_SERICES_H

#include <stdint.h>

#define NOTIFICATION_ENABLED    1
#define INDICATION_ENABLED      2

#pragma pack (push, 1)
typedef struct
{
    uint8_t     flags;
    uint16_t    speed;            // Unit is in m/s with a resolution of 1/256 s
    uint8_t     cadence;          // Unit is in 1/minute (or RPM) with a resolutions of 1 1/min (or 1 RPM) 
    uint16_t    stride_length;    // Unit is in meter with a resolution of 1/100 m (or centimeter).
    uint32_t    total_distance;   // Unit is in meter with a resolution of 1/10 m (or decimeter).
} rsc_meas_t;

typedef struct
{
    uint8_t     flags;
    uint8_t     heart_rate;      // flags & 1 == 0 -> Heart Rate Value Format is set to UINT8. Units: beats per minute (bpm) (C1)
} heart_rate_meas_t;

#pragma pack (pop)

extern rsc_meas_t rsc_meas;
extern uint8_t rsc_enable;

extern heart_rate_meas_t heart_rate_meas;

void heart_meas_init(void);
void heart_meas_sample(void);
void heart_meas_ctrl(uint8_t enable);
uint8_t heart_meas_get_enable(void);


#define SAMPLING_CNT_32HZ   (32)

#endif
