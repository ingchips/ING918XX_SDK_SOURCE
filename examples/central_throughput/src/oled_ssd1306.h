#ifndef __OLED_H
#define __OLED_H

#include <stdint.h>
#define u8 uint8_t
#define u32 uint32_t

#define OLED_MODE 0
#define SIZE 8
#define XLevelL         0x00
#define XLevelH         0x10
#define Max_Column      128
#define Max_Row         64
#define	Brightness      0xFF 
#define X_WIDTH         128
#define Y_WIDTH         64

#define OLED_ADDR   0x78

#define OLED_CMD  0
#define OLED_DATA 1

#define I2C_PORT        I2C_PORT_0

#ifndef PIN_SCL
#define PIN_SCL       7
#define PIN_SDA       8
#endif

//OLED¿ØÖÆÓÃº¯Êý
void OLED_WR_Byte(unsigned dat,unsigned cmd);  
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,char chr,u8 mode,u8 Char_Size);
void OLED_ShowString(u8 x,u8 y, const char *p,u8 mode,u8 Char_Size);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1, const uint8_t *BMP);
void OLED_Fill_Picture(unsigned char fill_Data);

#endif  
	 



