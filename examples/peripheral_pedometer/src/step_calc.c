// *****************************************************************************
//
//  Adopted from https://blog.csdn.net/Dancer__Sky/article/details/81504778
//  Reference: https://www.analog.com/en/analog-dialogue/articles/pedometer-design-3-axis-digital-acceler.html
//
// *****************************************************************************

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "eflash.h"
#include "bma2x2.h"
#include "step_calc.h"

#include "FreeRTOS.h"
#include "task.h"

#include "board.h"

#include <stdlib.h>


typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
}axis_info_t;

typedef struct {
	axis_info_t newmax;
	axis_info_t newmin;
	axis_info_t oldmax;
	axis_info_t oldmin;
}peak_value_t;

//Linear shift register for filtering high frequency noise
typedef struct slid_reg{
	axis_info_t new_sample;
	axis_info_t old_sample;
}slid_reg_t;

static pedometer_info_t result =
{
    .total_sample_cnt    = 0,
    .temp_sample_cnt    = 0,
    .total_steps   = 0,
    .temp_steps    = 0,

    .speed         = 0,
    .cadence       = 0,
    .stride_length = 70,     // 70cm
    .total_distance= 0,
};

#define MOST_ACTIVE_NULL                  0
#define MOST_ACTIVE_X                     1
#define MOST_ACTIVE_Y                     2
#define MOST_ACTIVE_Z                     3
#define ACTIVE_PRECISION    10

#define ABS(a) (0 - (a)) > 0 ? (-(a)) : (a)

#define DYNAMIC_PRECISION                 0

#define FILTER_CNT            3
#define PEAK_UPDATE_PERIOD    ACC_SAMPLING_RATE

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#ifndef SIMULATION

static uint8_t filter(axis_info_t *sample, struct bma2x2_accel_data *sample_xyz)
{
    uint8_t i;
    static uint8_t counter = 0;
    static struct bma2x2_accel_data buffer[FILTER_CNT] = {0};
    int16_t x_sum = 0, y_sum = 0, z_sum = 0;

    buffer[counter].x = sample_xyz->x;
    buffer[counter].y = sample_xyz->y;
    buffer[counter].z = sample_xyz->z;
    counter++;

    if (counter == FILTER_CNT)
    {
        counter = 0;

        for (i = 0; i < FILTER_CNT; i++)
        {
            x_sum += buffer[i].x;
            y_sum += buffer[i].y;
            z_sum += buffer[i].z;
        }

        sample->x = x_sum / FILTER_CNT;
        sample->y = y_sum / FILTER_CNT;
        sample->z = z_sum / FILTER_CNT;
    }

    return counter;
}

static void axis_value_init(axis_info_t *info, int16_t value)
{
    info->x = value;
    info->y = value;
    info->z = value;
}

static void peak_value_init(peak_value_t *peak)
{
    axis_value_init(&peak->newmin, 0x0);
    axis_value_init(&peak->newmax, 0x0);
}

static void update_peak(peak_value_t *peak, axis_info_t *cur_sample)
{
    static unsigned int sample_size = 0;

    peak->newmax.x = MAX(peak->newmax.x, cur_sample->x);
    peak->newmax.y = MAX(peak->newmax.y, cur_sample->y);
    peak->newmax.z = MAX(peak->newmax.z, cur_sample->z);

    peak->newmin.x = MIN(peak->newmin.x, cur_sample->x);
    peak->newmin.y = MIN(peak->newmin.y, cur_sample->y);
    peak->newmin.z = MIN(peak->newmin.z, cur_sample->z);

    sample_size++;
    if (sample_size >= PEAK_UPDATE_PERIOD)
    {
        sample_size = 0;
        peak->oldmax = peak->newmax;
        peak->oldmin = peak->newmin;
        peak_value_init(peak);
    }
}

static char update_slid(slid_reg_t *slid, axis_info_t *cur_sample)
{
    char res = 0;

    slid->old_sample = slid->new_sample;

    if (ABS((cur_sample->x - slid->new_sample.x)) > DYNAMIC_PRECISION)
    {
        slid->new_sample.x = cur_sample->x;
        res = 1;
    }

    if (ABS((cur_sample->y - slid->new_sample.y)) > DYNAMIC_PRECISION)
    {
        slid->new_sample.y = cur_sample->y;
        res = 1;
    }

    if (ABS((cur_sample->z - slid->new_sample.z)) > DYNAMIC_PRECISION)
    {
        slid->new_sample.z = cur_sample->z;
        res = 1;
    }

    return res;
}

static char is_most_active(peak_value_t *peak)
{
    char res = MOST_ACTIVE_NULL;
    short x_change = ABS((peak->newmax.x - peak->newmin.x));
    short y_change = ABS((peak->newmax.y - peak->newmin.y));
    short z_change = ABS((peak->newmax.z - peak->newmin.z));

    if (x_change > y_change && x_change > z_change && x_change >= ACTIVE_PRECISION)
        res = MOST_ACTIVE_X;
    else if (y_change > x_change && y_change > z_change && y_change >= ACTIVE_PRECISION)
        res = MOST_ACTIVE_Y;
    else if (z_change > x_change && z_change > y_change && z_change >= ACTIVE_PRECISION)
        res = MOST_ACTIVE_Z;
    return res;
}

static void detect_step(peak_value_t *peak, slid_reg_t *slid)
{
    int16_t threshold;
    int16_t old_sample;
    int16_t new_sample;

#define check_axis(a)                                                           \
    do {                                                                        \
        threshold = (peak->oldmax.a + peak->oldmin.a) / 2;                      \
        old_sample = slid->old_sample.a; new_sample = slid->new_sample.a;       \
    } while (0)

    if (peak->oldmax.x < peak->oldmin.x)
        return;

    switch (is_most_active(peak))
    {
    case MOST_ACTIVE_X:
        check_axis(x);
        break;

    case MOST_ACTIVE_Y:
        check_axis(y);
        break;

    case MOST_ACTIVE_Z:
        check_axis(z);
        break;
    default:
        return;
    }

    if (old_sample > threshold && new_sample < threshold)
    {
        result.total_steps++;
        result.temp_steps++;
    }
}

static struct bma2x2_accel_data sample_xyz = {0};
static axis_info_t cur_sample = {0};

static peak_value_t     peak = {0};
static slid_reg_t       slid_sample = {0};

#endif


void pedometer_init(void)
{
#ifndef SIMULATION    
    peak_value_init(&peak);
    peak.oldmin = peak.newmin;
    peak.oldmax = peak.newmax;

    setup_accelerometer();
#endif
}

//Round data of type float to round
#define INT16_T(x) ((x) > 0 ? (int16_t)(x + 0.5) : (int16_t)(x - 0.5))

void accelarator_sample(void)
{
    result.total_sample_cnt++;
    result.temp_sample_cnt++;

#ifndef SIMULATION
    float x,y,z;
    get_acc_xyz(&x, &y, &z);

    sample_xyz.x = INT16_T(x);
    sample_xyz.y = INT16_T(y);
    sample_xyz.z = INT16_T(z);

    if(filter(&cur_sample, &sample_xyz) == 0)
    {
        update_peak(&peak, &cur_sample);
        if (update_slid(&slid_sample, &cur_sample))
            detect_step(&peak,&slid_sample);
    }

#else
    if ((result.temp_sample_cnt % ACC_SAMPLING_RATE) == 0)
    {
        int8_t i = rand() & 0x3;
        result.total_steps += i;
        result.temp_steps += i;
    }
#endif

    // update speed estimations
    if (result.temp_sample_cnt >= ACC_SAMPLING_RATE * 10)
    {
        result.cadence = result.temp_steps * 6;
        result.speed = ((uint32_t)(result.stride_length * result.temp_steps) << 8) / (10 * 100);
        result.total_distance = result.stride_length * result.total_steps / 10;
        printf("%d,%d\n", result.speed, result.total_distance);

        result.temp_steps = 0;
        result.temp_sample_cnt = 0;
    }
}

const pedometer_info_t *pedometer_get_info(void)
{
    return &result;
}
