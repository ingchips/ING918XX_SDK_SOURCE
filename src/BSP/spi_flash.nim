##  test spi flash £¨AT45DBXX£©

import
  cm32gpm3

const
  DEF_FLASH_DEVICE_ERR* = ((s8) - 1)
  DEF_FLASH_NONE_ERR* = (cast[s8](0))
  SPI_FLASH_CS* = GIO_L32_GPIO_4
  SPI_FLASH_WP* = GIO_L32_RF_CHN0_4
  SPI_FLASH_RST* = GIO_L32_RF_CLK

const
  DummyByte* = 0x000000A5
  DummyWord* = 0xA5A5A5A5

template W8_2*(`addr`: untyped): untyped =
  (`addr` shr 16) and 0x000000FF

template W8_1*(`addr`: untyped): untyped =
  (`addr` shr 8) and 0x000000FF

template W8_0*(`addr`: untyped): untyped =
  `addr` and 0x000000FF

##  FLASH Command

const
  RdStatusRegCMD* = 0x00000005
  RdIDCMD* = 0x00000090
  WriteEnableCMD* = 0x00000006
  WriteDisableCMD* = 0x00000004
  PageWriteCMD* = 0x00000002
  ByteReadCMD* = 0x00000003

##  Datasheet 7.1

const
  Buffer_1_WrCMD* = 0x00000084
  Buffer_2_WrCMD* = 0x00000087

##  Datasheet 7.2

const
  Buffer_1_ToMemWithEraseCMD* = 0x00000083
  Buffer_2_ToMemWithEraseCMD* = 0x00000086

##  Datasheet 7.3

const
  Buffer_1_ToMemWithoutEraseCMD* = 0x00000088
  Buffer_2_ToMemWithoutEraseCMD* = 0x00000089

##  Datasheet 7.4

const
  PageEraseCMD* = 0x000000FF

##  Datasheet 7.5

const
  BlockEraseCMD* = 0x000000D8

##  Datasheet 7.6

const
  SectorEraseCMD* = 0x00000020

##  Datasheet 7.6

const
  PageEraseCMD* = 0x000000FF

##  Datasheet 7.7

const
  ChipEraseCMD1* = 0x000000C7
  ChipEraseCMD2* = 0x00000060
  ChipEraseCMD3* = 0x00000080
  ChipEraseCMD4* = 0x0000009A

proc test_spi_flash*() {.importc: "test_spi_flash", header: "spi_flash.h".}
proc spi_flash_init*() {.importc: "spi_flash_init", header: "spi_flash.h".}
proc FLASH_BUSY*(): uint8 {.importc: "FLASH_BUSY", header: "spi_flash.h".}
proc BytesWrite*(`addr`: uint32; databuff: ptr uint8; length: uint8) {.
    importc: "BytesWrite", header: "spi_flash.h".}
proc spi_flash_send_byte*(data: uint8) {.importc: "spi_flash_send_byte",
                                      header: "spi_flash.h".}
proc spi_flash_rece_byte*(): uint8 {.importc: "spi_flash_rece_byte",
                                  header: "spi_flash.h".}
proc PageWrite*(`addr`: uint32; databuff: ptr uint8): uint8 {.importc: "PageWrite",
    header: "spi_flash.h".}
proc ByteWrite*(`addr`: uint32; databuff: ptr uint8; writesize: uint8): uint8 {.
    importc: "ByteWrite", header: "spi_flash.h".}
proc ByteRead*(`addr`: uint32; databuff: ptr uint8; length: uint32) {.
    importc: "ByteRead", header: "spi_flash.h".}
proc FastRead*(`addr`: uint32; databuff: ptr uint8) {.importc: "FastRead",
    header: "spi_flash.h".}
proc BufferToRead*(`addr`: uint32; length: uint32; databuff: ptr uint8) {.
    importc: "BufferToRead", header: "spi_flash.h".}
proc display_flash_status_reg*() {.importc: "display_flash_status_reg",
                                 header: "spi_flash.h".}
proc display_flash_id*(): uint8 {.importc: "display_flash_id", header: "spi_flash.h".}
proc PageErase*(`addr`: uint32) {.importc: "PageErase", header: "spi_flash.h".}
proc BlockErase*(`addr`: uint32): uint8 {.importc: "BlockErase", header: "spi_flash.h".}
proc SectorErase*(`addr`: uint32): uint8 {.importc: "SectorErase",
                                       header: "spi_flash.h".}
proc ChipErase*(): uint8 {.importc: "ChipErase", header: "spi_flash.h".}
proc PageErase*(`addr`: uint32) {.importc: "PageErase", header: "spi_flash.h".}
proc io_sleep*() {.importc: "io_sleep", header: "spi_flash.h".}
proc Delay*() {.importc: "Delay", header: "spi_flash.h".}