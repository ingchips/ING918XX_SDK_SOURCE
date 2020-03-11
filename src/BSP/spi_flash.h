
// test spi flash £¨AT45DBXX£©

#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

#include "ingsoc.h"


#define	DEF_FLASH_DEVICE_ERR				((s8)-1)
#define	DEF_FLASH_NONE_ERR					((s8)0)

 
#define SPI_FLASH_CS    GIO_L32_GPIO_4
#define SPI_FLASH_WP    GIO_L32_RF_CHN0_4
#define SPI_FLASH_RST   GIO_L32_RF_CLK

#ifdef C2NIM

#else
// set CS 0 or 1 
#define FLASH_Enable 	  gio_clr_bit(GIO_L32_DO , SPI_FLASH_CS); 
#define FLASH_Disable 	gio_set_bit(GIO_L32_DO , SPI_FLASH_CS); 
#endif


#define DummyByte  0xA5
#define DummyWord  0xA5A5A5A5

#define W8_2(addr)   (addr>>16)&0xFFU
#define W8_1(addr)   (addr>>8)&0xFFU
#define W8_0(addr)    addr&0xFFU


// FLASH Command
#define RdStatusRegCMD 0x05
#define RdIDCMD        0x90
#define WriteEnableCMD 0x06
#define WriteDisableCMD 0x04
#define PageWriteCMD  	0x02
#define ByteReadCMD  		0x03
// Datasheet 7.1
#define Buffer_1_WrCMD  0x84
#define Buffer_2_WrCMD  0x87
// Datasheet 7.2
#define Buffer_1_ToMemWithEraseCMD  0x83
#define Buffer_2_ToMemWithEraseCMD  0x86
// Datasheet 7.3
#define Buffer_1_ToMemWithoutEraseCMD  0x88
#define Buffer_2_ToMemWithoutEraseCMD  0x89
// Datasheet 7.4
#define PageEraseCMD  0xFF
// Datasheet 7.5
#define BlockEraseCMD 0xD8
// Datasheet 7.6
#define SectorEraseCMD 0x20
// Datasheet 7.6
#define PageEraseCMD 	0xFF
// Datasheet 7.7
#define ChipEraseCMD1 0xC7
#define ChipEraseCMD2 0x60

#define ChipEraseCMD3 0x80
#define ChipEraseCMD4 0x9A

#ifdef __cplusplus
extern "C" {
#endif

void test_spi_flash(void);
void spi_flash_init(void);
static uint8_t FLASH_BUSY(void);
void BytesWrite(uint32_t addr, uint8_t* databuff , uint8_t  length);
void spi_flash_send_byte(uint8_t data);
uint8_t spi_flash_rece_byte(void);
uint8_t PageWrite(uint32_t addr, uint8_t* databuff);
uint8_t ByteWrite(uint32_t addr, uint8_t* databuff,uint8_t writesize);
void ByteRead(uint32_t addr, uint8_t* databuff , uint32_t  length);
void FastRead(uint32_t addr, uint8_t* databuff);
void BufferToRead(uint32_t addr,  uint32_t length,  uint8_t* databuff);

void display_flash_status_reg(void);
uint8_t display_flash_id(void);

void PageErase(uint32_t addr);
uint8_t BlockErase(uint32_t addr);
uint8_t SectorErase(uint32_t addr);
uint8_t ChipErase(void);
void PageErase(uint32_t addr);
void io_sleep(void);
void Delay(void);

#ifdef __cplusplus
}
#endif

#endif // __TEST_SPI_FLASH_H__

