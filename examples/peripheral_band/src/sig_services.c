#include "sig_services.h"
#include "ingsoc.h"
#include "iic.h"

#include "bh1790/bh1790.h"
#include "bh1790/hr_bh1790.h"

#include <stdlib.h>

rsc_meas_t rsc_meas = {0};
uint8_t rsc_enable = 0;

heart_rate_meas_t heart_rate_meas = {0};
uint8_t heart_rate_enable = 0;

static uint8_t s_cnt_freq = 0;

void heart_meas_init(void)
{
}

void req_send_hr_meas(void);

void heart_meas_sample(void)
{
#ifndef SIMULATION     
    uint8_t  bpm     = 0;
    uint8_t  wearing = 0;

    hr_bh1790_Calc(s_cnt_freq);
    s_cnt_freq++;
    if (s_cnt_freq >= SAMPLING_CNT_32HZ)
    {
        s_cnt_freq = 0;    
        hr_bh1790_GetData(&bpm, &wearing);
        heart_rate_meas.flags = wearing ? 3 << 1 : 2 << 1;
        heart_rate_meas.heart_rate = bpm;
        if (heart_rate_enable)
            req_send_hr_meas();
    }
#else
    s_cnt_freq++;
    if (s_cnt_freq >= SAMPLING_CNT_32HZ)
    {
        s_cnt_freq = 0;    
        heart_rate_meas.flags = 3 << 1;
        heart_rate_meas.heart_rate = 60 + (rand() & 0xf);
        if (heart_rate_enable)
            req_send_hr_meas();
    }
#endif
}

void heart_meas_ctrl(const uint8_t enable)
{
    heart_rate_enable = enable;
    if (heart_rate_enable)
    {
        s_cnt_freq = 0;
#ifndef SIMULATION        
        hr_bh1790_Init();
        hr_bh1790_StartMeasure();
#endif
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
        TMR_Enable(APB_TMR2);
#else
        #warning WIP
#endif
    }
    else
    {
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
        TMR_Disable(APB_TMR2);
#else
        #warning WIP
#endif
    }
}

int8_t bh1790_Read(uint8_t reg_addr, uint8_t *data, uint8_t size)
{
    i2c_read(I2C_PORT_0, BH1790_SLAVE_ADDRESS, &reg_addr, 1,
             data, size);

    return BH1790_RC_OK;
}

int8_t bh1790_Write(uint8_t reg_addr, uint8_t *data, uint8_t size)
{
    uint8_t array[20];
	uint8_t i = 0;
	array[0] = reg_addr;
	for (i = 0; i < size; i++) {
		array[i + 1] = data[i];
	}

	i2c_write(I2C_PORT_0, BH1790_SLAVE_ADDRESS, array, size + 1);
    
    return BH1790_RC_OK;
}

uint8_t heart_meas_get_enable(void)
{
    return heart_rate_enable;
}
