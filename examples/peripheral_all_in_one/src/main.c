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

#define PIN_BUZZER 8

void set_freq0(uint8_t channel_index, uint16_t freq)
{
    uint32_t pera = OSC_CLK_FREQ / freq;
    PWM_HaltCtrlEnable(channel_index, 1);
    PWM_Enable(channel_index, 0);
    if (freq > 0)
    {
        PWM_SetPeraThreshold(channel_index, pera);
        PWM_SetMultiDutyCycleCtrl(channel_index, 0);        // do not use multi duty cycles
        PWM_SetHighThreshold(channel_index, 0, pera >> 1);
        PWM_SetMode(channel_index, PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE);
        PWM_SetMask(channel_index, 0, 0);
        PWM_Enable(channel_index, 1); 
        PWM_HaltCtrlEnable(channel_index, 0);
    }
}

void set_freq(uint16_t freq)
{
    set_freq0(PIN_BUZZER >> 1, freq);
}

#include "../../peripheral_led/src/impl_led.c"

struct bme280_t bme280_data;

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);

    // for RGB
    setup_led();

    // buzzer
    PINCTRL_SetPadMux(PIN_BUZZER, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(PIN_BUZZER, 1);
    set_freq(0);
    
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

extern void app_timer_callback(void);

uint32_t timer2_isr(void *user_data)
{
    extern SemaphoreHandle_t sem_battery;
    BaseType_t xHigherPriorityTaskWoke = pdFALSE;
    TMR_IntClr(APB_TMR2);
    app_timer_callback();
    xSemaphoreGiveFromISR(sem_battery, &xHigherPriorityTaskWoke);
    return 0;
}

uint32_t timer_isr(void *user_data)
{
    static int cnt = 0;
    BaseType_t xHigherPriorityTaskWoke = pdFALSE;
    TMR_IntClr(APB_TMR1);
    if (rsc_notify_enable | rsc_indicate_enable)
        xSemaphoreGiveFromISR(sem_pedometer, &xHigherPriorityTaskWoke);
    cnt++;
    if (cnt >= ACC_SAMPLING_RATE)
    {
        timer2_isr(user_data);
        cnt = 0;
    }
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
