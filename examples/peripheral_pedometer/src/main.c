#define OPTIONAL_RF_CLK
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
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_I2C0)
                              | (1 << SYSCTRL_ClkGate_APB_TMR1));

#ifndef SIMULATION
    PINCTRL_SetPadMux(10, IO_SOURCE_I2C0_SCL_OUT);
    PINCTRL_SetPadMux(11, IO_SOURCE_I2C0_SDA_OUT);
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    PINCTRL_SelI2cSclIn(I2C_PORT_0, 10);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)   
    PINCTRL_SelI2cIn(I2C_PORT_0, 10, IO_NOT_A_PIN);
#else
    #error unknown or unsupported chip family
#endif
    printf("init");
    i2c_init(I2C_PORT_0);
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    // timer 0 can be used as watchdog, so we use timer 1.
    // setup timer 1: 50Hz
	TMR_SetCMP(APB_TMR1, TMR_CLK_FREQ / ACC_SAMPLING_RATE);
	TMR_SetOpMode(APB_TMR1, TMR_CTL_OP_MODE_WRAPPING);
	TMR_IntEnable(APB_TMR1);
    TMR_Reload(APB_TMR1);
	TMR_Enable(APB_TMR1);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    // setup channel 0 of timer 1: 50Hz
    TMR_SetOpMode(APB_TMR1, 0, TMR_CTL_OP_MODE_32BIT_TIMER_x1, TMR_CLK_MODE_APB, 0);
    TMR_SetReload(APB_TMR1, 0, TMR_GetClk(APB_TMR1, 0) / ACC_SAMPLING_RATE);
    TMR_Enable(APB_TMR1, 0, 0xf);
#else
    #error unknown or unsupported chip family
#endif
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
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    TMR_IntClr(APB_TMR1);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    TMR_IntClr(APB_TMR1, 0, 0xf);
#else
    #error unknown or unsupported chip family
#endif
    
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

    // setup putc handle
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);
    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER1, timer_isr, NULL);
    
    xTaskCreate(pedometer_task,
               "b",
               150,
               NULL,
               (configMAX_PRIORITIES - 1),
               NULL);
    return 0;
}



