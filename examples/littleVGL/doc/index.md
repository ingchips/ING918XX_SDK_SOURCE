# LittlevGL (LVGL)

This example is a port of [LVGL](https://lvgl.io) on ING918xx/ING916xx chip families.

A collection of TFT LCD modules is supported by specifying compiling macroes (see [LCD driver](../src/port/lcd_drv.c)):

| Macro Name |  Note               |       Values        | Default |
|: -------- :| : --------------- : | : --------------- : | : ----- :|
|  SPI_LCD_DRIVER  | Name of the driver chip | <ul><li>LCD_DRIVER_ST7735</li><li>LCD_DRIVER_ST7789</li><li>LCD_DRIVER_ILI9488</li></ul> |LCD_DRIVER_ST7735 |
|  LCD_WIDTH  | Horizontal resolution of the LCD in pixels | Integer | 128 |
|  LCD_HEIGHT | Vertical resolution of the LCD in pixels  | Integer | 160 |

These TFT LCD modules have been tested:

* [1.44inch SPI Module ST7735S (128x128)](http://www.lcdwiki.com/1.44inch_SPI_Module_ST7735S_SKU:MSP1443)
* [1.8inch SPI Module ST7735S (128x160)](http://www.lcdwiki.com/1.8inch_SPI_Module_ST7735S_SKU:MSP1803)
* [3.5inch SPI Module ILI9488](http://www.lcdwiki.com/3.5inch_SPI_Module_ILI9488_SKU:MSP3520)

## Hardware Setup

Take ING918xx Dev-Board as an example, the LCD module is connected like this:

<img src="./img/hardware.png" width="40%" />
