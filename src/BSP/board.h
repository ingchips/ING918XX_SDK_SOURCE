#ifndef __BOARD_H
#define __BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

void get_temperature(struct bme280_data comp_data);

void get_acc_xyz(struct bma2x2_accel_data accel);

void set_rgb(uint8_t r, uint8_t g, uint8_t b);


#ifdef __cplusplus
}
#endif

#endif