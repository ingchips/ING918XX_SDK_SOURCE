#include <string.h>
#include "peripheral_gpio.h"
#include "spi.h"
#include "lcd_config.h"

#include "FreeRTOS.h"
#include "task.h"

#include "lvgl.h"
#include "lcd_config.h"

#if (LV_COLOR_DEPTH != 16)
#error LV_COLOR_DEPTH != 16
#endif

#define delay_ms(n_ms)      vTaskDelay(pdMS_TO_TICKS(n_ms))

#define TFTLCD_DELAY8       0xff

static void lcd_init_table(const uint8_t *table, int size);
static void lcd_clear(void);

void lcd_write_cmd(unsigned char command)
{
	SPI_Delay(1);
	LCD_CS_0;
	LCD_DC_0;
	SPI_Write(command);
	LCD_CS_1;
}

void lcd_write_data(unsigned char command)
{
	LCD_CS_0;
	LCD_DC_1;
	SPI_Write(command);
}

#define LCD_WriteCMD lcd_write_cmd
#define LCD_WriteDATA lcd_write_data

#if (SPI_LCD_DRIVER == LCD_DRIVER_ST7735)

void lcd_init()
{
	LCD_RST_1;
	delay_ms(1);
	LCD_RST_0;
	delay_ms(1);
	LCD_RST_1;
	delay_ms(1);

    uint8_t init[] = {
        0x11,  0,
        0xB1,  3, 0x01, 0x2C, 0x2D,
        0xB2,  3, 0x01, 0x2C, 0x2D,
        0xB3,  6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,
        0xB4,  1, 0x07,
        0xC0,  5, 0xA2, 0x02, 0x84, 0xC1, 0xC5,
        0xC2,  2, 0x0A, 0x00,
        0xC3,  5, 0x8A, 0x2A, 0xC4, 0x8A, 0xEE,
        0xC5,  1, 0x0E,
        0xE0, 16, 0x0F, 0x1A, 0x0F, 0x18, 0x2F, 0x28, 0x20, 0x22, 0x1F, 0x1B, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10,
        0xE1, 16, 0x0F, 0x1B, 0x0F, 0x17, 0x33, 0x2C, 0x29, 0x2E, 0x30, 0x30, 0x39, 0x3F, 0x00, 0x07, 0x03, 0x10,
        0x2A,  4, 0x00, 0x00, 0x00, 0x7F,
        0x2B,  4, 0x00, 0x00, 0x00, 0x9F,
        0xF0,  3, 0x01, 0xF6, 0x00,
        0x3A,  1, 0x05,
        0x29,  0,
        0x36,  1,
#if (DISPLAY_HORIZONTAL)
        0xA0,
#else
        0xC0,
#endif
    };
    lcd_init_table(init, sizeof(init));

	lcd_clear();
 	LCD_On();
}

#elif (SPI_LCD_DRIVER == LCD_DRIVER_ST7789)

void lcd_init(void)
{
	int i,j;

	LCD_RST_1;
	delay_ms(1);
	LCD_RST_0;
	delay_ms(1);
	LCD_RST_1;
	delay_ms(1);

    uint8_t init[] = {
        0x01,  0,   //Software Reset
        TFTLCD_DELAY8, 1,
        0x11,  0,
        TFTLCD_DELAY8, 1,

        0x3A,  1, 0x05,
        0xC5,  1, 0x1A,
        0x36,  1,
#if (DISPLAY_HORIZONTAL)
        0x00,
#else
        0xC0,
#endif
        0xB2,  5, 0x05, 0x05, 0x00, 0x33, 0x33, // Porch Setting
        0xB7,  1, 0x05, // Gate Control
        0xBB,  1, 0x3F, // VCOM
        0xC0,  1, 0x2C, // Power control
        0xC2,  1, 0x01, // VDV and VRH Command Enable
        0xC3,  1, 0x0F, // 4.3+ (vcom+vcom offset+vdv)
        0xC4,  1, 0x20, // VDV Set (0v)
        0xC6,  1, 0x01, // Frame Rate Control in Normal Mode

        0xD0,  2, 0xA4, 0xA1,
        0xE8,  1, 0x03,
        0xE9,  3, 0x09, 0x09, 0x08,

        0xE0, 14, 0xD0, 0x05, 0x09, 0x09, 0x08, 0x14, 0x28, 0x33, 0x3F, 0x07, 0x13, 0x14, 0x28, 0x30,
        0xE1, 14, 0xD0, 0x05, 0x09, 0x09, 0x08, 0x03, 0x24, 0x32, 0x32, 0x3B, 0x14, 0x13, 0x28, 0x2F,

        0x20,  0,
        TFTLCD_DELAY8, 1,
        0x29,  0,
    };

	lcd_clear();
	LCD_On();
}

#elif (SPI_LCD_DRIVER == LCD_DRIVER_ILI9488)

void lcd_init(void)
{
    SPI_Init();

    LCD_RST_1;
	delay_ms(10);
    LCD_RST_0;
	delay_ms(50);
	LCD_RST_1;
	delay_ms(200);

    LCD_On();

    static const uint8_t init[] =
    {
        0xE0, 15, 0x00, 0x07, 0x0f, 0x0D, 0x1B, 0x0A, 0x3c, 0x78, 0x4A, 0x07, 0x0E, 0x09, 0x1B, 0x1e, 0x0f,
        0xE1, 15, 0x00, 0x22, 0x24, 0x06, 0x12, 0x07, 0x36, 0x47, 0x47, 0x06, 0x0a, 0x07, 0x30, 0x37, 0x0f,

        0xC0,  2, 0x10, 0x10,
        0xC1,  1, 0x41,
        0xC5,  3, 0x00, 0x22, 0x80,

        0x36, 1,
#if (DISPLAY_HORIZONTAL)
        0x28,
#else
        0x48,
#endif

        0x3A,  1, 0x66,

        0xB0,  1, 0x00,
        0xB1,  2, 0xB0, 0x11,
        0xB4,  1, 0x02,
        0xB6,  2, 0x02, 0x02,
        0xB7,  1, 0xC6,

        0xE9,  1, 0x00,
        0xF7,  4, 0xA9, 0x51, 0x2C, 0x82,

        0x11, 0,
        TFTLCD_DELAY8, 130,
        0x29, 0,
        TFTLCD_DELAY8, 50,
    };

    lcd_init_table(init, sizeof(init));

    lcd_clear();
}

#define DRIVER_SPECIFIC_IMPL

static void lcd_send_16(int32_t num, const uint16_t *color_data)
{
    uint8_t data[3];

	SPI_Init();

	LCD_CS_0;
	LCD_DC_1;

    for (; num > 0; num--)
    {
        lv_color_t cl;
        cl.full = *color_data++;
        data[0] = cl.ch.red << 3;
        data[1] = cl.ch.green << 2;
        data[2] = cl.ch.blue << 3;
        SPI_Write_FIFO_8(data, sizeof(data));
    }

    SPI_WaitTxDone();

	LCD_CS_1;
}

static void lcd_send_fixed_16(int32_t num, const uint16_t color)
{
	uint8_t data[3];
    lv_color_t cl;

    cl.full = color;
    data[0] = cl.ch.red << 3;
    data[1] = cl.ch.green << 2;
    data[2] = cl.ch.blue << 3;

	SPI_Init();

	LCD_CS_0;
	LCD_DC_1;

    for (; num > 0; num--)
    {
        SPI_Write_FIFO_8(data, sizeof(data));
    }

    SPI_WaitTxDone();

	LCD_CS_1;
}

#else

#error unknown SPI_LCD_DRIVER

#endif

static void lcd_init_table(const uint8_t *table, int size)
{
    uint8_t i;
    const uint8_t *p = table;
    while (size > 0)
	{
        uint8_t cmd = *p++;
        uint8_t len = *p++;
        if (cmd == TFTLCD_DELAY8)
		{
            delay_ms(len);
            len = 0;
        }
		else
		{
            lcd_write_cmd(cmd);
            for (; len > 0; len--)
                lcd_write_data(*p++);
        }
        size -= len + 2;
    }
}

void lcd_set_window(uint16_t x1,uint16_t y1, uint16_t x2, uint16_t y2)
{
    SPI_Init();

    lcd_write_cmd(0x2a);
    lcd_write_data(x1 >> 8);
    lcd_write_data(x1);
    lcd_write_data(x2 >> 8);
    lcd_write_data(x2);

    lcd_write_cmd(0x2b);
    lcd_write_data(y1 >> 8);
    lcd_write_data(y1);
    lcd_write_data(y2 >> 8);
    lcd_write_data(y2);

    lcd_write_cmd(0x2c);
}

#ifndef DRIVER_SPECIFIC_IMPL

static void lcd_send_16(int32_t num, const uint16_t *color_data)
{
	SPI_Init16bit();

	LCD_CS_0;
	LCD_DC_1;

    SPI_Write_FIFO_16(color_data, num);
    SPI_WaitTxDone();

	LCD_CS_1;
}

static void lcd_send_fixed_16(int32_t num, const uint16_t color)
{
	int32_t i;

	SPI_Init16bit();

	LCD_CS_0;
	LCD_DC_1;

    SPI_Fill_FIFO_16(color, num);
    SPI_WaitTxDone();

	LCD_CS_1;
}

#endif

void lcd_draw(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const uint16_t *color)
{
    int num = (int)(x2 - x1 + 1) * (y2 - y1 + 1);
    lcd_set_window(x1, y1, x2, y2);
    lcd_send_16(num, color);
}

void lcd_fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    int num = (int)(x2 - x1 + 1) * (y2 - y1 + 1) + 15;
    lcd_set_window(x1, y1, x2, y2);
    lcd_send_fixed_16(num, color);
}

static void lcd_clear(void)
{
    lcd_fill(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, COLOR_CLEAR);
}
