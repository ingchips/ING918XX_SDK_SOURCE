/////////////////////////////////////////////////////////////////////////////////
// movingAverage.h
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

#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include "typeDef.h"


//===============================================================================
//  Constants
//===============================================================================
#define MA_BUF_LEN      (8U)

// Retern Value
#define MA_RC_OK        (0)     // OK
#define MA_RC_ERR_LEN   (-1)    // Incorrect Length Value


//===============================================================================
//  Type Definition
//===============================================================================
// Moving Average Parameter of uint8_t data
typedef struct {
    uint8_t   buffer[MA_BUF_LEN]; // Buffer of uint8_t data
    int16_t   sum;                // Sum of values within the buffer
    uint8_t   pos;                // Index of the oldest data
    uint8_t   len;                // Moving Average Length
    uint8_t   num;                // Number of valid data
} MAParamU8;

// Moving Average Parameter of float32_t data
typedef struct {
    float32_t buffer[MA_BUF_LEN]; // Buffer of float32_t data
    float64_t sum;                // Sum of values within the buffer
    uint8_t   pos;                // Index of the oldest data
    uint8_t   len;                // Moving Average Length
    uint8_t   num;                // Number of valid data
} MAParamF;


//===============================================================================
//  Extern Global Variables
//===============================================================================


//===============================================================================
//  Public Function Prototypes
//===============================================================================
// Moving Average of uint8_t data
int8_t    ma_InitU8(uint8_t len, MAParamU8 *prm);
uint8_t   ma_AverageU8(uint8_t data, MAParamU8 *prm);
void      ma_ClearU8(MAParamU8 *prm);

// Moving Average of float32_t data
int8_t    ma_InitF(uint8_t len, MAParamF *prm);
float32_t ma_AverageF(float32_t data, MAParamF *prm);
void      ma_ClearF(MAParamF *prm);



//===============================================================================
//  Macro
//===============================================================================



#endif  // MOVING_AVERAGE_H
