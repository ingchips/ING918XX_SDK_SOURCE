#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "step_calc.h"
#include "iic.h"
#include "bma2x2.h"
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

    // timer 0 can be used as watchdog, so we use timer 1.
    // setup timer 1: 50Hz
	TMR_SetCMP(APB_TMR1, TMR_CLK_FREQ / ACC_SAMPLING_RATE);
	TMR_SetOpMode(APB_TMR1, TMR_CTL_OP_MODE_WRAPPING);
	TMR_IntEnable(APB_TMR1);
    TMR_Reload(APB_TMR1);
	TMR_Enable(APB_TMR1);
}

static SemaphoreHandle_t sem_pedometer = NULL;

static void pedometer_task(void *pdata)
{
    pedometer_init();
    for (;;)
    {
        BaseType_t r = xSemaphoreTake(sem_pedometer,  portMAX_DELAY);
        if (pdTRUE == r)
            accelarator_sample();
    }
}

/*	Brief : The delay routine
 *	\param : delay in ms
*/
void BMA2x2_delay_msek(u32 msek)
{
	xSemaphoreTake(sem_pedometer, pdMS_TO_TICKS(msek));
}

uint32_t timer_isr(void *user_data)
{
    BaseType_t xHigherPriorityTaskWoke = pdFALSE;
    TMR_IntClr(APB_TMR1);
    xSemaphoreGiveFromISR(sem_pedometer, &xHigherPriorityTaskWoke);
    return 0;
}

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    setup_peripherals();

    sem_pedometer = xSemaphoreCreateBinary();
    sysSetPublicDeviceAddr((const unsigned char *)(0x26130));

    // setup putc handle
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);
    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER1, timer_isr, NULL);
    // platform_config(PLATFORM_CFG_LOG_HCI, PLATFORM_CFG_ENABLE);
    
#ifndef SIMULATION
    printf("init");
    i2c_init(I2C_PORT_0);
#endif
    
    xTaskCreate(pedometer_task,
               "b",
               150,
               NULL,
               (configMAX_PRIORITIES - 1),
               NULL);
    return 0;
}



