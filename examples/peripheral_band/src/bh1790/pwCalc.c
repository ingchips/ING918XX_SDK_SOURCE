/////////////////////////////////////////////////////////////////////////////////
// pwCalc.c
// 
// Copyright (c) 2016 ROHM Co.,Ltd.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
/////////////////////////////////////////////////////////////////////////////////

#include "sysDef.h"
#include "iirFilter.h"
#include "movingAverage.h"
#include "pwCalc.h"


//  Global Variables


//  Static Variables
static uint8_t   s_is_init;
static iirParam  s_iirPrm_hpf;  // Hi-Pass Filter
                                // fc=0.5Hz (fs=32.0Hz)
static iirParam  s_iirPrm_lpf;  // Low-Pass Filter
                                // fc=3.5Hz (fs=32.0Hz)
static MAParamF  s_maPrm_pre1;
static MAParamF  s_maPrm_pre2;
static MAParamF  s_maPrm_pre3;


//  Local Function Prototypes


/////////////////////////////////////////////////////////////////////////////////
//  Public Functions
/////////////////////////////////////////////////////////////////////////////////

//===============================================================================
// @brief Initialize Pulse Wave
// 
// @param[in]    : None
// @param[out]   : None
// @param[inout] : None
// @retval       : uint16_t
//                   System Error Code for Pulse Wave Calculation
//===============================================================================
uint16_t pwCalc_Init(void)
{
    int8_t   ret8  = MA_RC_OK;
    uint16_t ret16 = ERROR_NONE;
    
    const float32_t hpf_coeff[] = { 0.9329322F, -1.8658643F, 0.9329322F, 1.8613611F, -0.8703675F };
    const float32_t lpf_coeff[] = { 0.2635176F,  0.2635176F, 0.0F,       0.4729648F,  0.0F };
    
    // Move Average
    ret8 = ma_InitF(4U, &s_maPrm_pre1);
    if (ret8 == MA_RC_OK) {
        ret8 = ma_InitF(8U, &s_maPrm_pre2);
    }
    if (ret8 == MA_RC_OK) {
        ret8 = ma_InitF(8U, &s_maPrm_pre3);
    }
    if (ret8 != MA_RC_OK) {
        ret16 = ERROR_PWCALC_MA_PARAM;
    }
    
    // IIR Filter
    if (ret16 == ERROR_NONE) {
        s_is_init = 0U;
        iir_Init(hpf_coeff, &s_iirPrm_hpf);
        iir_Init(lpf_coeff, &s_iirPrm_lpf);
    }
    
    return (ret16);
}

//===============================================================================
// @brief Clear Pulse Wave
// 
// @param[in]    : None
// @param[out]   : None
// @param[inout] : None
// @retval       : void
//===============================================================================
void pwCalc_Clear(void)
{
    s_is_init = 0U;
    ma_ClearF(&s_maPrm_pre1);
    ma_ClearF(&s_maPrm_pre2);
    ma_ClearF(&s_maPrm_pre3);
}

//===============================================================================
// @brief Generate Pulse Wave
// 
// @param[in]    : u16_pair_t *pwData        => Pointer to BH1790 Raw Data
// @param[out]   : float32_t  *dataOut       => Pointer to Pulse Wave
// @param[inout] : None
// @retval       : void
//===============================================================================
void pwCalc(u16_pair_t *pwData, float32_t *dataOut)
{
    float32_t pre1Out    = 0.0F;
    float32_t pre2Out    = 0.0F;
    float32_t dataIn_ave = 0.0F;
    float32_t hpfOut     = 0.0F;
    
    pre1Out    = ma_AverageF((float32_t)pwData->on, &s_maPrm_pre1);
    pre2Out    = ma_AverageF(pre1Out,               &s_maPrm_pre2);
    dataIn_ave = ma_AverageF(pre2Out,               &s_maPrm_pre3);
    
    if (s_is_init == 0U) {
        s_is_init = 1U;
        iir_Set(dataIn_ave, dataIn_ave, &s_iirPrm_hpf);
        iir_Set(0.0F,       0.0F,       &s_iirPrm_lpf);
    }
    hpfOut   = iir_Filter(dataIn_ave, &s_iirPrm_hpf);
    *dataOut = iir_Filter(hpfOut,     &s_iirPrm_lpf);
}


/////////////////////////////////////////////////////////////////////////////////
//  Local Functions
/////////////////////////////////////////////////////////////////////////////////
