
#ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;

#include "spi.h"

#define EPD_W21_WRITE_DATA 1
#define EPD_W21_WRITE_CMD  0

#define BW_RAM_ADDR            0x10
#define RED_RAM_ADDR           0x13

extern void driver_delay_us(unsigned int xus);
extern void driver_delay_xms(unsigned int ms);

void OLED_Clear(void);   
void OLED_ShowString(u8 x,u8 y, u8 *p) ;
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
 
void OLED_Init(void);
void OLED_Display(const char *str);

uint8_t OLED_Display_sensor(uint8_t Hr,uint32_t step);
uint8_t OLED_Display_Hr(uint8_t Hr);
uint8_t OLED_Display_Steps(uint16_t step);

void OLED_ClrScr(uint8_t _ucMode);

void OLED_DispOff(void);

#endif
/***********************************************************
						end file
***********************************************************/


