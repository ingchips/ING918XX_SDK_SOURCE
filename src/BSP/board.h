#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>

#define BOARD_ING91881B_02_02_05        1
//#define BOARD_......

#ifndef BOARD_ID
#define BOARD_ID       BOARD_ING91881B_02_02_05
#endif

#define BME280_ADDR     BME280_I2C_ADDR_PRIM
#define BME280_INTF_RET_TYPE        int8_t

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    union
    {
        struct {uint8_t r, g, b;};
        uint32_t value;
    };
} rgb_t;

typedef struct 
{
    rgb_t rgb0, rgb1, cur;
    int dir;
} breathing_t;

void setup_rgb_led(void);
void set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b);
void setup_rgb_breathing(void);
void set_rbg_breathing(rgb_t rgb0, rgb_t rgb1);

void setup_env_sensor(void);
void regist_init(BME280_INTF_RET_TYPE *read,BME280_INTF_RET_TYPE *write, void *delay);
double get_temperature(void);
double get_humidity(void);
double get_pressure();

int setup_accelerometer(void);
void get_acc_xyz(float *x, float *y, float *z);

void setup_buzzer(void);
void set_buzzer_freq(uint16_t freq);

#ifdef __cplusplus
}
#endif

#endif