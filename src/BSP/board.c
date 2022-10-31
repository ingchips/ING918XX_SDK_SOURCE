/*将所有传感器以及LED驱动进行集中处理*/
/*添加SIMULATION开关*/

#include "board.h"
#include "ingsoc.h"

/* rgb led灯驱动 */
#include "rgb_led.c"

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t cmd = (0x3a << 24) | (b << 16) | (r << 8) | g;
#if(RGB_LED == LED_TLC59731)
    tlc59731_write(cmd);
#elif(RGB_LED == LED_WS2881)
    ws2881_write(cmd);
#endif
} 


/*加速度计*/
#include "bma2x2.c"
void get_acc_xyz(struct bma2x2_accel_data accel)
{
    bma2x2_read_accel_xyz(&accel);
}


/*温度计*/
#include "bme280.c"
uint8_t dev_addr = BME280_ADDR;
struct bme280_dev bme280_data =
{
    .intf_ptr = &dev_addr,
    .intf = BME280_I2C_INTF,
    .read = user_i2c_read,
    .write = user_i2c_write,
    .delay_us = user_delay_us,
    /* Recommended mode of operation: Indoor navigation */
    .settings =
    {
        .osr_h = BME280_OVERSAMPLING_1X,
        .osr_p = BME280_OVERSAMPLING_16X,
        .osr_t = BME280_OVERSAMPLING_2X,
        .filter = BME280_FILTER_COEFF_16,
        .standby_time = BME280_STANDBY_TIME_62_5_MS,
    },
};

void get_temperature(struct bme280_data comp_data)
{
    bme280_get_sensor_data(BME280_ALL, &comp_data, &bme280_data)
}
