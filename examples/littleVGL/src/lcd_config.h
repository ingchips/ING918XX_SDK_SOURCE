#ifndef __LCD_CONFIG_H__
#define __LCD_CONFIG_H__

#include "ingsoc.h"
#include "peripheral_gpio.h"
#include "lv_color.h"
//LCDMODULE_LCD128160:Z180SN019
//LCDMODULE_LCD240320:GMT024-8P10P-SPI
//#define LCDMODULE_LCD128160


#ifdef LCDMODULE_LCD128160
#define LCD_WIDTH   128
#define LCD_HEIGHT	160

#define SPI_LCD_POWER  	GIO_GPIO_17
#define SPI_LCD_CS    	GIO_GPIO_9
#define SPI_LCD_DC      GIO_GPIO_6
#define SPI_LCD_RST   	GIO_GPIO_1
#define SPI_LCD_MOSI   	GIO_GPIO_19
#define SPI_LCD_SCLK   	GIO_GPIO_18

#elif defined(LCDMODULE_LCD240320)
#define LCD_WIDTH   240
#define LCD_HEIGHT	320

#define SPI_LCD_POWER  	GIO_GPIO_17
#define SPI_LCD_CS    	GIO_GPIO_11
#define SPI_LCD_DC      GIO_GPIO_10
#define SPI_LCD_RST   	GIO_GPIO_8
#define SPI_LCD_MOSI   	GIO_GPIO_19
#define SPI_LCD_SCLK   	GIO_GPIO_18

#else
#define LCD_WIDTH   128
#define LCD_HEIGHT	128
#endif







#define LCD_CS_0		GIO_WriteValue(SPI_LCD_CS, 0) 
#define LCD_CS_1		GIO_WriteValue(SPI_LCD_CS, 1)  

#define LCD_DC_0		GIO_WriteValue(SPI_LCD_DC, 0) 
#define LCD_DC_1		GIO_WriteValue(SPI_LCD_DC, 1)  

#define LCD_RST_0		GIO_WriteValue(SPI_LCD_RST, 0) 
#define LCD_RST_1		GIO_WriteValue(SPI_LCD_RST, 1)  

#define SPI_LCD_SCLK_0		GIO_WriteValue(SPI_LCD_RST, 0) 
#define SPI_LCD_SCLK_1		GIO_WriteValue(SPI_LCD_RST, 1)  

// Powers control
#define  LCD_On()       GIO_WriteValue(SPI_LCD_POWER, 1);
#define  LCD_Off()      GIO_WriteValue(SPI_LCD_POWER, 0); 


#define LCD_PWR(n) (n?\
GIO_WriteValue(SPI_LCD_POWER,1):\
GIO_WriteValue(SPI_LCD_POWER,0))
#define LCD_WR_RS(n) (n?\
GIO_WriteValue(SPI_LCD_DC,1):\
GIO_WriteValue(SPI_LCD_DC,0))
#define LCD_RST(n) (n?\
GIO_WriteValue(SPI_LCD_RST,1):\
GIO_WriteValue(SPI_LCD_RST,0))

void lcd_set_window(uint16_t x1,uint16_t y1, uint16_t x2,uint16_t y2 );
void lcd_draw(uint16_t Width, uint16_t Height, void *data);
void  lcd_initial(void);

#endif
