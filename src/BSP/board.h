#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>
#include "board_macro_ctrl.h"

#define BOARD_ING91881B_02_02_04        1
#define BOARD_ING91881B_02_02_05        2
#define BOARD_ING91881B_02_02_06        3
//#define BOARD_......

#ifndef BOARD_ID
#define BOARD_ID       BOARD_ING91881B_02_02_05
#endif

//LED宏开关
#ifndef MACRO_SWITCH_RGB_LED_OFF
#define MACRO_SWITCH_RGB_LED_OFF        0
#endif
#ifndef MACRO_SWITCH_RGB_LED_ON
#define MACRO_SWITCH_RGB_LED_ON         1
#endif
#ifndef MACRO_SWITCH_RGB_LED
#define MACRO_SWITCH_RGB_LED    MACRO_SWITCH_RGB_LED_OFF
#endif

//温度计宏开关
#ifndef MACRO_SWITCH_THERMO_OFF
#define MACRO_SWITCH_THERMO_OFF     0
#endif
#ifndef MACRO_SWITCH_THERMO_ON
#define MACRO_SWITCH_THERMO_ON      1
#endif
#ifndef MACRO_SWITCH_THERMO
#define MACRO_SWITCH_THERMO     MACRO_SWITCH_THERMO_OFF
#endif

//加速度计宏开关
#ifndef MACRO_SWITCH_ACCEL_OFF
#define MACRO_SWITCH_ACCEL_OFF      0
#endif
#ifndef MACRO_SWITCH_ACCEL_ON
#define MACRO_SWITCH_ACCEL_ON       1
#endif
#ifndef MACRO_SWITCH_ACCEL
#define MACRO_SWITCH_ACCEL      MACRO_SWITCH_ACCEL_OFF
#endif

//蜂鸣器宏开关
#ifndef MACRO_SWITCH_BUZZER_OFF
#define MACRO_SWITCH_BUZZER_OFF     0
#endif
#ifndef MACRO_SWITCH_BUZZER_ON
#define MACRO_SWITCH_BUZZER_ON      1
#endif
#ifndef MACRO_SWITCH_BUZZER
#define MACRO_SWITCH_BUZZER     MACRO_SWITCH_BUZZER_OFF
#endif


#define BME280_ADDR     BME280_I2C_ADDR_PRIM
#define BME280_INTF_RET_TYPE        int8_t

#ifdef __cplusplus
extern "C" {
#endif

void setup_rgb_led(void);
void set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b);

typedef enum {
    CONVERT_T            	= 0xCC44,   //测量温度
    WRITE_ALERT_HI_SET   	= 0x611d,   //设置报警门限上阈值
    WRITE_ALERT_HI_UNSET	= 0x6116,   //设置报警清除门限上阈值
    WRITE_ALERT_LO_SET   	= 0x6100,   //设置报警清除门限下阈值
    WRITE_ALERT_LO_UNSET	= 0x610b,   //设置报警门限下阈值
    READ_ALERT_HI_SET    	= 0xe11f,   //读取报警门限上阈值
    READ_ALERT_HI_UNSET	    = 0xe114,   //读取报警清除门限上阈值
    READ_ALERT_LO_SET    	= 0xe102,   //读取报警清除门限下阈值
    READ_ALERT_LO_UNSET	    = 0xe109,   //读取报警门限下阈值
    WRITE_CONFIG         	= 0x5206,	//写配置寄存器
    READ_STATUSCONFIG     	= 0xf32d,   //读取状态和配置寄存器
    CLEAR_STATUS         	= 0x3041,   //清除状态寄存器
    BREAK                	= 0x3093,   //终止周期测量
	SOFT_RST            	= 0x30a2,	//软件复位
    COPY_PAGE0           	= 0xcc48,   //保存page0到mtp
    RECALL_EE            	= 0xccb8,   //恢复ee区域
    RECALL_PAGE0_RES     	= 0xccb6,	//恢复page0的扩展区域
} I2C_CMD;

void setup_env_sensor();
float get_temperature(void);
float get_humidity(void);
float get_pressure(void);

void setup_accelerometer(void);
void get_acc_xyz(float *x, float *y, float *z);

void setup_buzzer(const uint8_t buzz_pin, const uint8_t pwm_channel);
void set_buzzer_freq(uint32_t freq);

#ifdef __cplusplus
}
#endif

#endif