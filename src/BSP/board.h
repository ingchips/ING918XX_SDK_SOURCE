#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>

#define BOARD_ING91881B_02_02_05        1
#define BOARD_ING91881B_02_02_06        2
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
    CONVERT_T            	= 0xCC44,   //娴嬮噺娓╁害
    BREAK                	= 0x3093,   //涓鍛ㄦ湡娴嬮噺
    READ_STATUSCONFIG     	= 0xf32d,   //璇诲彇鐘舵€佸拰閰嶇疆瀵勫瓨鍣�
    CLEAR_STATUS         	= 0x3041,   //娓呴櫎鐘舵€佸瘎瀛樺櫒
    WRITE_CONFIG         	= 0x5206,	//鍐欓厤缃瘎瀛樺櫒
    READ_ALERT_HI_SET    	= 0xe11f,   //璇诲彇鎶ヨ闂ㄩ檺涓婇槇鍊�
    READ_ALERT_LO_SET    	= 0xe102,   //璇诲彇鎶ヨ娓呴櫎闂ㄩ檺涓嬮槇鍊�
    WRITE_ALERT_HI_SET   	= 0x611d,   //璁剧疆鎶ヨ闂ㄩ檺涓婇槇鍊�
    WRITE_ALERT_LO_SET   	= 0x6100,   //璁剧疆鎶ヨ娓呴櫎闂ㄩ檺涓嬮槇鍊�
    COPY_PAGE0           	= 0xcc48,   //淇濆瓨page0鍒癿tp
    RECALL_EE            	= 0xccb8,   //鎭㈠ee鍖哄煙
    RECALL_PAGE0_RES     	= 0xccb6,	//鎭㈠page0鐨勬畫鐣欏尯鍩�
	SOFT_RST            	= 0x30a2,	//杞欢澶嶄綅
	WRITE_ALERT_LO_UNSET	= 0x610b,   //璁剧疆鎶ヨ闂ㄩ檺涓嬮槇鍊�
	WRITE_ALERT_HI_UNSET	= 0x6116,   //璁剧疆鎶ヨ娓呴櫎闂ㄩ檺涓婇槇鍊�
    READ_ALERT_HI_UNSET	    = 0xe114,   //璇诲彇鎶ヨ娓呴櫎闂ㄩ檺涓婇槇鍊�
    READ_ALERT_LO_UNSET	    = 0xe109,   //璇诲彇鎶ヨ闂ㄩ檺涓嬮槇鍊�

} I2C_CMD;

void setup_env_sensor(BME280_INTF_RET_TYPE *read,BME280_INTF_RET_TYPE *write, void *delay);
double get_temperature(void);
double get_humidity(void);
double get_pressure(void);

void setup_accelerometer(void);
void get_acc_xyz(float *x, float *y, float *z);

void setup_buzzer(void);
void set_buzzer_freq(uint16_t freq);

#ifdef __cplusplus
}
#endif

#endif