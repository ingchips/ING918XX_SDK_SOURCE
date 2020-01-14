/***********************************************************
Copyright(C), Yk Technology
FileName    : 
Author        : Yukewantong, Zhongxiaodong
Date              : 2011/12/22
Description    : 
Version        : V1.0
History        : 
--------------------------------
2011/12/22: created
2012/02/29: update the pvi's waveform(20C) as default
2012/03/18: update the EPD initinal step, add function-->
            void EPD_W21_PowerOnInit(void)
***********************************************************/

#include "Display_EPD_W21.h"
#include "screen.h"
#include <stdio.h>

#define EPD_W21_WRITE_DATA 1
#define EPD_W21_WRITE_CMD  0

#define BW_RAM_ADDR            0x10
#define RED_RAM_ADDR           0x13

void driver_delay_xms(uint32_t ms);

void EPD_W21_Init(void)
{ 
    //EPD_W21_BS_0; // 4 wire spi mode selected
    EPD_W21_RST_0;    // Module reset
    driver_delay_xms(1000);
    EPD_W21_RST_1;
    driver_delay_xms(10); 
}

void EPD_W21_WriteCMD(unsigned char command)
{
    SPI_Delay(1);
    EPD_W21_CS_0;                   
    EPD_W21_DC_0;        // command write
    SPI_Write(command);
    EPD_W21_CS_1;
}


void EPD_W21_WriteDATA(unsigned char command)
{
    SPI_Delay(1);
    EPD_W21_CS_0;                   
    EPD_W21_DC_1;        // command write
    SPI_Write(command);
    EPD_W21_CS_1;
}

void lcd_chkstatus(void)
{
    while(1)
    {     
        if(isEPD_W21_BUSY==0) break;
            printf("EPD BUSY!\r\n");
    }                 
}

// 0 => black/red
// 1 => white
void pic_display(const uint8_t *black_white, const uint8_t *red_white)
{
    unsigned int i;
    EPD_W21_WriteCMD(0x10);            //开始传输黑白图像 
    for(i = 0; i< IMAGE_BYTE_SIZE; i++)         
    { 
        EPD_W21_WriteDATA(black_white != NULL ? black_white[i] : 0xff);
    }
    
    EPD_W21_WriteCMD(0x13);            //开始传输红图像 
    for(i = 0; i< IMAGE_BYTE_SIZE; i++)
    { 
        EPD_W21_WriteDATA(red_white != NULL ? red_white[i] : 0xff);
    }   

    EPD_W21_WriteCMD(0x12);  
    lcd_chkstatus(); 
}

void LCD_Init(void)
{  
    EPD_On();
    EPD_W21_Init(); 
    EPD_W21_WriteCMD(0x06);         //boost设定
    EPD_W21_WriteDATA (0x17);
    EPD_W21_WriteDATA (0x17);
    EPD_W21_WriteDATA (0x17);

    EPD_W21_WriteCMD(0x04);         //上电 
    lcd_chkstatus();                //查看芯片状态  

    EPD_W21_WriteCMD(0X00);
#if (HEIGHT == 128)
    EPD_W21_WriteDATA(0x8f);     //选择分辨率128x296
#elif (HEIGHT == 152)
    EPD_W21_WriteDATA(0x0f);	 //LUT from OTP,128x296
    EPD_W21_WriteDATA(0x0d);     //VCOM to 0V fast
#endif

    EPD_W21_WriteCMD(0X50);
    EPD_W21_WriteDATA(0x77);

    EPD_W21_WriteCMD(0x61);            //像素设定
    EPD_W21_WriteDATA (HEIGHT);      
    EPD_W21_WriteDATA (0x01);          // 0x0128 = 296
    EPD_W21_WriteDATA (0x28); 
}

void LCD_Sleep(void)
{ 
    EPD_W21_WriteCMD(0x02);            //power off
    lcd_chkstatus();
    EPD_W21_WriteCMD(0x07);            //deep sleep
    EPD_W21_WriteDATA(0xA5);
    driver_delay_xms(100); 
    EPD_Off();     
}
