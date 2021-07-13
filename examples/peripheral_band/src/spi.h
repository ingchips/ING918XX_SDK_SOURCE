#ifndef __SPI_H__
#define __SPI_H__

#include "ingsoc.h"

#define SPI_LCD_POWER  	GIO_GPIO_9
#define SPI_LCD_DC      GIO_GPIO_8
#define SPI_LCD_RST   	GIO_GPIO_7
#define SPI_LCD_CS    	GIO_GPIO_5
#define SPI_LCD_SCLK   	GIO_GPIO_0
#define SPI_LCD_MOSI   	GIO_GPIO_1

#define LCD_CS_0		GIO_WriteValue(SPI_LCD_CS, 0) 
#define LCD_CS_1		GIO_WriteValue(SPI_LCD_CS, 0)  

#define LCD_DC_0		GIO_WriteValue(SPI_LCD_DC, 0) 
#define LCD_DC_1		GIO_WriteValue(SPI_LCD_DC, 1)  

#define LCD_RST_0		GIO_WriteValue(SPI_LCD_RST, 0) 
#define LCD_RST_1		GIO_WriteValue(SPI_LCD_RST, 1)  
  
// Powers control
#define  LCD_On()       GIO_WriteValue(SPI_LCD_POWER, 0);
#define  LCD_Off()      GIO_WriteValue(SPI_LCD_POWER, 1); 

#define DummyByte       0xA5
#define DummyWord       0xA5A5A5A5

#define W8_2(addr)      ((addr >> 16) & 0xFFU)
#define W8_1(addr)      ((addr >> 8) & 0xFFU)
#define W8_0(addr)      (addr& 0xFFU)

void SPI_Init(SSP_TypeDef * SSP_Ptr);
 
void SPI_Write(uint8_t data);
 
void SPI_Delay(uint8_t t);

#endif

