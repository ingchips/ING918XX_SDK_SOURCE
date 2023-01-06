#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>

#define BOARD_ING91881B_02_02_04        1
#define BOARD_ING91881B_02_02_05        2
#define BOARD_ING91881B_02_02_06        3
#define BOARD_DB682AC1A                 4

#ifndef BOARD_ID
#if   (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    #define BOARD_ID       BOARD_ING91881B_02_02_06
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    #define BOARD_ID       BOARD_DB682AC1A
#endif
#endif

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
 * Note:To use the rgb led, you need to add the macro 'BOARD_USE_RGB_LED' definition to the keil
 * project.
 *
 ****************************************************************************************
 */
void setup_rgb_led(void);

/**
 ****************************************************************************************
 * @brief Set the rgb led color
 *
 * @param r             the red color   (0..255)
 * @param g             the green color (0..255)
 * @param b             the blue color  (0..255)
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
void setup_env_sensor(void);

/**
 ****************************************************************************************
 * @brief Get the temperature
 *
 * Note:To use the thermometer, you need to add the macro 'BOARD_USE_THERMO' definition
 * to the keil project.
 *
 * @return Temperature measurement value with float type.（unit is 0.01°C）
 ****************************************************************************************
 */
float get_temperature(void);

/**
 ****************************************************************************************
 * @brief Get the humidity
 *
 * @return Humidity measurement value with float type.（unit is %）
 ****************************************************************************************
 */
float get_humidity(void);

/**
 ****************************************************************************************
 * @brief Get the pressure
 *
 * @return Pressure measurement value with float type.（unit is 1Pascal）
 ****************************************************************************************
 */
float get_pressure(void);

/**
 ****************************************************************************************
 * @brief Gets the I2C address of the accelerometer
 *
 * @return uint16_t address
 ****************************************************************************************
 */
uint16_t get_thermo_addr(void);

/**
 ****************************************************************************************
 * @brief Set the up accelerometer
 *
 * BMA280 and STK8BA58 are supported. And it supported the same code.
 *
 * Note:To use the accelerometer, you need to add the macro 'BOARD_USE_ACCEL' definition
 * to the keil project.
 *
 ****************************************************************************************
 */
void setup_accelerometer(void);

/**
 ****************************************************************************************
 * @brief Get the acc xyz object
 *
 * @param x X-axis acceleration value in m/s^2
 * @param y Y-axis acceleration value in m/s^2
 * @param z Z-axis acceleration value in m/s^2
 *
 ****************************************************************************************
 */
void get_acc_xyz(float *x, float *y, float *z);

/**
 ****************************************************************************************
 * @brief Gets the I2C address of the accelerometer
 *
 * @return uint16_t address
 ****************************************************************************************
 */
uint16_t get_acc_addr(void);

/**
 ****************************************************************************************
 * @brief Configure the initialization of the buzzer
 *
 * Note:To use the accelerometer, you need to add the macro 'BOARD_USE_BUZZER' definition
 * to the keil project.
 *
 ****************************************************************************************
 */
void setup_buzzer(void);

/**
 ****************************************************************************************
 * @brief Set the PWM input frequency of the buzzer
 *
 * @param freq          frequency in Hz
 *
 ****************************************************************************************
 */
void set_buzzer_freq(uint16_t freq);

/**
 ****************************************************************************************
 * @brief Configure the initialization of keys
 *
 * All keys on the board are initialized.
 *
 ****************************************************************************************
 */
void setup_keys(void);

#ifdef __cplusplus
}
#endif

#endif
