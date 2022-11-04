#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>

#define BOARD_ING91881B_02_02_05        1
//#define BOARD_......

#ifndef BOARD_ID
#define BOARD_ID       BOARD_ING91881B_02_02_05
#endif

#ifdef __cplusplus
extern "C" {
#endif


int setup_rgb_led(void);
int set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b);

int setup_env_sensor(void);
int get_temperature(void);
int get_humidity(void);
int get_pressure();

int setup_accelerometer(void);
void get_acc_xyz(float *x, float *y, float *z);

void setup_buzzer(void);
void set_buzzer_freq(uint16_t freq);

#ifdef __cplusplus
}
#endif

#endif