#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "spi.h"
#include "iic.h"
#include <stdio.h>
#include "bme280.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "Display_EPD_W21.h"
#include "screen.h"

#include "step_calc.h"
#include "iic.h"
#include "bma2x2.h"

#define PRINT_PORT    APB_UART0

uint32_t cb_putc(char *c, void *dummy)
{
    while (apUART_Check_TXFIFO_FULL(PRINT_PORT) == 1);
    UART_SendData(PRINT_PORT, (uint8_t)*c);
    return 0;
}

int fputc(int ch, FILE *f)
{
    cb_putc((char *)&ch, NULL);
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

#ifdef BOARD_V2

#define PIN_SDI   GIO_GPIO_0

// CPU clok: PLL_CLK_FREQ  48000000
// 1 cycle = 21ns
// 48 cycles per us
// Tcycle = 2us --> ~100 cycles
void delay(int cycles)
{
    int i;
    for (i = 0; i < cycles; i++)
    {
        __nop();
    }
}

void tlc59731_write(uint32_t value)
{
    int8_t i;

    #define pulse()                     \
        { GIO_WriteValue(PIN_SDI, 1);   \
        delay(1);                       \
        GIO_WriteValue(PIN_SDI, 0); } while (0)

    for( i = 0; i < 32; i++ )
    {
        uint32_t bit = value & ( 0x80000000 >> i );
        pulse();
        
        if (bit)
        {
            delay(10);
            pulse();
            delay(78);
        }
        else
            delay(90);
    }
    delay(100 * 8);
}

void set_led_color(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t cmd = (0x3a << 24) | (b << 16) | (r << 8) | g;
    tlc59731_write(cmd);
}

void setup_led(void)
{
    PINCTRL_SetPadMux(PIN_SDI, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(PIN_SDI, 0);
    GIO_SetDirection(PIN_SDI, GIO_DIR_OUTPUT);
    GIO_WriteValue(PIN_SDI, 0);

    set_led_color(50, 50, 50);
}

#else

#define CHANNEL_RED     4
#define CHANNEL_GREEN   0
#define CHANNEL_BLUE    6

#define PERA_THRESHOLD (OSC_CLK_FREQ / 1000)

void set_led_color(uint8_t r, uint8_t g, uint8_t b)
{
#define TO_PERCENT(v) (((uint32_t)(v) * 100) >> 8)

    PWM_SetHighThreshold(CHANNEL_RED   >> 1, 0, PERA_THRESHOLD / 100 * TO_PERCENT(r));
    PWM_SetHighThreshold(CHANNEL_GREEN >> 1, 0, PERA_THRESHOLD / 100 * TO_PERCENT(g >> 1));  // GREEN & BLUE led seems too bright
    PWM_SetHighThreshold(CHANNEL_BLUE  >> 1, 0, PERA_THRESHOLD / 100 * TO_PERCENT(b >> 1));
}

static void setup_channel(uint8_t channel_index)
{
    PWM_HaltCtrlEnable(channel_index, 1);
    PWM_Enable(channel_index, 0);
    PWM_SetPeraThreshold(channel_index, PERA_THRESHOLD);
    PWM_SetMultiDutyCycleCtrl(channel_index, 0);        // do not use multi duty cycles
    PWM_SetHighThreshold(channel_index, 0, PERA_THRESHOLD / 2);
    PWM_SetMode(channel_index, PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE);
    PWM_SetMask(channel_index, 0, 0);
    PWM_Enable(channel_index, 1); 
    PWM_HaltCtrlEnable(channel_index, 0);
}

void setup_led(void)
{
    PINCTRL_SetPadMux(CHANNEL_RED, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(CHANNEL_RED, 1);
    PINCTRL_SetPadMux(CHANNEL_GREEN, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(CHANNEL_GREEN, 1);
    PINCTRL_SetPadMux(CHANNEL_BLUE, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(CHANNEL_BLUE, 1);
    setup_channel(CHANNEL_RED   >> 1);
    setup_channel(CHANNEL_GREEN >> 1);
    setup_channel(CHANNEL_BLUE  >> 1);
    set_led_color(50, 50, 50);
}

#endif

struct bme280_t bme280_data;

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);

    // for RGB
    setup_led();

    // buzzer
    PINCTRL_SetPadMux(8, IO_SOURCE_GENERAL);
    GIO_WriteValue((GIO_Index_t)8, 0);
    
    // for eTAG
#ifdef HAS_ETAG    
    PINCTRL_SetPadMux(SPI_EPD_SDI, IO_SOURCE_SPI0_DO);
    PINCTRL_SetPadMux(SPI_EPD_SCK, IO_SOURCE_SPI0_CLK);
    PINCTRL_SetPadMux(SPI_EPD_DC, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_EPD_CS, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_EPD_DC, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_EPD_POWER, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_EPD_RST, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_EPD_BUSY, IO_SOURCE_GENERAL);
    gio_set_bit(GIO_L32_OEB, SPI_EPD_CS);      // set CS output
    gio_set_bit(GIO_L32_OEB, SPI_EPD_DC);      // set CS output
    gio_set_bit(GIO_L32_OEB, SPI_EPD_POWER);   // set output
    gio_set_bit(GIO_L32_OEB, SPI_EPD_RST);     // set RST output
    gio_clr_bit(GIO_L32_OEB, SPI_EPD_BUSY);    // set Busy input
    
    SPI_Init(AHB_SSP0);
#endif

#ifndef SIMULATION
    PINCTRL_SetPadMux(7, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(14, IO_SOURCE_I2C0_SCL_O);
    PINCTRL_SetPadMux(15, IO_SOURCE_I2C0_SDO);
    PINCTRL_SelI2cSclIn(I2C_PORT_0, 14);
    
    printf("i2c_init\n");
    i2c_init(I2C_PORT_0);
    printf("bme init...");
    if (bme280_init(&bme280_data)==0)
        printf("failed\n");
    else
        printf("OK\n");
#endif

    // timer 0 can be used as watchdog, so we use timer 1.
    // setup timer 1: 50Hz
	TMR_SetCMP(APB_TMR1, TMR_CLK_FREQ / ACC_SAMPLING_RATE);
	TMR_SetOpMode(APB_TMR1, TMR_CTL_OP_MODE_WRAPPING);
	TMR_IntEnable(APB_TMR1);
    TMR_Reload(APB_TMR1);
	TMR_Enable(APB_TMR1);
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

/*	Brief : The delay routine
 *	\param : delay in ms
*/
void BMA2x2_delay_msek(u32 msek)
{
	driver_delay_xms(msek);
}

static void display_task(void *pdata)
{
    queue_msg_t msg;
#ifdef HAS_ETAG
    LCD_Init();
    pic_display(NULL,NULL);
    LCD_Sleep();
    for (;;)
    {
        if (xQueueReceive(xQueue, &msg, portMAX_DELAY) != pdPASS)
            continue;
        
        LCD_Init();
        pic_display(msg.black_white, msg.red_white);
        LCD_Sleep();
    }
#else
    for (;;)
    {
        xQueueReceive(xQueue, &msg, portMAX_DELAY);
    }
#endif
}

static SemaphoreHandle_t sem_pedometer = NULL;
void accelarator_sample_sim(void);

static void pedometer_task(void *pdata)
{
    pedometer_init();
    for (;;)
    {
        BaseType_t r = xSemaphoreTake(sem_pedometer,  portMAX_DELAY);
        if (pdTRUE == r)
            accelarator_sample_sim();
    }
}

extern uint8_t rsc_notify_enable;
extern uint8_t rsc_indicate_enable;

uint32_t timer_isr(void *user_data)
{
    BaseType_t xHigherPriorityTaskWoke = pdFALSE;
    TMR_IntClr(APB_TMR1);
    if (rsc_notify_enable | rsc_indicate_enable)
        xSemaphoreGiveFromISR(sem_pedometer, &xHigherPriorityTaskWoke);
    return 0;
}

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    sem_pedometer = xSemaphoreCreateBinary();

    // setup putc handle
    // platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);
    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER1, timer_isr, NULL);
    setup_peripherals();

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
    xTaskCreate(pedometer_task,
               "b",
               150,
               NULL,
               (configMAX_PRIORITIES - 1),
               NULL);
    return 0;
}



