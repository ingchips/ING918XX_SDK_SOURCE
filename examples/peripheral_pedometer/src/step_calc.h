#ifndef __TRIAXIAL_CALCULATION_H__
#define __TRIAXIAL_CALCULATION_H__

#include <stdint.h>

// accelarator sampling rate (Hz)
#define ACC_SAMPLING_RATE    (50)  

typedef struct
{
    uint32_t    total_sample_cnt;
    uint32_t    temp_sample_cnt;
    uint32_t    total_steps;
    uint16_t    temp_steps;
    
    uint16_t    speed;            // Unit is in m/s with a resolution of 1/256 s
    uint8_t     cadence;          // Unit is in 1/minute (or RPM) with a resolutions of 1 1/min (or 1 RPM) 
    uint16_t    stride_length;    // Unit is in meter with a resolution of 1/100 m (or centimeter).
    uint32_t    total_distance;   // Unit is in meter with a resolution of 1/10 m (or decimeter).
} pedometer_info_t;

void accelarator_sample(void);
void pedometer_init(void);

const pedometer_info_t *pedometer_get_info(void);

#endif

