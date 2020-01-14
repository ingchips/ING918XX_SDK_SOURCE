#ifndef __SPI_H__
#define __SPI_H__

#include "cm32gpm3.h"

#ifdef PROD_VER

#define SPI_EPD_BUSY   	GIO_GPIO_6
#define SPI_EPD_RST   	GIO_GPIO_7
#define SPI_EPD_DC   	GIO_GPIO_8
#define SPI_EPD_CS    	GIO_GPIO_5
#define SPI_EPD_SCK    	GIO_GPIO_0
#define SPI_EPD_SDI    	GIO_GPIO_1
#define SPI_EPD_POWER  	GIO_GPIO_9

#else

#define SPI_EPD_BUSY   	GIO_GPIO_15
#define SPI_EPD_RST   	GIO_GPIO_14
#define SPI_EPD_DC   	GIO_GPIO_13
#define SPI_EPD_CS    	GIO_GPIO_12
#define SPI_EPD_SCK    	GIO_GPIO_11
#define SPI_EPD_SDI    	GIO_GPIO_10
#define SPI_EPD_POWER  	GIO_GPIO_9

#endif

#define EPD_W21_CS_0		GIO_WriteValue(SPI_EPD_CS, 0)
#define EPD_W21_CS_1		GIO_WriteValue(SPI_EPD_CS, 1)

#define EPD_W21_DC_0		GIO_WriteValue(SPI_EPD_DC, 0) 
#define EPD_W21_DC_1		GIO_WriteValue(SPI_EPD_DC, 1)  

#define EPD_W21_RST_0		GIO_WriteValue(SPI_EPD_RST, 0) 
#define EPD_W21_RST_1		GIO_WriteValue(SPI_EPD_RST, 1)
  
// Powers control
#define  EPD_On() 		 	GIO_WriteValue(SPI_EPD_POWER, 0);
#define  EPD_Off()  	 	GIO_WriteValue(SPI_EPD_POWER, 1); 
 
#define isEPD_W21_BUSY 	    GIO_ReadValue(SPI_EPD_BUSY) 	// for solomen solutions
 
void SPI_Init(SSP_TypeDef * SSP_Ptr);
 
void SPI_Write(uint8_t data);
 
void SPI_Delay(uint8_t t);

#endif

