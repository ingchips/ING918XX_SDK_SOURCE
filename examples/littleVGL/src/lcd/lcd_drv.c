#include <string.h>
#include "peripheral_gpio.h"
#include "spi.h"
#include "lcd_config.h"

void delay_ms(uint16_t n_ms)
{
   uint16_t i,j;
   for(i=0;i<n_ms;i++)
   {
       for(j=0;j<48000;j++)
       {
           __NOP();
       }
   }
}

#ifdef LCDMODULE_LCD128160
void LCD_WriteCMD(unsigned char command)
{
	SPI_Delay(1);
	LCD_CS_0;                   
	LCD_DC_0;// command write
	SPI_Write(command);
	LCD_CS_1;
}


void LCD_WriteDATA(unsigned char command)
{
	LCD_CS_0;//	
	LCD_DC_1;// data write
	SPI_Write(command);
}
void LCD_Write_Data(uint32_t dat16)
{
  LCD_WriteDATA(dat16>>8);
  LCD_WriteDATA(dat16);
}

//========================================================
void DISP_WINDOWS(void)
{
		 SPI_Init(AHB_SSP0);
         LCD_WriteCMD(0x2A);
         LCD_WriteDATA(0x00);
         LCD_WriteDATA(0x02);
         LCD_WriteDATA(0x00);
         LCD_WriteDATA(0x81);

         LCD_WriteCMD(0x2B);
         LCD_WriteDATA(0x00);
         LCD_WriteDATA(0x01);
         LCD_WriteDATA(0x00);
         LCD_WriteDATA(0xa0);
         LCD_WriteCMD(0x2C);
}

//========================================================
void lcd_set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 )
{
		 uint8_t xStarL,xStarH, xEndL,xEndH,yStarL,yStarH, yEndL,yEndH;
		 xStarL = (uint8_t)(x1&0xff);
		 xEndL = (uint8_t) (x2&0xff);
		 yStarL =(uint8_t) (y1&0xff);
		 yEndL  = (uint8_t)(y2&0xff);
		 
		 SPI_Init(AHB_SSP0);
         LCD_WriteCMD(0x2A);
         LCD_WriteDATA(0x00);
         LCD_WriteDATA(0x02+xStarL);
         LCD_WriteDATA(0x00);
         LCD_WriteDATA(0x02+xEndL);

         LCD_WriteCMD(0x2B);
         LCD_WriteDATA(0x00);
         LCD_WriteDATA(0x01+yStarL);
         LCD_WriteDATA(0x00);
         LCD_WriteDATA(0x01+yEndL);
         LCD_WriteCMD(0x2C);
}


void lcd_draw(uint16_t width, uint16_t height, void *data)
{
	uint32_t i;
	lv_color_t *pData = (lv_color_t *)data;
	SPI_Init16bit(AHB_SSP0);
	
	LCD_CS_0;                   
	LCD_DC_1;   
	apSSP_DeviceEnable(AHB_SSP0);
    for(i=width*height;i>0;i--)
    {
	    SPI_Write_FIFO(pData->full);
		pData++;
	}  
	apSSP_DeviceDisable(AHB_SSP0);

	LCD_CS_1; 				  
	
}


void lcd_clear()
{
	int i,j;
 	DISP_WINDOWS();	
	SPI_Init16bit(AHB_SSP0);
	
	LCD_CS_0;                   
	LCD_DC_1;   
	apSSP_DeviceEnable(AHB_SSP0);
	for (i=LCD_HEIGHT;i>0;i--)
	for (j=LCD_WIDTH; j>0;j--)
	SPI_Write_FIFO(0xf800);
	
	apSSP_DeviceDisable(AHB_SSP0);	
	LCD_CS_1; 				  

}


void  ST7735_LAIBAO177_INITIAL ()
 
{ 
	//------------------------------------ST7735R Reset Sequence-----------------------------------------// 
	LCD_RST_1; 
	delay_ms(1);//Delay 1ms 
	LCD_RST_0;
	delay_ms(1);//Delay 1ms 
	LCD_RST_1; 
	delay_ms(1);                                                              //Delay 120ms 
	//--------------------------------End ST7735R Reset Sequence --------------------------------------// 
	 
	//--------------------------------End ST7735S Reset Sequence --------------------------------------// 
	LCD_WriteCMD(0x11); //Sleep out 
	delay_ms(1);              //Delay 120ms 
	//------------------------------------ST7735S Frame Rate-----------------------------------------// 
	LCD_WriteCMD(0xB1); 
	LCD_WriteDATA(0x05); 
	LCD_WriteDATA(0x3C); 
	LCD_WriteDATA(0x3C); 
	LCD_WriteCMD(0xB2); 
	LCD_WriteDATA(0x05); 
	LCD_WriteDATA(0x3C); 
	LCD_WriteDATA(0x3C); 
	LCD_WriteCMD(0xB3); 
	LCD_WriteDATA(0x05); 
	LCD_WriteDATA(0x3C); 
	LCD_WriteDATA(0x3C); 
	LCD_WriteDATA(0x05); 
	LCD_WriteDATA(0x3C); 
	LCD_WriteDATA(0x3C); 
	//------------------------------------End ST7735S Frame Rate---------------------------------// 
	LCD_WriteCMD(0xB4); //Dot inversion 
	LCD_WriteDATA(0x03); 
	//------------------------------------ST7735S Power Sequence---------------------------------// 
	LCD_WriteCMD(0xC0); 
	LCD_WriteDATA(0x28); 
	LCD_WriteDATA(0x08); 
	LCD_WriteDATA(0x04); 
	LCD_WriteCMD(0xC1); 
	LCD_WriteDATA(0XC0); 
	LCD_WriteCMD(0xC2); 
	LCD_WriteDATA(0x0D); 
	LCD_WriteDATA(0x00); 
	LCD_WriteCMD(0xC3); 
	LCD_WriteDATA(0x8D); 
	LCD_WriteDATA(0x2A); 
	LCD_WriteCMD(0xC4); 
	LCD_WriteDATA(0x8D); 
	LCD_WriteDATA(0xEE); 
	//---------------------------------End ST7735S Power Sequence-------------------------------------// 
	LCD_WriteCMD(0xC5); //VCOM 
	LCD_WriteDATA(0x1A); 
	LCD_WriteCMD(0x36); //MX, MY, RGB mode 
	LCD_WriteDATA(0xC0); 
	//------------------------------------ST7735S Gamma Sequence---------------------------------// 
	LCD_WriteCMD(0xE0); 
	LCD_WriteDATA(0x04); 
	LCD_WriteDATA(0x22); 
	LCD_WriteDATA(0x07); 
	LCD_WriteDATA(0x0A); 
	LCD_WriteDATA(0x2E); 
	LCD_WriteDATA(0x30); 
	LCD_WriteDATA(0x25); 
	LCD_WriteDATA(0x2A); 
	LCD_WriteDATA(0x28); 
	LCD_WriteDATA(0x26); 
	LCD_WriteDATA(0x2E); 
	LCD_WriteDATA(0x3A); 
	LCD_WriteDATA(0x00); 
	LCD_WriteDATA(0x01); 
	LCD_WriteDATA(0x03); 
	LCD_WriteDATA(0x13); 
	LCD_WriteCMD(0xE1); 
	LCD_WriteDATA(0x04); 
	LCD_WriteDATA(0x16); 
	LCD_WriteDATA(0x06); 
	LCD_WriteDATA(0x0D); 
	LCD_WriteDATA(0x2D); 
	LCD_WriteDATA(0x26); 
	LCD_WriteDATA(0x23); 
	LCD_WriteDATA(0x27); 
	LCD_WriteDATA(0x27); 
	LCD_WriteDATA(0x25); 
	LCD_WriteDATA(0x2D); 
	LCD_WriteDATA(0x3B); 
	LCD_WriteDATA(0x00); 
	LCD_WriteDATA(0x01); 
	LCD_WriteDATA(0x04); 
	LCD_WriteDATA(0x13); 
	//------------------------------------End ST7735S Gamma Sequence-----------------------------// 
	LCD_WriteCMD(0x3A); //65k mode 
	LCD_WriteDATA(0x05); 
	LCD_WriteCMD(0x29); //Display on 
	lcd_clear();
 	LCD_On();
	
} 


void lcd_initial(void)
{
	ST7735_LAIBAO177_INITIAL();

}

#elif defined(LCDMODULE_LCD240320)
void LCD_WriteCMD(unsigned char command)
{
	SPI_Delay(1);
	LCD_CS_0;                   
	LCD_DC_0;		// command write
	SPI_Write(command);
	LCD_CS_1;
}


void LCD_WriteDATA(unsigned char command)
{
	LCD_CS_0;
	
	LCD_DC_1;		// command write
	SPI_Write(command);
	
    LCD_DC_1;
}

void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_WriteCMD(0x2a);
	LCD_WriteDATA(x1 >> 8);
	LCD_WriteDATA(x1);
	LCD_WriteDATA(x2 >> 8);
	LCD_WriteDATA(x2);
	LCD_WriteCMD(0x2b);
	LCD_WriteDATA(y1 >> 8);
	LCD_WriteDATA(y1);
	LCD_WriteDATA(y2 >> 8);
	LCD_WriteDATA(y2);
	LCD_WriteCMD(0x2C);
}


void LCD_WriteRAM_Prepare(void)
{
	LCD_WriteCMD(0x2C);
}	 



//========================================================
void lcd_set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 )
{
	// printf("%d\r\n",x1);
	 SPI_Init(AHB_SSP0);
	 LCD_WriteCMD(0x2A); 
	 LCD_WriteDATA(x1>>8);
	 LCD_WriteDATA(0x00FF&x1);		 
	 LCD_WriteDATA(x2>>8);
	 LCD_WriteDATA(0x00FF&x2);
	 
	 LCD_WriteCMD(0x2B); 
	 LCD_WriteDATA(y1>>8);
	 LCD_WriteDATA(0x00FF&y1);		 
	 LCD_WriteDATA(x2>>8);
	 LCD_WriteDATA(0x00FF&x2); 
     LCD_WriteCMD(0x2C);
}

void LCD_Clear(uint16_t Color)
{
	uint32_t index=0;      
	lcd_set_window(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);	
	LCD_CS_0;	
	LCD_DC_1;//    
	for(index=0;index<LCD_WIDTH*LCD_HEIGHT;index++)
	{
	 SPI_Write(Color>>8);
	 SPI_Write(Color);
	}
	LCD_CS_1;	
		
}   		  

void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{	
	LCD_WriteCMD(LCD_Reg);  
	LCD_WriteDATA(LCD_RegValue);	    		 
}	   


//========================================================
void DISP_WINDOWS(void)
{
	 SPI_Init(AHB_SSP0);
     LCD_WriteCMD(0x2A);
     LCD_WriteDATA(0x00);
     LCD_WriteDATA(0x00);
     LCD_WriteDATA(0x00);
     LCD_WriteDATA(0xf0);

     LCD_WriteCMD(0x2B);
     LCD_WriteDATA(0x00);
     LCD_WriteDATA(0x00);
     LCD_WriteDATA(0x01);
     LCD_WriteDATA(0x40);
     LCD_WriteCMD(0x2C);
}



void lcd_draw(uint16_t width, uint16_t height, void *data)
{
	uint32_t i;
	
	lv_color_t *pData = (lv_color_t *)data;
	SPI_Init16bit(AHB_SSP0);
	
	LCD_CS_0;                   
	LCD_DC_1;   
	apSSP_DeviceEnable(AHB_SSP0);
    for(i=width*height;i>0;i--)
    {
	    SPI_Write_FIFO(pData->full);
		pData++;
	}  
	apSSP_DeviceDisable(AHB_SSP0);

	LCD_CS_1; 				  
	
}

void lcd_clear()
{
	int i,j;
	DISP_WINDOWS(); 
	
	SPI_Init16bit(AHB_SSP0);
	
	LCD_CS_0;					
	LCD_DC_1;	
	apSSP_DeviceEnable(AHB_SSP0);
	
	for (i=LCD_HEIGHT;i>0;i--)
		for (j=LCD_WIDTH; j>0;j--)
			SPI_Write_FIFO(0xf800);
	
	apSSP_DeviceDisable(AHB_SSP0);	
	LCD_CS_1;				  	

}

void ST7789_Init(void)
{
	int i,j;

  	LCD_RST_0;
	delay_ms(1);
	LCD_RST_1;
	delay_ms(1);

	LCD_WriteCMD(0x01);//Software Reset
	delay_ms(1);

    LCD_WriteCMD(0x11); 			//Sleep Out
	delay_ms(1);               //DELAY120ms
	 	  //-----------------------ST7789V Frame rate setting-----------------//
//************************************************
	LCD_WriteCMD(0x3A);        //65k mode
	LCD_WriteDATA(0x05);
	LCD_WriteCMD(0xC5); 		//VCOM
	LCD_WriteDATA(0x1A);
	LCD_WriteCMD(0x36);                 // ?：?????：???∴??：＜：?：：??
	LCD_WriteDATA(0x00);
	//-------------ST7789V Frame rate setting-----------//
	LCD_WriteCMD(0xb2);		//Porch Setting
	LCD_WriteDATA(0x05);
	LCD_WriteDATA(0x05);
	LCD_WriteDATA(0x00);
	LCD_WriteDATA(0x33);
	LCD_WriteDATA(0x33);

	LCD_WriteCMD(0xb7);			//Gate Control
	LCD_WriteDATA(0x05);			//12.2v   -10.43v
	//--------------ST7789V Power setting---------------//
	LCD_WriteCMD(0xBB);//VCOM
	LCD_WriteDATA(0x3F);

	LCD_WriteCMD(0xC0); //Power control
	LCD_WriteDATA(0x2c);

	LCD_WriteCMD(0xC2);		//VDV and VRH Command Enable
	LCD_WriteDATA(0x01);

	LCD_WriteCMD(0xC3);			//VRH Set
	LCD_WriteDATA(0x0F);		//4.3+( vcom+vcom offset+vdv)

	LCD_WriteCMD(0xC4);			//VDV Set
	LCD_WriteDATA(0x20);				//0v

	LCD_WriteCMD(0xC6);				//Frame Rate Control in Normal Mode
	LCD_WriteDATA(0X01);			//111Hz

	LCD_WriteCMD(0xd0);				//Power Control 1
	LCD_WriteDATA(0xa4);
	LCD_WriteDATA(0xa1);

	LCD_WriteCMD(0xE8);				//Power Control 1
	LCD_WriteDATA(0x03);

	LCD_WriteCMD(0xE9);				//Equalize time control
	LCD_WriteDATA(0x09);
	LCD_WriteDATA(0x09);
	LCD_WriteDATA(0x08);
	//---------------ST7789V gamma setting-------------//
	LCD_WriteCMD(0xE0); //Set Gamma
	LCD_WriteDATA(0xD0);
	LCD_WriteDATA(0x05);
	LCD_WriteDATA(0x09);
	LCD_WriteDATA(0x09);
	LCD_WriteDATA(0x08);
	LCD_WriteDATA(0x14);
	LCD_WriteDATA(0x28);
	LCD_WriteDATA(0x33);
	LCD_WriteDATA(0x3F);
	LCD_WriteDATA(0x07);
	LCD_WriteDATA(0x13);
	LCD_WriteDATA(0x14);
	LCD_WriteDATA(0x28);
	LCD_WriteDATA(0x30);

	LCD_WriteCMD(0XE1); //Set Gamma
	LCD_WriteDATA(0xD0);
	LCD_WriteDATA(0x05);
	LCD_WriteDATA(0x09);
	LCD_WriteDATA(0x09);
	LCD_WriteDATA(0x08);
	LCD_WriteDATA(0x03);
	LCD_WriteDATA(0x24);
	LCD_WriteDATA(0x32);
	LCD_WriteDATA(0x32);
	LCD_WriteDATA(0x3B);
	LCD_WriteDATA(0x14);
	LCD_WriteDATA(0x13);
	LCD_WriteDATA(0x28);
	LCD_WriteDATA(0x2F);

	LCD_WriteCMD(0x20); 	
	delay_ms(1); 
	LCD_WriteCMD(0x29);     
	
	lcd_clear();
	LCD_On();
  }



void lcd_initial(void)
{
	ST7789_Init();
}

#else

#endif

