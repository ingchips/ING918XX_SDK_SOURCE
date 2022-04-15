#ifndef __SPI_H__
#define __SPI_H__

#include "ingsoc.h"

#define DummyByte       0xA5
#define DummyWord       0xA5A5A5A5

#define W8_2(addr)      ((addr >> 16) & 0xFFU)
#define W8_1(addr)      ((addr >> 8) & 0xFFU)
#define W8_0(addr)      (addr& 0xFFU)

void SPI_Init(SSP_TypeDef * SSP_Ptr);
 
void SPI_Write(uint8_t data);
 
void SPI_Delay(uint8_t t);
void SPI_Write_FIFO(uint16_t data);
void SPI_Init16bit(SSP_TypeDef * SSP_Ptr);

#endif

