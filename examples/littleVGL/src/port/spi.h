#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>

#define SPI_PORT        AHB_QSPI
#define MAX_SPI_TRANSFER_COUNT 512
#define DMA_EN                  0


void SPI_Init(void);
void SPI_Init16bit(void);

void SPI_Write(uint8_t data);
 
void SPI_Write_FIFO_16(const uint16_t *data, int len);
void SPI_Write_FIFO_8(const uint8_t *data, int len);

void SPI_Fill_FIFO_16(uint16_t data, int len);
void SPI_Fill_FIFO_8(uint8_t data, int len);

void SPI_WaitTxDone(void);

void SPI_Delay(uint32_t t);

#endif

