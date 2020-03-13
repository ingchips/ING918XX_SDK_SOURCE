/////////////////////////////////////////////////////////////////////////////////
// touchDet.h
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

#ifndef TOUCH_DET_H
#define TOUCH_DET_H

#include "typeDef.h"


//===============================================================================
//  Constants
//===============================================================================
// Threshold
// LED_LIGHTING_FREQ 128Hz, RCYCLE 32Hz
#define TD_TH_GOFF_W1W2                 (500U)
#define TD_TH_GOFF_W3W4                 (1000U)
#define TD_TH_GOFF_W5                   (1000U)
#define TD_TH_GON_W3W4                  (5000U)
#define TD_TH_AMP_W5                    (20.0F)

#define TD_TH_CNT_GOFF_W1TOW2           (32U)   // 1sec
#define TD_TH_CNT_GOFF_W3W4TOW1         (32U)   // 1sec
#define TD_TH_CNT_GON_W3TOW4            (32U)   // 1sec
#define TD_TH_CNT_GON_W3W4TOW1          (960U)  // 30sec
#define TD_TH_CNT_GOFF_W5TOW1           (32U)   // 1sec
#define TD_TH_CNT_GON_W5TOW1            (960U)  // 30sec


//===============================================================================
//  Type Definition
//===============================================================================
typedef enum {
    td_state_w1 = 1,
    td_state_w2,
    td_state_w3,
    td_state_w4,
    td_state_w5
} TD_STATE;


//===============================================================================
//  Extern Global Variables
//===============================================================================


//===============================================================================
//  Public Function Prototypes
//===============================================================================
void touchDet_Init(TD_STATE *td_stat);
void touchDet(const u16_pair_t *data, float32_t pulse_wave, TD_STATE *td_stat);



//===============================================================================
//  Macro
//===============================================================================



#endif  // TOUCH_DET_H
