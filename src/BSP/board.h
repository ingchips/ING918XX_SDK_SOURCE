#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>

#define BOARD_ING91881B_02_02_04        1
#define BOARD_ING91881B_02_02_05        2
#define BOARD_ING91881B_02_02_06        3

#ifndef BOARD_ID
#define BOARD_ID       BOARD_ING91881B_02_02_06
#endif

#define BME280_ADDR     BME280_I2C_ADDR_PRIM
#define BME280_INTF_RET_TYPE        int8_t

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 * @brief  Configure the initialization of rgb led and set the rgb value to (50,50,50).
 * 
 * tlc59731 and ws2811 are supported.The type of RGB_LED driver chip is tlc59731 or ws2811
 * determined by the BOARD_ID.
 * 
 * Note:To use the rgb led, you need to add the macro 'BOARD_RGB_LED'definition to the keil 
 * project.
 * 
 ****************************************************************************************
 */
void setup_rgb_led(void);

/**
 ****************************************************************************************
 * @brief Set the rgb led color
 * 
 * @param r the red color
 * @param g the green color
 * @param b the blue color
 * 
 ****************************************************************************************
 */
void set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b);

/**
 ****************************************************************************************
 * @brief  Configure the initialization of sensor
 * 
 * BME280 and MTS01B are supported.
 * 
 ****************************************************************************************
 */
void setup_env_sensor();

/**
 ****************************************************************************************
 * @brief Get the temperature
 * 
 * Note:To use the thermometer, you need to add the macro 'BOARD_SENSOR_THERMO'definition 
 * to the keil project.
 * 
 * @return Temperature measurement value with float type
 ****************************************************************************************
 */
float get_temperature(void);

/**
 ****************************************************************************************
 * @brief Get the humidity
 * 
 * @return Humidity measurement value with float type
 ****************************************************************************************
 */
float get_humidity(void);

/**
 ****************************************************************************************
 * @brief Get the pressure
 * 
 * @return Pressure measurement value with float type
 ****************************************************************************************
 */
float get_pressure(void);

/**
 ****************************************************************************************
 * @brief Set the up accelerometer
 * 
 * BMA280 and STK8BA58 are supported. And it supported the same code.
 * 
 * Note:To use the accelerometer, you need to add the macro 'BOARD_SENSOR_THERMO'definition 
 * to the keil project.
 * 
 ****************************************************************************************
 */
void setup_accelerometer(void);

/**
 ****************************************************************************************
 * @brief Get the acc xyz object
 * 
 * @param x X-axis acceleration value
 * @param y Y-axis acceleration value
 * @param z Z-axis acceleration value
 * 
 **************************************************************************************** 
 */
void get_acc_xyz(float *x, float *y, float *z);

/**
 ****************************************************************************************
 * @brief Configure the initialization of the buzzer
 * 
 * Note:To use the accelerometer, you need to add the macro 'BOARD_SENSOR_BUZZER'definition 
 * to the keil project.
 * 
 ****************************************************************************************
 */
void setup_buzzer();

/**
 ****************************************************************************************
 * @brief Set the PWM input frequency of the buzzer
 * 
 * @param freq 
 * 
 ****************************************************************************************
 */
void set_buzzer_freq(uint16_t freq);

/**
 ****************************************************************************************
 * @brief Configure the initialization of keys
 * 
 * Six keys are initialized by default
 * 
 ****************************************************************************************
 */
void setup_keyconfigure();

#ifdef __cplusplus
}
#endif

#endif