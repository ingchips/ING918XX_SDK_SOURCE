/////////////////////////////////////////////////////////////////////////////////
// lxCtrl.h
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


#ifndef LX_CTRL_H
#define LX_CTRL_H

#include "typeDef.h"
#include "touchDet.h"


//===============================================================================
//  Constants
//===============================================================================
// Threshold
// LED_LIGHTING_FREQ 128Hz, RCYCLE 32Hz
#define LX_TH_UP_INIT               (10000U)
#define LX_TH_UP_IMMED              (5000U)
#define LX_TH_DOWN_IMMED            (27000U)
#define LX_TH_DOWN_STEPPED          (22000U)

#define LX_TH_CNT_LV_SEL            (160U)     // 5sec


//===============================================================================
//  Type Definition
//===============================================================================
typedef enum {
    led_level_0 = 0,
    led_level_1 = 1,
    led_level_2 = 2,
    led_level_3 = 3,
    led_level_4 = 4,
    led_level_5 = 5,
    led_level_6 = 6,
    led_level_7 = 7,
    led_level_8 = 8,
    led_level_max = 8
} LED_LEVEL;


//===============================================================================
//  Extern Global Variables
//===============================================================================


//===============================================================================
//  Public Function Prototypes
//===============================================================================
uint16_t lxCtrl_Init(void);
uint16_t lxCtrl(const u16_pair_t *data, TD_STATE td_stat, uint8_t *is_updated_led);



//===============================================================================
//  Macro
//===============================================================================



#endif  // LX_CTRL_H
