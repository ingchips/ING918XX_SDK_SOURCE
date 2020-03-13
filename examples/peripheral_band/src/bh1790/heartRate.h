/////////////////////////////////////////////////////////////////////////////////
// heartRate.h
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

#ifndef HEART_RATE_H
#define HEART_RATE_H

#include "typeDef.h"


//===============================================================================
//  Constants
//===============================================================================
#define HR_VERSION          ("1.00")

#define HR_BPM_MIN_TH       (40.0F)
#define HR_BPM_MAX_TH       (240.0F)

#define HR_SAMPLING_PERIOD  (0.03125F)

#define HR_NOT_WEARING      (0U)
#define HR_WEARING          (1U)

//===============================================================================
//  Type Definition
//===============================================================================
typedef struct {
    uint8_t      bpm;
    int32_t      cnt;
    int32_t      cnt_diff2;
    int32_t      cnt_pkDet;
    float32_t    last1;
    float32_t    last2;
} hrParam;


//===============================================================================
//  Extern Global Variables
//===============================================================================


//===============================================================================
//  Public Function Prototypes
//===============================================================================
uint16_t hr_Init(hrParam *prm);
uint16_t hr_Clear(hrParam *prm, uint8_t is_wearing);
void     hr_CalcBPM(float32_t data, hrParam *prm);



//===============================================================================
//  Macro
//===============================================================================



#endif  // HEART_RATE_H
