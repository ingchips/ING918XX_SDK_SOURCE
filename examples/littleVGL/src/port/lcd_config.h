#ifndef __LCD_CONFIG_H__
#define __LCD_CONFIG_H__

#include "ingsoc.h"
#include "peripheral_gpio.h"

// NOTE: COLOR_DEPTH must be 16

#ifndef COLOR_CLEAR
#define COLOR_CLEAR     0x0000
#endif

#define LCD_DRIVER_ST7735       0
#define LCD_DRIVER_ST7789       1
#define LCD_DRIVER_ILI9488      2
#define LCD_DRIVER_ILI9341      3

#ifndef LCD_WIDTH
#define LCD_WIDTH               128
#endif

#ifndef LCD_HEIGHT
#define LCD_HEIGHT              160
#endif

#ifndef SPI_LCD_DRIVER
#define SPI_LCD_DRIVER          LCD_DRIVER_ST7735
#endif

#if (LCD_WIDTH > LCD_HEIGHT)
#define DISPLAY_HORIZONTAL   1
#else
#define DISPLAY_HORIZONTAL   0
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#define SPI_LCD_BL  	GIO_GPIO_17
#define SPI_LCD_CS    	GIO_GPIO_9
#define SPI_LCD_DC      GIO_GPIO_6
#define SPI_LCD_RST   	GIO_GPIO_1
#define SPI_LCD_MOSI   	GIO_GPIO_19
#define SPI_LCD_SCLK   	GIO_GPIO_18
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
#define SPI_LCD_BL  	GIO_GPIO_28
#define SPI_LCD_CS    	GIO_GPIO_27
#define SPI_LCD_DC      GIO_GPIO_26
#define SPI_LCD_RST   	GIO_GPIO_23
#define SPI_LCD_MOSI   	GIO_GPIO_22
#define SPI_LCD_SCLK   	GIO_GPIO_21
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
#define SPI_LCD_BL  	GIO_GPIO_14
#define SPI_LCD_CS    	GIO_GPIO_13
#define SPI_LCD_DC      GIO_GPIO_12
#define SPI_LCD_RST   	GIO_GPIO_11
#define SPI_LCD_MOSI   	GIO_GPIO_10
#define SPI_LCD_SCLK   	GIO_GPIO_9
#endif

#define LCD_CS_0		GIO_WriteValue(SPI_LCD_CS, 0)
#define LCD_CS_1		GIO_WriteValue(SPI_LCD_CS, 1)

#define LCD_DC_0		GIO_WriteValue(SPI_LCD_DC, 0)
#define LCD_DC_1		GIO_WriteValue(SPI_LCD_DC, 1)

#define LCD_RST_0		GIO_WriteValue(SPI_LCD_RST, 0)
#define LCD_RST_1		GIO_WriteValue(SPI_LCD_RST, 1)

// Back light control
#define  LCD_On()       GIO_WriteValue(SPI_LCD_BL, 1);
#define  LCD_Off()      GIO_WriteValue(SPI_LCD_BL, 0);


void lcd_init(void);
void lcd_draw(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const uint16_t *color);
void lcd_fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

#endif
