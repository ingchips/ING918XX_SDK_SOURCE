/////////////////////////////////////////////////////////////////////////////////
// heartRate.c
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
#include "heartRate.h"


//  Global Variables


//  Static Variables
static MAParamU8  s_maPrm_bpm;   // Moving Average Parameter of Heart Rate[Units: bpm]


//  Local Function Prototypes


/////////////////////////////////////////////////////////////////////////////////
//  Public Functions
/////////////////////////////////////////////////////////////////////////////////

//===============================================================================
// @brief Initialize Heart Rate
// 
// @param[in]    : None
// @param[out]   : hrParam *prm           => Pointer to Heart Rate Parameter
// @param[inout] : None
// @retval       : uint16_t
//                   System Error Code for Heart Rate
//===============================================================================
uint16_t hr_Init(hrParam *prm)
{
    int8_t   ret8  = MA_RC_OK;
    uint16_t ret16 = ERROR_NONE;
    
    prm->bpm       = 0U;
    prm->cnt       = 0;
    prm->cnt_diff2 = 0;
    prm->cnt_pkDet = 0;
    prm->last1     = 0.0F;
    prm->last2     = 0.0F;
    
    ret8 = ma_InitU8(4U, &s_maPrm_bpm);
    if (ret8 != MA_RC_OK) {
        ret16 = ERROR_HR_MA_PARAM;
    }
    
    return (ret16);
}

//===============================================================================
// @brief Clear Heart Rate
// 
// @param[in]    : uint8_t is_wearing     => Wearing or Not-Wearing
// @param[out]   : hrParam *prm           => Pointer to Heart Rate Parameter
// @param[inout] : None
// @retval       : uint16_t
//                   System Error Code for Heart Rate
//===============================================================================
uint16_t hr_Clear(hrParam *prm, uint8_t is_wearing)
{
    uint16_t ret16 = ERROR_NONE;
    
    if (is_wearing > HR_WEARING) {
        ret16 = ERROR_HR_IS_WEARING;
    }
    else {
        prm->cnt       = 0;
        prm->cnt_diff2 = 0;
        prm->cnt_pkDet = 0;
        
        if (is_wearing == HR_NOT_WEARING) {
            prm->bpm = 0U;
            ma_ClearU8(&s_maPrm_bpm);
        }
    }
    
    return (ret16);
}


//===============================================================================
// @brief Calculate Heart Rate
// 
// @param[in]    : float32_t data         => Current Data of Pulse Wave
// @param[out]   : hrParam   *prm         => Pointer to Heart Rate Parameter
// @param[inout] : None
// @retval       : void
//===============================================================================
void hr_CalcBPM(float32_t data, hrParam *prm)
{
    float32_t bpm_candidate = 0.0F;
    float32_t diff1         = 0.0F;
    float32_t diff2         = 0.0F;
    int32_t   tmp           = 0;
    
    prm->cnt++;
    
    if (prm->cnt > 1) {
        diff1 = data - prm->last1;
    }
    else {
        diff1 = 0.0F;
    }
    prm->last1 = data;
    
    if (prm->cnt > 2) {
        diff2 = diff1 - prm->last2;
    }
    else {
        diff2 = 0.0F;
    }
    prm->last2 = diff1;
    
    tmp = prm->cnt - prm->cnt_pkDet;
    if (tmp > 0) {
        bpm_candidate = 60.0F / (HR_SAMPLING_PERIOD * (float32_t)tmp);
    }
    else {
        bpm_candidate = 0.0F;
    }
    
    if (bpm_candidate < HR_BPM_MIN_TH) {
        prm->cnt_pkDet = 0;
    }
    
    if ((diff1 < 0.0F) && (diff2 >= 0.0F) && (prm->cnt_diff2 >= 3)) {
        if(prm->cnt_pkDet != 0) {
            if (bpm_candidate <= HR_BPM_MAX_TH) {
                bpm_candidate += 0.5F;
                prm->bpm = ma_AverageU8((uint8_t)bpm_candidate, &s_maPrm_bpm);
            }
        }
        prm->cnt_pkDet = prm->cnt;
    }
    
    if (diff2 < 0.0F) {
        prm->cnt_diff2 = prm->cnt_diff2 + 1;
    }
    else {
        prm->cnt_diff2 = 0;
    }
}


/////////////////////////////////////////////////////////////////////////////////
//  Local Functions
/////////////////////////////////////////////////////////////////////////////////
