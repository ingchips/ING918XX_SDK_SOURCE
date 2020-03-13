/////////////////////////////////////////////////////////////////////////////////
// lxCtrl.c
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
#include "bh1790.h"
#include "pw_bh1790.h"
#include "lxCtrl.h"
#include "touchDet.h"


//  Global Variables


//  Static Variables
static const uint8_t led_config_w1[] = {
    BH1790_PRM_CTRL2_ONTIME_0_3MS, BH1790_PRM_CTRL2_CUR_0MA
};

static int32_t    s_led_lv;
static TD_STATE   s_pre_stat;
static uint16_t   s_th_up;
static uint16_t   s_cnt_lv_sel;

//  Local Function Prototypes
static void lxCtrl_LvSel(const u16_pair_t *data, uint8_t *is_updated_led);


/////////////////////////////////////////////////////////////////////////////////
//  Public Functions
/////////////////////////////////////////////////////////////////////////////////

//===============================================================================
// @brief Initialize Lx Control
// 
// @param[in]    : None
// @param[out]   : None
// @param[inout] : None
// @retval       : uint16_t
//                   System Error Code for BH1790
//===============================================================================
uint16_t lxCtrl_Init(void)
{
    uint16_t ret16 = ERROR_NONE;
    
    s_led_lv     = led_level_0;
    s_pre_stat   = td_state_w1;
    s_th_up      = LX_TH_UP_INIT;
    s_cnt_lv_sel = 0U;
    
    ret16 = pw_SetParam(BH1790_PRM_CTRL2_ONTIME, led_config_w1[0]);
    if (ret16 == ERROR_NONE) {
        ret16 = pw_SetParam(BH1790_PRM_CTRL2_CUR, led_config_w1[1]);
    }
    
    return (ret16);
}

//===============================================================================
// @brief Lx Control
// 
// @param[in]    : u16_pair_t *data           => Pointer to BH1790 Raw Data
//               : TD_STATE   td_stat         => Current state of Touch Detection
// @param[out]   : uint8_t    *is_updated_led => Update flag for LED Config
// @param[inout] : None
// @retval       : uint16_t
//                   System Error Code for BH1790
//===============================================================================
uint16_t lxCtrl(const u16_pair_t *data, TD_STATE td_stat, uint8_t *is_updated_led)
{
    uint16_t ret16 = ERROR_NONE;
    
    uint8_t led_mS = 0U;
    uint8_t led_mA = 0U;
    
    const uint8_t led_config_w3[] = {
        BH1790_PRM_CTRL2_ONTIME_0_3MS, BH1790_PRM_CTRL2_CUR_20MA
    };
    const uint8_t led_config_w5[][2] = {
        {BH1790_PRM_CTRL2_ONTIME_0_3MS, BH1790_PRM_CTRL2_CUR_1MA},
        {BH1790_PRM_CTRL2_ONTIME_0_3MS, BH1790_PRM_CTRL2_CUR_2MA},
        {BH1790_PRM_CTRL2_ONTIME_0_3MS, BH1790_PRM_CTRL2_CUR_3MA},
        {BH1790_PRM_CTRL2_ONTIME_0_3MS, BH1790_PRM_CTRL2_CUR_6MA},
        {BH1790_PRM_CTRL2_ONTIME_0_3MS, BH1790_PRM_CTRL2_CUR_10MA},
        {BH1790_PRM_CTRL2_ONTIME_0_6MS, BH1790_PRM_CTRL2_CUR_10MA},
        {BH1790_PRM_CTRL2_ONTIME_0_6MS, BH1790_PRM_CTRL2_CUR_20MA},
        {BH1790_PRM_CTRL2_ONTIME_0_6MS, BH1790_PRM_CTRL2_CUR_30MA},
        {BH1790_PRM_CTRL2_ONTIME_0_6MS, BH1790_PRM_CTRL2_CUR_60MA}
    };
    
    
    *is_updated_led = 0U;
    
    switch (td_stat) {
        case td_state_w1:
            if (s_pre_stat != td_state_w1) {
                led_mS          = led_config_w1[0];
                led_mA          = led_config_w1[1];
                *is_updated_led = 1U;
            }
            break;
        
        case td_state_w3:
            if (s_pre_stat != td_state_w3) {
                led_mS          = led_config_w3[0];
                led_mA          = led_config_w3[1];
                *is_updated_led = 1U;
            }
            break;
        
        case td_state_w5:
            if (s_pre_stat != td_state_w5) {
                s_cnt_lv_sel    = 0U;
                s_th_up         = LX_TH_UP_INIT;
                s_led_lv        = led_level_0;
                *is_updated_led = 1U;
            }
            else {
                if (data->off < TD_TH_GOFF_W5) {
                    lxCtrl_LvSel(data, is_updated_led);
                }
            }
            
            led_mS = led_config_w5[s_led_lv][0];
            led_mA = led_config_w5[s_led_lv][1];
            break;
        
        default:
            break;
    }
    
    if (*is_updated_led == 1U) {
        ret16 = pw_StopMeasure();
        if (ret16 == ERROR_NONE) {
            ret16 = pw_SetParam(BH1790_PRM_CTRL2_ONTIME, led_mS);
        }
        if (ret16 == ERROR_NONE) {
            ret16 = pw_SetParam(BH1790_PRM_CTRL2_CUR, led_mA);
        }
        if (ret16 == ERROR_NONE) {
            ret16 = pw_StartMeasure();
        }
    }
    
    s_pre_stat = td_stat;
    
    return (ret16);
}

/////////////////////////////////////////////////////////////////////////////////
//  Local Functions
/////////////////////////////////////////////////////////////////////////////////

//===============================================================================
// @brief Select LED Config for BH1790 while wearing a heart rate monitor
// 
// @param[in]    : u16_pair_t *data           => Pointer to BH1790 Raw Data
// @param[out]   : uint8_t    *is_updated_led => Update flag for LED Config
// @param[inout] : None
// @retval       : void
//===============================================================================
static void lxCtrl_LvSel(const u16_pair_t *data, uint8_t *is_updated_led)
{
    if (data->on >= s_th_up) {
        s_th_up = LX_TH_UP_IMMED;
        
        if (data->on >= LX_TH_DOWN_IMMED) {
            s_cnt_lv_sel = 0U;
            if (s_led_lv > led_level_0) {
                s_led_lv--;
                *is_updated_led = 1U;
            }
        }
        else {
            if (data->on >= LX_TH_DOWN_STEPPED) {
                s_cnt_lv_sel++;
                if (s_cnt_lv_sel >= LX_TH_CNT_LV_SEL) {
                    s_cnt_lv_sel = 0U;
                    if (s_led_lv > led_level_0) {
                        s_led_lv--;
                        *is_updated_led = 1U;
                    }
                }
            }
            else {
                s_cnt_lv_sel = 0U;
            }
        }
    }
    else {
        s_cnt_lv_sel = 0U;
        if (s_led_lv < led_level_max) {
            s_led_lv++;
            *is_updated_led = 1U;
        }
    }
}
