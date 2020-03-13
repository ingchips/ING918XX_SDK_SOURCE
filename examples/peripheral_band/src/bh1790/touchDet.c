/////////////////////////////////////////////////////////////////////////////////
// touchDet.c
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

#include "touchDet.h"


//  Global Variables


//  Static Variables
static uint16_t  s_cnt_goff;
static uint16_t  s_cnt_gon;
static int8_t    s_flg_slope;
static float32_t s_peak_Max;
static float32_t s_peak_Min;

//  Local Function Prototypes
static uint8_t touchDet_CheckAMP(float32_t pulse_wave);


/////////////////////////////////////////////////////////////////////////////////
//  Public Functions
/////////////////////////////////////////////////////////////////////////////////

//===============================================================================
// @brief Initialize Touch Detection
// 
// @param[in]    : None
// @param[out]   : TD_STATE *td_stat        => Pointer to the state
//                                             of Touch Detection
// @param[inout] : None
// @retval       : void
//===============================================================================
void touchDet_Init(TD_STATE *td_stat)
{
    s_cnt_goff  = 0U;
    s_cnt_gon   = 0U;
    s_flg_slope = 0;
    s_peak_Max  = 0.0F;
    s_peak_Min  = 0.0F;
    
    *td_stat = td_state_w1;
}

//===============================================================================
// @brief State Machine for Touch Detection
// 
// @param[in]    : u16_pair_t *data         => Pointer to BH1790 Raw Data
//               : float32_t  pulse_wave    => Current data of Pulse Wave
// @param[out]   : None
// @param[inout] : TD_STATE   *td_stat      => Pointer to the state
//                                             of Touch Detection
// @retval       : void
//===============================================================================
void touchDet(const u16_pair_t *data, float32_t pulse_wave, TD_STATE *td_stat)
{
    uint8_t ret        = 0U;
    uint8_t is_cleared = 0U;
    
    switch(*td_stat) {
        case td_state_w1:
            if (data->off < TD_TH_GOFF_W1W2) {
                is_cleared = 1U;
                *td_stat   = td_state_w3;
            }
            else {
                s_cnt_goff++;
                
                if (s_cnt_goff >= TD_TH_CNT_GOFF_W1TOW2) {
                    is_cleared = 1U;
                    *td_stat   = td_state_w2;
                }
            }
            break;
        
        case td_state_w2:
            if (data->off < TD_TH_GOFF_W1W2) {
                *td_stat = td_state_w3;
            }
            break;
        
        case td_state_w3:
        case td_state_w4:
            if (data->off < TD_TH_GOFF_W3W4) {
                s_cnt_goff = 0U;
                
                if (data->on >= TD_TH_GON_W3W4) {
                    is_cleared = 1U;
                    *td_stat   = td_state_w5;
                    break;
                }
            }
            else {
                s_cnt_goff++;
                if (s_cnt_goff >= TD_TH_CNT_GOFF_W3W4TOW1) {
                    is_cleared = 1U;
                    *td_stat   = td_state_w1;
                    break;
                }
            }
            
            s_cnt_gon++;
            
            if (s_cnt_gon >= TD_TH_CNT_GON_W3W4TOW1) {
                is_cleared = 1U;
                *td_stat   = td_state_w1;
            }
            else {
                if (s_cnt_gon == TD_TH_CNT_GON_W3TOW4) {
                    *td_stat = td_state_w4;
                }
            }
            break;
        
        case td_state_w5:
            if (data->off < TD_TH_GOFF_W5) {
                s_cnt_goff = 0U;
            }
            else {
                s_cnt_goff++;
                if (s_cnt_goff >= TD_TH_CNT_GOFF_W5TOW1) {
                    is_cleared = 1U;
                    *td_stat   = td_state_w1;
                    break;
                }
            }
            
            ret = touchDet_CheckAMP(pulse_wave);
            if (ret == 0U) {
                is_cleared = 1U;
                *td_stat   = td_state_w1;
            }
            break;
        
        default:
            break;
    }
    
    if (is_cleared == 1U) {
        s_cnt_goff  = 0U;
        s_cnt_gon   = 0U;
        s_flg_slope = 0;
    }
}


/////////////////////////////////////////////////////////////////////////////////
//  Local Functions
/////////////////////////////////////////////////////////////////////////////////

//===============================================================================
// @brief Check if the amplitude of Pulse Wave is large enough for 
//        Heart Rate Calculation
// 
// @param[in]    : float32_t pulse_wave     => Current data of Pulse Wave
// @param[out]   : None
// @param[inout] : None
// @retval       : uint8_t
//                   Judgement Result
//===============================================================================
static uint8_t touchDet_CheckAMP(float32_t pulse_wave)
{
    uint8_t  ret = 1U;

    if (s_flg_slope == 0) {
        s_flg_slope = 1;
        s_peak_Max  = pulse_wave;
        s_peak_Min  = pulse_wave;
    }
    else if (s_flg_slope > 0) {
        if(s_peak_Max <= pulse_wave) { 
            s_peak_Max = pulse_wave;
        }
        else {
            s_flg_slope = -1;
            s_peak_Min  = pulse_wave;
        }
    }
    else {
        if (s_peak_Min >= pulse_wave) { 
            s_peak_Min = pulse_wave;
        }
        else {
            s_flg_slope = 1;
            s_peak_Max  = pulse_wave;
        }
    }

    if ((s_peak_Max - s_peak_Min) > TD_TH_AMP_W5) {
        s_cnt_gon = 0U;
    }
    else {
        s_cnt_gon++;
        if (s_cnt_gon >= TD_TH_CNT_GON_W5TOW1) {
            ret = 0U;
        }
    }

    return (ret);
}
