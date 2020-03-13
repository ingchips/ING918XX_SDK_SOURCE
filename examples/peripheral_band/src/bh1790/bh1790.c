/////////////////////////////////////////////////////////////////////////////////
// bh1790.c
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

#include "bh1790.h"


//  Global Variables


//  Static Variables


//  Local Function Prototypes
static int8_t bh1790_IsExist(void);


/////////////////////////////////////////////////////////////////////////////////
//  Public Functions
/////////////////////////////////////////////////////////////////////////////////

//===============================================================================
// @brief  Initialize BH1790
// Check if BH1790 is connected, and then do the soft reset.
//
// @param[in]    : None
// @param[out]   : None
// @param[inout] : None
// @retval       : int8_t
//                   BH1790_RC_OK       => OK
//                   BH1790_RC_NO_EXIST => No BH1790 exists
//                   BH1790_RC_I2C_ERR  => I2C error with BH1790
//===============================================================================
int8_t bh1790_Init(void)
{
    int8_t  ret8 = BH1790_RC_OK;
    
    ret8 = bh1790_IsExist();
    if (ret8 == BH1790_RC_OK) {
        ret8 = bh1790_SoftReset();
    }
    
    return (ret8);
}

//===============================================================================
// @brief  Soft-reset BH1790
//
// @param[in]    : None
// @param[out]   : None
// @param[inout] : None
// @retval       : int8_t
//                   BH1790_RC_OK       => OK
//                   BH1790_RC_I2C_ERR  => I2C error with BH1790
//===============================================================================
int8_t bh1790_SoftReset(void)
{
    int8_t  ret8 = BH1790_RC_OK;
    uint8_t reg[1];
    
    reg[0] = BH1790_PRM_SWRESET << 7U;
    ret8   = bh1790_Write(BH1790_RESET, reg, 1U);
    
    return (ret8);
}

//===============================================================================
// @brief  Write data to BH1790 registers
// Need to implement I2C.
//
// @param[in]    : uint8_t adr          => Register Address
//                 uint8_t *data        => Pointer to a write data
//                 uint8_t size         => Data Size(Byte)
// @param[out]   : None
// @param[inout] : None
// @retval       : int8_t
//                   BH1790_RC_OK       => OK
//                   BH1790_RC_I2C_ERR  => I2C error with BH1790
//===============================================================================
//int8_t bh1790_Write(uint8_t adr, uint8_t *data, uint8_t size)

//===============================================================================
// @brief  Read data from BH1790 registers
// Need to implement I2C.
//
// @param[in]    : uint8_t adr          => Register Address
//                 uint8_t size         => Data Size(Byte)
// @param[out]   : uint8_t *data        => Pointer to a read data
// @param[inout] : None
// @retval       : int8_t
//                   BH1790_RC_OK       => OK
//                   BH1790_RC_I2C_ERR  => I2C error with BH1790
//===============================================================================
//int8_t bh1790_Read(uint8_t adr, uint8_t *data, uint8_t size)


/////////////////////////////////////////////////////////////////////////////////
//  Local Functions
/////////////////////////////////////////////////////////////////////////////////

//===============================================================================
// @brief  Check if BH1790 is connected using BH1790 PART ID register.
//
// @param[in]    : None
// @param[out]   : None
// @param[inout] : None
// @retval       : int8_t
//                   BH1790_RC_OK       => OK
//                   BH1790_RC_NO_EXIST => No BH1790 exists
//                   BH1790_RC_I2C_ERR  => I2C error with BH1790
//===============================================================================
static int8_t bh1790_IsExist(void)
{
    int8_t  ret8 = BH1790_RC_OK;
    uint8_t reg[1];
    
    ret8 = bh1790_Read(BH1790_PARTID, reg, 1U);
    if (ret8 == BH1790_RC_OK) {
        if (reg[0] != BH1790_PARTID_VAL) {
            ret8 = BH1790_RC_NO_EXIST;
        }
    }
    
    return (ret8);
}
