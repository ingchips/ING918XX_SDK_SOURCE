#define  OPTIONAL_RF_CLK
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adc_cali.h"
#include "peripheral_adc.h"

#define PRINT_PORT    APB_UART0

static uint32_t cb_hard_fault(hard_fault_info_t *info, void *_)
{
    platform_printf("HARDFAULT:\nPC : 0x%08X\nLR : 0x%08X\nPSR: 0x%08X\n"
                    "R0 : 0x%08X\nR1 : 0x%08X\nR2 : 0x%08X\nP3 : 0x%08X\n"
                    "R12: 0x%08X\n",
                    info->pc, info->lr, info->psr,
                    info->r0, info->r1, info->r2, info->r3, info->r12);
    for (;;);
}

static uint32_t cb_assertion(assertion_info_t *info, void *_)
{
    platform_printf("[ASSERTION] @ %s:%d\n",
                    info->file_name,
                    info->line_no);
    for (;;);
}

static uint32_t cb_heap_out_of_mem(uint32_t tag, void *_)
{
    platform_printf("[OOM] @ %d\n", tag);
    for (;;);
}

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

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
typedef struct {
    uint8_t s;
    uint8_t cnt;
    uint16_t data[AVE_NUM];
} __attribute__((packed)) SADC_adcAve_t;
static SADC_adcAve_t *adcAve[12];
static SADC_ftCali_t SADC_ftCali_data;

uint16_t ADC_GetAveData(uint32_t data)
{
    SADC_channelId ch = ADC_GetDataChannel(data);
    if (!adcAve[ch]) return ADC_GetData(data);
    adcAve[ch]->data[adcAve[ch]->cnt] = ADC_GetData(data);
    adcAve[ch]->cnt++;
    if (adcAve[ch]->cnt >= AVE_NUM)
        adcAve[ch]->cnt = 0;
    if (adcAve[ch]->s < AVE_NUM)
        adcAve[ch]->s++;
    uint8_t i;
    uint32_t sum = 0;
    for (i = 0; i < adcAve[ch]->s; ++i)
        sum += adcAve[ch]->data[i];
    if (adcAve[ch]->s <= 2)
        return sum / i;
#if (INCLUDE_EXTREMUM == 0)
    uint16_t min = 0x3fff;
    uint16_t max = 0;
    for (i = 0; i < adcAve[ch]->s; ++i) {
        if (adcAve[ch]->data[i] < min)
            min = adcAve[ch]->data[i];
        if (adcAve[ch]->data[i] > max)
            max = adcAve[ch]->data[i];
    }
    sum -= min;
    sum -= max;
    i -= 2;
#endif
    return sum / i;
}
static void ADC_AveInitSet(SADC_channelId ch)
{
    if (adcAve[ch])
        memset(adcAve[ch], 0, 3);
    else
        adcAve[ch] = malloc(sizeof(SADC_adcAve_t));
}
void ADC_AveInit(void)
{
    uint16_t ch;
    uint16_t reg = ADC_GetEnabledChannels();
    for (ch = 0; ch < 12; ++ch) {
        if (reg & (1 << ch))
            ADC_AveInitSet((SADC_channelId)ch);
    }
}
void ADC_AveDisable(void)
{
    uint8_t i;
    for (i = 0; i < 12; ++i) {
        if (!adcAve[i]) continue;
        free(adcAve[i]);
        adcAve[i] = 0;
    }
}
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
typedef struct {
    uint8_t s;
    uint8_t cnt;
    uint16_t data[AVE_NUM];
} __attribute__((packed)) SADC_adcAve_t;
static SADC_adcAve_t *adcAve[12];

uint16_t ADC_GetAveData(uint32_t data)
{
    SADC_channelId ch = ADC_GetDataChannel(data);
    if (!adcAve[ch]) return ADC_GetData(data);
    adcAve[ch]->data[adcAve[ch]->cnt] = ADC_GetData(data);
    adcAve[ch]->cnt++;
    if (adcAve[ch]->cnt >= AVE_NUM)
        adcAve[ch]->cnt = 0;
    if (adcAve[ch]->s < AVE_NUM)
        adcAve[ch]->s++;
    uint8_t i;
    uint32_t sum = 0;
    for (i = 0; i < adcAve[ch]->s; ++i)
        sum += adcAve[ch]->data[i];
    if (adcAve[ch]->s <= 2)
        return sum / i;
#if (INCLUDE_EXTREMUM == 0)
    uint16_t min = 0x3fff;
    uint16_t max = 0;
    for (i = 0; i < adcAve[ch]->s; ++i) {
        if (adcAve[ch]->data[i] < min)
            min = adcAve[ch]->data[i];
        if (adcAve[ch]->data[i] > max)
            max = adcAve[ch]->data[i];
    }
    sum -= min;
    sum -= max;
    i -= 2;
#endif
    return sum / i;
}
static void ADC_AveInitSet(SADC_channelId ch)
{
    if (adcAve[ch])
        memset(adcAve[ch], 0, 3);
    else
        adcAve[ch] = malloc(sizeof(SADC_adcAve_t));
}
void ADC_AveInit(void)
{
    uint16_t ch;
    uint16_t reg = ADC_GetEnabledChannels();
    for (ch = 0; ch < 12; ++ch) {
        if (reg & (1 << ch))
            ADC_AveInitSet((SADC_channelId)ch);
    }
}
void ADC_AveDisable(void)
{
    uint8_t i;
    for (i = 0; i < 12; ++i) {
        if (!adcAve[i]) continue;
        free(adcAve[i]);
        adcAve[i] = 0;
    }
}
#endif

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_TMR1));

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    // setup timer 1: 0.5s (2Hz)
    TMR_SetCMP(APB_TMR1, TMR_CLK_FREQ / 2);
    TMR_SetOpMode(APB_TMR1, TMR_CTL_OP_MODE_WRAPPING);
    TMR_IntEnable(APB_TMR1);
    TMR_Reload(APB_TMR1);
    TMR_Enable(APB_TMR1);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    // setup channel 0 timer 1: 0.5s (2Hz)
    SYSCTRL_SelectTimerClk(TMR_PORT_1, SYSCTRL_CLK_32k);
    TMR_SetOpMode(APB_TMR1, 0, TMR_CTL_OP_MODE_32BIT_TIMER_x1, TMR_CLK_MODE_EXTERNAL, 0);
    TMR_SetReload(APB_TMR1, 0, TMR_GetClk(APB_TMR1, 0) / 2);
    TMR_Enable(APB_TMR1, 0, 0xf);
    TMR_IntEnable(APB_TMR1, 0, 0xf);
#ifndef SIMULATION
    // setup ADC
    SYSCTRL_ClearClkGate(SYSCTRL_ITEM_APB_ADC);
    SYSCTRL_SetAdcClkDiv(4);
    SYSCTRL_ReleaseBlock(SYSCTRL_ITEM_APB_ADC);
    ADC_Reset();
    ADC_Calibration(SINGLE_END_MODE);
    ADC_ftInitCali(&SADC_ftCali_data);
    ADC_VrefCalibration();
#endif
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
    // setup channel 0 timer 1: 0.5s (2Hz)
    SYSCTRL_SelectTimerClk(TMR_PORT_1, 1, SOURCE_32K_CLK);
    TMR_SetOpMode(APB_TMR1, 0, TMR_CTL_OP_MODE_32BIT_TIMER_x1, TMR_CLK_MODE_EXTERNAL, 0);
    TMR_SetReload(APB_TMR1, 0, TMR_GetClk(APB_TMR1, 0) / 2);
    TMR_Enable(APB_TMR1, 0, 0xf);
    TMR_IntEnable(APB_TMR1, 0, 0xf);
#ifndef SIMULATION
    // setup ADC
    SYSCTRL_ClearClkGate(SYSCTRL_ITEM_APB_ADC);
    SYSCTRL_SetAdcClkDiv(4);
    SYSCTRL_ReleaseBlock(SYSCTRL_ITEM_APB_ADC);
    ADC_Reset();
#endif
#else
    #error unknown or unsupported chip family
#endif

}

uint32_t timer_isr(void *user_data);

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
void ADC_ClearChannelDataValid(const uint8_t channel_id) { }
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
void ADC_ClearChannelDataValid(const uint8_t channel_id) { }
uint16_t ADC_ReadChannelData(const uint8_t channel_id) { return 0;}
#endif

const platform_evt_cb_table_t evt_cb_table =
{
    .callbacks = {
        [PLATFORM_CB_EVT_HARD_FAULT] = {
            .f = (f_platform_evt_cb)cb_hard_fault
        },
        [PLATFORM_CB_EVT_ASSERTION] = {
            .f = (f_platform_evt_cb)cb_assertion
        },
        [PLATFORM_CB_EVT_HEAP_OOM] = {
            .f = (f_platform_evt_cb)cb_heap_out_of_mem
        },
        [PLATFORM_CB_EVT_PUTC] = {
            .f = (f_platform_evt_cb)cb_putc
        },
        [PLATFORM_CB_EVT_PROFILE_INIT] = {
            .f = setup_profile
        },
    }
};

int app_main()
{
    // setup handlers
    platform_set_evt_callback_table(&evt_cb_table);

    setup_peripherals();

    SYSCTRL_Init();

    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER1, timer_isr, NULL);

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    adc_prepare_calibration();
#endif

    return 0;
}



