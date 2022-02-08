#define OPTIONAL_RF_CLK
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "step_calc.h"
#include "iic.h"
#include "bma2x2.h"

#include "bh1790/bh1790.h"

#include "sig_services.h"

#include "spi.h"
#include <stdio.h>
#include "disp_task.h"

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

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_GPIO)
                              | (1 << SYSCTRL_ClkGate_APB_PinCtrl)
                              | (1 << SYSCTRL_ClkGate_APB_PWM)
                              | (1 << SYSCTRL_ClkGate_APB_I2C0)
                              | (1 << SYSCTRL_ClkGate_APB_TMR1)
                              | (1 << SYSCTRL_ClkGate_APB_TMR2));

    PINCTRL_DisableAllInputs();

    // setup timer 1: 50Hz
    TMR_SetCMP(APB_TMR1, TMR_CLK_FREQ / ACC_SAMPLING_RATE);
    TMR_SetOpMode(APB_TMR1, TMR_CTL_OP_MODE_WRAPPING);
    TMR_Reload(APB_TMR1);
    TMR_IntEnable(APB_TMR1);

    // setup timer 2: SAMPLING_CNT_32HZ
    TMR_SetCMP(APB_TMR2, TMR_CLK_FREQ / SAMPLING_CNT_32HZ);
    TMR_SetOpMode(APB_TMR2, TMR_CTL_OP_MODE_WRAPPING);
    TMR_Reload(APB_TMR2);
    TMR_IntEnable(APB_TMR2);

    // IO setup
    PINCTRL_SetPadMux(SPI_LCD_POWER, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_LCD_DC, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_LCD_RST, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(SPI_LCD_CS, IO_SOURCE_GENERAL);

    GIO_SetDirection(SPI_LCD_POWER, GIO_DIR_OUTPUT);
    GIO_SetDirection(SPI_LCD_RST, GIO_DIR_OUTPUT);
    GIO_SetDirection(SPI_LCD_CS, GIO_DIR_OUTPUT);
    GIO_SetDirection(SPI_LCD_DC, GIO_DIR_OUTPUT);

    PINCTRL_SetPadMux(10, IO_SOURCE_I2C0_SCL_O);
    PINCTRL_SetPadMux(11, IO_SOURCE_I2C0_SDO);
    PINCTRL_SelI2cSclIn(I2C_PORT_0, 10);

    PINCTRL_SetPadMux(SPI_LCD_MOSI, IO_SOURCE_SPI0_DO);
    PINCTRL_SetPadMux(SPI_LCD_SCLK, IO_SOURCE_SPI0_CLK);
    
    SPI_Init(AHB_SSP0);

    RTC_Enable(RTC_ENABLED);
    RTC_SetNextIntOffset(32768 * 10);
}

static SemaphoreHandle_t sem_heart_rate = NULL;

static void heart_rate_task(void *pdata)
{  
    heart_meas_init();
    for (;;)
    {
        BaseType_t r = xSemaphoreTake(sem_heart_rate,  portMAX_DELAY);
        if (pdTRUE == r)
        {
            heart_meas_sample();
        }
    }
}

uint32_t hr_timer_isr(void *user_data)
{
    BaseType_t xHigherPriorityTaskWoke = pdFALSE;
    TMR_IntClr(APB_TMR2);
    xSemaphoreGiveFromISR(sem_heart_rate, &xHigherPriorityTaskWoke);
    return 0;
}

uint32_t rtc_timer_isr(void *user_data)
{
    RTC_ClearInt();
    RTC_SetNextIntOffset(32768 * 10);
    disp_item(DISP_TIME);
    return 0;
}

uint32_t on_deep_sleep_wakeup(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);   
    setup_peripherals();
    return 0;
}
    
uint32_t query_deep_sleep_allowed(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);    
    return 0;
}

static void init_tasks()
{
    sem_heart_rate = xSemaphoreCreateBinary();

    xTaskCreate(heart_rate_task,
               "h",
               550,
               NULL,
               (configMAX_PRIORITIES - 1),
               NULL);
    
    disp_init();
}

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    // setup deep sleep handlers
    platform_set_evt_callback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, on_deep_sleep_wakeup, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, query_deep_sleep_allowed, NULL);

    // setup putc handle
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    setup_peripherals();
        
#ifndef SIMULATION
    platform_printf("init");
    i2c_init(I2C_PORT_0);
#endif
    
    init_tasks();

    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER2, hr_timer_isr, NULL);
    platform_set_irq_callback(PLATFORM_CB_IRQ_RTC, rtc_timer_isr, NULL);
    
    TMR_Enable(APB_TMR1);
    
    return 0;
}

void driver_delay_xms(uint32_t ms)
{ 
    vTaskDelay(pdMS_TO_TICKS(ms));
}

/*	Brief : The delay routine
 *	\param : delay in ms
*/
void BMA2x2_delay_msek(u32 msek)
{
	vTaskDelay(pdMS_TO_TICKS(msek));
}
