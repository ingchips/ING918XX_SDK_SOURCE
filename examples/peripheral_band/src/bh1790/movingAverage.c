/////////////////////////////////////////////////////////////////////////////////
// movingAverage.c
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

#include "movingAverage.h"


//  Global Variables


//  Static Variables


//  Local Function Prototypes


/////////////////////////////////////////////////////////////////////////////////
//  Public Functions
/////////////////////////////////////////////////////////////////////////////////

//===============================================================================
// @brief Initialize Moving Average of uint8_t data
// 
// @param[in]    : uint8_t   len            => Move Average Length
// @param[out]   : None
// @param[inout] : MAParamU8 *prm           => Pointer to Move Average Parameter
//                                             of uint8_t data
// @retval       : int8_t
//                   MA_RC_OK               => OK
//                   MA_RC_ERR_LEN          => Incorrect Length Value
//===============================================================================
int8_t ma_InitU8(uint8_t len, MAParamU8 *prm)
{
    uint8_t i    = 0U;
    int8_t  ret8 = MA_RC_OK;
    
    if ((len == 0U) || (len > MA_BUF_LEN)) {
        ret8 = MA_RC_ERR_LEN;
    }
    else {
        prm->len = len;
        prm->sum = 0;
        for (i = 0U; i < MA_BUF_LEN; i++) {
            prm->buffer[i] = 0U;
        }
        prm->pos = 0U;
        prm->num = 0U;
    }
    
    return (ret8);
}

//===============================================================================
// @brief Calculate Moving Average of uint8_t data
// 
// @param[in]    : uint8_t data             => Input Data to Moving Average
// @param[out]   : None
// @param[inout] : MAParamU8 *prm           => Pointer to Move Average Parameter
//                                             of uint8_t data
// @retval       : uint8_t
//                   Move Average Value
//===============================================================================
uint8_t ma_AverageU8(uint8_t data, MAParamU8 *prm)
{
    int16_t ans = 0;
    
    if (prm->num == prm->len) {
        prm->sum -= (int16_t)prm->buffer[prm->pos];
    }
    prm->sum += (int16_t)data;
    prm->buffer[prm->pos] = data;
    
    (prm->pos)++;
    if (prm->pos >= prm->len) {
        prm->pos = 0U;
    }
    if (prm->num < prm->len) {
        (prm->num)++;
    }
    
    if(prm->num > 0U) {
        ans = prm->sum / (int16_t)prm->num;
    }
    
    return ((uint8_t)ans);
}

//===============================================================================
// @brief Clear Moving Average of uint8_t data
// 
// @param[in]    : None
// @param[out]   : MAParamU8 *prm           => Pointer to Move Average Parameter
//                                             of uint8_t data
// @param[inout] : None
// @retval       : void
//===============================================================================
void ma_ClearU8(MAParamU8 *prm)
{
    prm->sum = 0;
    prm->pos = 0U;
    prm->num = 0U;
}

//===============================================================================
// @brief Initialize Moving Average of float32_t data
// 
// @param[in]    : uint8_t  len             => Move Average Length
// @param[out]   : None
// @param[inout] : MAParamF *prm            => Pointer to Move Average Parameter
//                                             of float32_t data
// @retval       : int8_t
//                   MA_RC_OK               => OK
//                   MA_RC_ERR_LEN          => Incorrect Length Value
//===============================================================================
int8_t ma_InitF(uint8_t len, MAParamF *prm)
{
    uint8_t i    = 0U;
    int8_t  ret8 = MA_RC_OK;
    
    if ((len == 0U) || (len > MA_BUF_LEN)) {
        ret8 = MA_RC_ERR_LEN;
    }
    else {
        prm->len = len;
        prm->sum = 0.0F;
        for (i = 0U; i < MA_BUF_LEN; i++) {
            prm->buffer[i] = 0.0F;
        }
        prm->pos = 0U;
        prm->num = 0U;
    }
    
    return (ret8);
}

//===============================================================================
// @brief Calculate Moving Average of float32_t data
// 
// @param[in]    : float32_t data            => Input Data to Moving Average
// @param[out]   : None
// @param[inout] : MAParamF  *prm            => Pointer to Move Average Parameter
//                                             of float32_t data
// @retval       : float32_t
//                   Move Average Value
//===============================================================================
float32_t ma_AverageF(float32_t data, MAParamF *prm)
{
    float64_t ans = 0.0F;
    
    if (prm->num == prm->len) {
        prm->sum -= prm->buffer[prm->pos];
    }
    prm->sum += data;
    prm->buffer[prm->pos] = data;
    
    (prm->pos)++;
    if (prm->pos >= prm->len) {
        prm->pos = 0U;
    }
    if (prm->num < prm->len) {
        (prm->num)++;
    }
    
    if (prm->num > 0U) {
        ans = prm->sum / (float64_t)prm->num;
    }
    
    return ((float32_t)ans);
}

//===============================================================================
// @brief Clear Moving Average of float32_t data
// 
// @param[in]    : None
// @param[out]   : MAParamF *prm            => Pointer to Move Average Parameter
//                                             of float32_t data
// @param[inout] : None
// @retval       : void
//===============================================================================
void ma_ClearF(MAParamF *prm)
{
    prm->sum = 0.0F;
    prm->pos = 0U;
    prm->num = 0U;
}


/////////////////////////////////////////////////////////////////////////////////
//  Local Functions
/////////////////////////////////////////////////////////////////////////////////
