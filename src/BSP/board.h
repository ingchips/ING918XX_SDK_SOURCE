#ifndef __BOARD_H
#define __BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/*添加SIMULATION类型开关，添加随机数模拟传感器*/

void get_temperature();

void get_humidity();

void get_acc_xyz();

void set_rgb(uint8_t r, uint8_t g, uint8_t b);


#ifdef __cplusplus
}
#endif

#endif