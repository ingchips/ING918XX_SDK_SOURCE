/////////////////////////////////////////////////////////////////////////////////
// iirFilter.h
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

#ifndef IIR_FILTER_H
#define IIR_FILTER_H

#include "typeDef.h"


//===============================================================================
//  Constants
//===============================================================================


//===============================================================================
//  Type Definition
//===============================================================================
// IIR Filter Parameter:  Coefficient
typedef struct {
    float32_t   a0; // Coeff of Filter Input Data
    float32_t   a1; // Coeff of x1
    float32_t   a2; // Coeff of x2
    float32_t   b1; // Coeff of y1
    float32_t   b2; // Coeff of y2
} iirCoeff;

// IIR Filter Parameter:  Internal register
typedef struct {
    float32_t   x1; // Last data of Filter Input
    float32_t   x2; // Second last data of Filter Input
    float32_t   y1; // Last data of Filter Output
    float32_t   y2; // Second last data of Filter Output
} iirRegister;

// IIR Filter Parameter
typedef struct {
    iirCoeff    coeff;
    iirRegister reg;
} iirParam;


//===============================================================================
//  Extern Global Variables
//===============================================================================


//===============================================================================
//  Public Function Prototypes
//===============================================================================
void      iir_Init(const float32_t coeff[], iirParam *prm);
float32_t iir_Filter(float32_t data, iirParam *prm);
void      iir_Set(float32_t x1, float32_t x2, iirParam *prm);



//===============================================================================
//  Macro
//===============================================================================



#endif  // IIR_FILTER_H
