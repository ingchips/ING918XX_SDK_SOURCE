
#ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_
 
#include "spi.h"

void pic_display(const uint8_t *black_white, const uint8_t *red_white);

void LCD_Init(void);
void LCD_Sleep(void);
#endif
/***********************************************************
						end file
***********************************************************/


