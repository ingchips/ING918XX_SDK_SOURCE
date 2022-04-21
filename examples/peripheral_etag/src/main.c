#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "spi.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "Display_EPD_W21.h"
#include "screen.h"
#include <stdio.h>

#define PRINT_PORT    APB_UART0

uint32_t cb_putc(char *c, void *dummy)
{
    while (apUART_Check_TXFIFO_FULL(PRINT_PORT) == 1);
    UART_SendData(PRINT_PORT, (uint8_t)*c);
    return 0;
}

int fputc(int ch, FILE *f)
{
    // cb_putc((char *)&ch, NULL);
    return ch;
}

void config_uart(uint32_t freq, uint32_t baud)
{
    UART_sStateStruct config;

    config.word_length       = UART_WLEN_8_BITS;
    config.parity            = UART_PARITY_NOT_CHECK;
    config.fifo_enable       = 1;
    config.two_stop_bits     = 0;
    config.receive_en        = 1;
    config.transmit_en       = 1;
    config.UART_en           = 1;
    config.cts_en            = 0;
    config.rts_en            = 0;
    config.rxfifo_waterlevel = 1;
    config.txfifo_waterlevel = 1;
    config.ClockFrequency    = freq;
    config.BaudRate          = baud;

    apUART_Initialize(PRINT_PORT, &config, 0);
}

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_AHB_SPI0) | (1 << SYSCTRL_ClkGate_APB_GPIO));

    // for eTAG
    PINCTRL_SetPadMux(SPI_EPD_SDI, IO_SOURCE_SPI0_DO);
    PINCTRL_SetPadMux(SPI_EPD_SCK, IO_SOURCE_SPI0_CLK);
    PINCTRL_SetPadMux(SPI_EPD_DC, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_EPD_CS, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_EPD_DC, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_EPD_POWER, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_EPD_RST, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_EPD_BUSY, IO_SOURCE_GENERAL);
    GIO_SetDirection(SPI_EPD_CS, GIO_DIR_OUTPUT);      // set CS output
    GIO_SetDirection(SPI_EPD_DC, GIO_DIR_OUTPUT);      // set CS output
    GIO_SetDirection(SPI_EPD_POWER, GIO_DIR_OUTPUT);   // set output
    GIO_SetDirection(SPI_EPD_RST, GIO_DIR_OUTPUT);     // set RST output
    GIO_SetDirection(SPI_EPD_BUSY, GIO_DIR_INPUT);     // set Busy input
    
    PINCTRL_SelUartRxdIn(UART_PORT_1, 0x3f);
    PINCTRL_SelI2cSclIn(I2C_PORT_0, 0x3f);

    SPI_Init(AHB_SSP0);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_AHB_SPI0) | (1 << SYSCTRL_ClkGate_APB_GPIO0));

    // for eTAG
    PINCTRL_SetPadMux(SPI_EPD_SDI, IO_SOURCE_SPI0_DO);
    PINCTRL_SetPadMux(SPI_EPD_SCK, IO_SOURCE_SPI0_CLK);
    PINCTRL_SetPadMux(SPI_EPD_DC, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_EPD_CS, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_EPD_DC, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_EPD_POWER, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_EPD_RST, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_EPD_BUSY, IO_SOURCE_GENERAL);
    GIO_SetDirection(SPI_EPD_CS, GIO_DIR_OUTPUT);      // set CS output
    GIO_SetDirection(SPI_EPD_DC, GIO_DIR_OUTPUT);      // set CS output
    GIO_SetDirection(SPI_EPD_POWER, GIO_DIR_OUTPUT);   // set output
    GIO_SetDirection(SPI_EPD_RST, GIO_DIR_OUTPUT);     // set RST output
    GIO_SetDirection(SPI_EPD_BUSY, GIO_DIR_INPUT);     // set Busy input

    SPI_Init(AHB_SSP0);
#else
    #error unknown or unsupported chip family
#endif
}

// message queue
#define QUEUE_LENGTH    1
#define ITEM_SIZE       sizeof(queue_msg_t) 
static StaticQueue_t xStaticQueue; 
uint8_t ucQueueStorageArea[QUEUE_LENGTH * ITEM_SIZE];
QueueHandle_t xQueue;
static SemaphoreHandle_t sem_display;

void driver_delay_xms(uint32_t ms)
{ 
	xSemaphoreTake(sem_display, pdMS_TO_TICKS(ms));
}

void driver_delay_us(unsigned int xus)
{
	for(;xus>1;xus--);
}

static void display_task(void *pdata)
{
    queue_msg_t msg;
    LCD_Init();
    pic_display(NULL,NULL);
    driver_delay_xms(15000);
    LCD_Sleep();
    for (;;)
    {
        if (xQueueReceive(xQueue, &msg, portMAX_DELAY) != pdPASS)
            continue;
        
        LCD_Init();
        pic_display(msg.black_white, msg.red_white);
        driver_delay_xms(15000);
        LCD_Sleep();
    }
}

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    setup_peripherals();
    
    // setup putc handle
    // platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    xTaskCreate(display_task,
           "b",
           150,
           NULL,
           (configMAX_PRIORITIES - 1),
           NULL);

    sem_display = xSemaphoreCreateBinary();
    xQueue = xQueueCreateStatic(QUEUE_LENGTH,
                                ITEM_SIZE,
                                ucQueueStorageArea,
                                &xStaticQueue);
    return 0;
}
