#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>

#define BOARD_ING91881B_02_02_04        1
#define BOARD_ING91881B_02_02_05        2
#define BOARD_ING91881B_02_02_06        3

//#define BOARD_......

#ifndef BOARD_ID
#define BOARD_ID       BOARD_ING91881B_02_02_05
#endif

#define BME280_ADDR     BME280_I2C_ADDR_PRIM
#define BME280_INTF_RET_TYPE        int8_t

#ifdef __cplusplus
extern "C" {
#endif

void setup_rgb_led(void);
void set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b);

typedef enum {
    CONVERT_T            	= 0xCC44,   //Measure the temperature
    WRITE_ALERT_HI_SET   	= 0x611d,   //Sets the alarm threshold threshold
    WRITE_ALERT_HI_UNSET	= 0x6116,   //Sets the upper threshold for alarm clearing
    WRITE_ALERT_LO_SET   	= 0x6100,   //Sets the threshold under the alarm clearing threshold
    WRITE_ALERT_LO_UNSET	= 0x610b,   //Sets the alarm threshold threshold
    READ_ALERT_HI_SET    	= 0xe11f,   //Upper threshold of the Read Alarm threshold
    READ_ALERT_HI_UNSET	    = 0xe114,   // Upper threshold of the Read alarm Clearing threshold
    READ_ALERT_LO_SET    	= 0xe102,   //Threshold below the Read alarm Clearance threshold
    READ_ALERT_LO_UNSET	    = 0xe109,   //Read Alarm threshold lower threshold
    WRITE_CONFIG         	= 0x5206,	//Write configuration register
    READ_STATUSCONFIG     	= 0xf32d,   //Read the status and configuration registers
    CLEAR_STATUS         	= 0x3041,   //Clear the status register
    BREAK                	= 0x3093,   //Terminate the period measurement
	SOFT_RST            	= 0x30a2,	//Software reset
    COPY_PAGE0           	= 0xcc48,   //Save page0 to mtp
    RECALL_EE            	= 0xccb8,   //Restore the ee area
    RECALL_PAGE0_RES     	= 0xccb6,	//Restores the extended area of page0
} I2C_CMD;

void setup_env_sensor();
float get_temperature(void);
float get_humidity(void);
float get_pressure(void);

void setup_accelerometer(void);
void get_acc_xyz(float *x, float *y, float *z);

void setup_buzzer();
void set_buzzer_freq(uint32_t freq);

void config_key(const uint8_t *key_pins, int key_num, const GIO_Direction_t dir);

#ifdef __cplusplus
}
#endif

#endif