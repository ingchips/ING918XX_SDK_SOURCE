# Peripheral eTAG

This example demonstrates an eTAG or ESL (electronic shelf label), and its content can be updated
by _ING BLE_.

## Hardware Setup

An triple color (white/black/red, or white/black/yellow) E-ink display (EPD) driven by UC8151D.
Supported resolutions are 296x128 (default) and 296x152.

| EPD              | ING918xx Dev-Board (Default) |
---                | ---                          |
|SPI_EPD_BUSY      | GIO_GPIO_6                   |
|SPI_EPD_RST       | GIO_GPIO_7                   |
|SPI_EPD_DC        | GIO_GPIO_8                   |
|SPI_EPD_CS        | GIO_GPIO_5                   |
|SPI_EPD_SCK       | GIO_GPIO_0                   |
|SPI_EPD_SDI       | GIO_GPIO_1                   |
|SPI_EPD_POWER     | GIO_GPIO_9                   |

Use compiler option `HEIGHT=128` or `HEIGHT=152` to set the actual height resolution.

## Test

Download this example to a Dev-Board, connect with _ING BLE_. Select EPD type, take a photo, then
upload it to the EPD.

## Design Details

* _ING BLE_ reads the resolution of the EPD from _eTag Resolution_ characteristics, therefore it
    can support other resolutions;

* Image is saved in `eink_image_t`. Each pixel uses two bits: one controls the position of the
    black capsule, the other one controls the position of the red (or yellow) capsule.