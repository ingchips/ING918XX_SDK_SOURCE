/////////////////////////////////////////////////////////////////////////////////
// iirFilter.c
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

#include <math.h>
#include "iirFilter.h"


//  Global Variables


//  Static Variables


//  Local Function Prototypes


/////////////////////////////////////////////////////////////////////////////////
//  Public Functions
/////////////////////////////////////////////////////////////////////////////////

//===============================================================================
// @brief Initialize IIR Filter
// 
// @param[in]    : const float32_t *coeff   => Pointer to IIR Coefficient
// @param[out]   : iirParam        *prm     => Pointer to IIR Parameter
// @param[inout] : None
// @retval       : void
//===============================================================================
void iir_Init(const float32_t coeff[], iirParam *prm)
{
    if (coeff == 0) {
        prm->coeff.a0 = 1.0F;
        prm->coeff.a1 = 0.0F;
        prm->coeff.a2 = 0.0F;
        prm->coeff.b1 = 0.0F;
        prm->coeff.b2 = 0.0F;
    }
    else {
        prm->coeff.a0 = coeff[0];
        prm->coeff.a1 = coeff[1];
        prm->coeff.a2 = coeff[2];
        prm->coeff.b1 = coeff[3];
        prm->coeff.b2 = coeff[4];
    }
    
    prm->reg.x1 = 0.0F;
    prm->reg.x2 = 0.0F;
    prm->reg.y1 = 0.0F;
    prm->reg.y2 = 0.0F;
}

//===============================================================================
// @brief IIR Filter
// 
// @param[in]    : float32_t data           => Filter Input Data
// @param[out]   : None
// @param[inout] : iirParam  *prm           => Pointer to IIR Parameter
// @retval       : float32_t
//                   Filter Output Data
//===============================================================================
float32_t iir_Filter(float32_t data, iirParam *prm)
{
    float32_t work1 = 0.0F;
    float32_t work2 = 0.0F;
    float32_t work  = 0.0F;
    
    work1 = (prm->coeff.a0 * data) + (prm->coeff.a1 * prm->reg.x1) + (prm->coeff.a2 * prm->reg.x2);
    work2 = (prm->coeff.b1 * prm->reg.y1) + (prm->coeff.b2 * prm->reg.y2);
    work  = work1 + work2;
    
    prm->reg.x2 = prm->reg.x1;
    prm->reg.x1 = data;
    
    prm->reg.y2 = prm->reg.y1;
    prm->reg.y1 = work;
    
    return (work);
}

//===============================================================================
// @brief Set initial values of IIR Filter
// 
// @param[in]    : float32_t x1             => IIR Internal Register x1 data
//               : float32_t x2             => IIR Internal Register x2 data
// @param[out]   : iirParam  *prm           => Pointer to IIR Parameter
// @param[inout] : None
// @retval       : void
//===============================================================================
void iir_Set(float32_t x1, float32_t x2, iirParam *prm)
{
    prm->reg.x1 = x1;
    prm->reg.x2 = x2;
    
    prm->reg.y1 = 0.0F;
    prm->reg.y2 = 0.0F;
}


/////////////////////////////////////////////////////////////////////////////////
//  Local Functions
/////////////////////////////////////////////////////////////////////////////////
