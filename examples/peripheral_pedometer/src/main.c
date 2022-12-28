#define OPTIONAL_RF_CLK
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "step_calc.h"
#include "iic.h"
#include "bma2x2.h"
#include "board.h"
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

#define I2C_SCL         GIO_GPIO_10
#define I2C_SDA         GIO_GPIO_11

void setup_peripherals_i2c_pin(void)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918) 
    SYSCTRL_ClearClkGateMulti( (1 << SYSCTRL_ClkGate_APB_I2C0)
                                | (1 << SYSCTRL_ClkGate_APB_GPIO0)
                                | (1 << SYSCTRL_ClkGate_APB_GPIO1)
                                |(1 << SYSCTRL_ClkGate_APB_PinCtrl));
    PINCTRL_SetPadMux(10, IO_SOURCE_I2C0_SCL_O);
    PINCTRL_SetPadMux(11, IO_SOURCE_I2C0_SDO);
    PINCTRL_SelI2cSclIn(I2C_PORT, 10);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ITEM_APB_I2C0)
                                | (1 << SYSCTRL_ITEM_APB_SysCtrl)
                                | (1 << SYSCTRL_ITEM_APB_PinCtrl)
                                | (1 << SYSCTRL_ITEM_APB_GPIO1)
                                | (1 << SYSCTRL_ITEM_APB_GPIO0));
    
    PINCTRL_Pull(I2C_SCL,PINCTRL_PULL_UP);
    PINCTRL_Pull(I2C_SDA,PINCTRL_PULL_UP);
    PINCTRL_SelI2cIn(I2C_PORT_0,I2C_SCL,I2C_SDA);
    PINCTRL_SetPadMux(I2C_SCL,IO_SOURCE_I2C0_SCL_OUT);
    PINCTRL_SetPadMux(I2C_SDA,IO_SOURCE_I2C0_SDA_OUT);
#else
    #error unknown or unsupported chip family
#endif
}

void setup_peripherals_i2c(void)
{
    setup_peripherals_i2c_pin();
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    //init I2C module
    I2C_Config(APB_I2C0,I2C_ROLE_MASTER,I2C_ADDRESSING_MODE_07BIT,get_acc_addr());
    I2C_ConfigClkFrequency(APB_I2C0,I2C_CLOCKFREQUENY_STANDARD);
    I2C_Enable(APB_I2C0,1);
    I2C_IntEnable(APB_I2C0,(1<<I2C_INT_CMPL)|(1<<I2C_INT_ADDR_HIT));
#endif
    printf("init");
    i2c_init(I2C_PORT_0);
}



void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_TMR1));

#ifndef SIMULATION
    setup_peripherals_i2c();
#endif

    SYSCTRL_SelectTimerClk(TMR_PORT_0,SYSCTRL_CLK_32k);

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
    SYSCTRL_SelectTimerClk(TMR_PORT_0,SYSCTRL_CLK_32k);
    TMR_SetOpMode(APB_TMR1, 0, TMR_CTL_OP_MODE_32BIT_TIMER_x1, TMR_CLK_MODE_APB, 0);
    TMR_IntEnable(APB_TMR1,0,0xf);
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



