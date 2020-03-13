/////////////////////////////////////////////////////////////////////////////////
// pw_bh1790.c
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


//  Global Variables


//  Static Variables
static uint8_t is_measured;    // Measurement flag

static uint8_t s_pw_freq;      // LED_LIGHTING_FREQ parameter
static uint8_t s_pw_rCycle;    // RCYCLE            parameter
static uint8_t s_pw_cur;       // LED_CURRENT       parameter
static uint8_t s_pw_onTime;    // LED_ON_TIME       parameter
static uint8_t s_pw_en;        // LED_EN            parameter


// Local Function Prototype
static uint16_t errorCode_8toU16(int8_t code);


/////////////////////////////////////////////////////////////////////////////////
//  Public Functions
/////////////////////////////////////////////////////////////////////////////////

//===============================================================================
// @brief  Initialize Pulse Wave Sensor
// Initialize BH1790, and set the default configuration parameters.
// 
// @param[in]    : None
// @param[out]   : None
// @param[inout] : None
// @retval       : uint16_t
//                   ERROR_NONE              => No error
//                   ERROR_PW_EXIST_SENSOR   => No PW Sensor exists
//                   ERROR_PW_I2C            => I2C error with PW Sensor
//===============================================================================
uint16_t pw_Init(void)
{
    int8_t   ret8  = BH1790_RC_OK;
    uint16_t ret16 = ERROR_NONE;

    is_measured  = 0U;
    s_pw_rCycle  = BH1790_PRM_CTRL1_RCYCLE_32HZ;
    s_pw_freq    = BH1790_PRM_CTRL1_FREQ_128HZ;
    s_pw_cur     = BH1790_PRM_CTRL2_CUR_0MA;
    s_pw_onTime  = BH1790_PRM_CTRL2_ONTIME_0_3MS;
    s_pw_en      = BH1790_PRM_CTRL2_EN_NONE;

    ret8  = bh1790_Init();
    ret16 = errorCode_8toU16(ret8);
    
    return (ret16);
}

//===============================================================================
// @brief Start Measurement of Pulse Wave
// 
// @param[in]    : None
// @param[out]   : None
// @param[inout] : None
// @retval       : uint16_t
//                   ERROR_NONE              => No error
//                   ERROR_PW_I2C            => I2C error with PW Sensor
//===============================================================================
uint16_t pw_StartMeasure(void)
{
    int8_t     ret8  = BH1790_RC_OK;
    uint16_t   ret16 = ERROR_NONE;
    uint8_t    reg[3];
    MEAS_CTRL1 ctrl1 = { s_pw_rCycle, s_pw_freq, 0, BH1790_PRM_CTRL1_RDY };
    MEAS_CTRL2 ctrl2 = { s_pw_cur, 0, s_pw_onTime, s_pw_en };
    MEAS_START start = { BH1790_PRM_MEAS_ST, 0 };
    
    ret8 = bh1790_SoftReset();
    if (ret8 == BH1790_RC_OK) {
        // BH1790_MEAS_CTRL1 Config Parameters
        reg[0] = *(uint8_t *)&ctrl1;
        
        // BH1790_MEAS_CTRL2 Config Parameters
        reg[1] = *(uint8_t *)&ctrl2;
        
        // BH1790_MEAS_START Config Parameters
        reg[2] = *(uint8_t *)&start;
        
        // Command Send
        ret8 = bh1790_Write(BH1790_MEAS_CTRL1, reg, sizeof(reg));
        if (ret8 == BH1790_RC_OK) {
            is_measured = 1U;
        }
    }
    
    ret16 = errorCode_8toU16(ret8);
    
    return (ret16);
}

//===============================================================================
// @brief Stop Measurement of Pulse Wave
// 
// @param[in]    : None
// @param[out]   : None
// @param[inout] : None
// @retval       : uint16_t
//                   ERROR_NONE              => No error
//                   ERROR_PW_I2C            => I2C error with PW Sensor
//===============================================================================
uint16_t pw_StopMeasure(void)
{
    int8_t   ret8  = BH1790_RC_OK;
    uint16_t ret16 = ERROR_NONE;

    ret8 = bh1790_SoftReset();
    if (ret8 == BH1790_RC_OK) {
        is_measured = 0U;
    }

    ret16 = errorCode_8toU16(ret8);
    
    return (ret16);
}

//===============================================================================
// @brief  Read Measurement Result
// Need to read repeatedly in a period for RCYCLE parameter.
// 
// @param[in]    : None
// @param[out]   : u16_pair_t *data          => Pointer to BH1790 Raw Data
// @param[inout] : None
// @retval       : uint16_t
//                   ERROR_NONE              => No error
//                   ERROR_PW_I2C            => I2C error with PW Sensor
//                   ERROR_PW_NOT_MEASURE    => Must start the measurement
//===============================================================================
uint16_t pw_GetMeasureData(u16_pair_t *data)
{
    int8_t   ret8  = BH1790_RC_OK;
    uint16_t ret16 = ERROR_NONE;
    uint16_t  reg[2];
    
    if (is_measured == 0U) {
        ret16 = ERROR_PW_NOT_MEASURE;
    }
    else {
        ret8 = bh1790_Read(BH1790_DATAOUT_LEDOFF_LSBS, (uint8_t *)reg, sizeof(reg));
        if (ret8 == BH1790_RC_OK){
            data->off = reg[0];
            data->on  = reg[1];
        }
        
        ret16 = errorCode_8toU16(ret8);
    }
    
    return (ret16);
}

//===============================================================================
// @brief Set Pulse Wave parameters
// 
// @param[in]    : uint8_t type              => Parameter Type
//                 uint8_t value             => Parameter Value
// @param[out]   : None
// @param[inout] : None
// @retval       : uint16_t
//                   System Error Code for BH1790
//===============================================================================
uint16_t pw_SetParam(uint8_t type, uint8_t value)
{
    uint16_t ret16 = ERROR_NONE;
    
    if (is_measured == 1U) {
        ret16 = ERROR_PW_MEASURING;
    }
    else {
        switch(type) {
            case BH1790_PRM_CTRL1_FREQ:
                if (value > BH1790_PRM_CTRL1_FREQ_64HZ) {
                    ret16 = ERROR_BH1790_PRM_CTRL1_FREQ;
                }
                else {
                    s_pw_freq = value;
                }
                break;
            case BH1790_PRM_CTRL1_RCYCLE:
                if ((value < BH1790_PRM_CTRL1_RCYCLE_64HZ) || (value > BH1790_PRM_CTRL1_RCYCLE_32HZ)) {
                    ret16 =  ERROR_BH1790_PRM_CTRL1_RCYCLE;
                }
                else {
                    s_pw_rCycle = value;
                }
                break;
            case BH1790_PRM_CTRL2_EN:
                if (value > BH1790_PRM_CTRL2_EN_LED1_LED2) {
                    ret16 =  ERROR_BH1790_PRM_CTRL2_EN;
                }
                else {
                    s_pw_en = value;
                }
                break;
            case BH1790_PRM_CTRL2_ONTIME:
                if (value > BH1790_PRM_CTRL2_ONTIME_0_6MS) {
                    ret16 =  ERROR_BH1790_PRM_CTRL2_ONTIME;
                }
                else {
                    s_pw_onTime = value;
                }
                break;
            case BH1790_PRM_CTRL2_CUR:
                if (((value > BH1790_PRM_CTRL2_CUR_0MA) && (value < BH1790_PRM_CTRL2_CUR_1MA)) || (value > BH1790_PRM_CTRL2_CUR_60MA)) {
                    ret16 =  ERROR_BH1790_PRM_CTRL2_CUR;
                }
                else {
                    s_pw_cur = value;
                }
                break;
            default:
                ret16 = ERROR_BH1790_PRM_TYPE;
                break;
        }
    }
    
    return (ret16);
}

//===============================================================================
// @brief Get Pulse Wave parameters
// 
// @param[in]    : uint8_t type              => Parameter Type
//                 void    *value            => Pointer to Parameter Value
// @param[out]   : None
// @param[inout] : None
// @retval       : uint16_t
//                   System Error Code for BH1790
//===============================================================================
uint16_t pw_GetParam(uint8_t type, uint8_t *value)
{
    uint16_t ret16 = ERROR_NONE;
    
    switch(type) {
        case BH1790_PRM_CTRL1_FREQ:
            *value = s_pw_freq;
            break;
        case BH1790_PRM_CTRL1_RCYCLE:
            *value = s_pw_rCycle;
            break;
        case BH1790_PRM_CTRL2_CUR:
            *value = s_pw_cur;
            break;
        case BH1790_PRM_CTRL2_ONTIME:
            *value = s_pw_onTime;
            break;
        case BH1790_PRM_CTRL2_EN:
            *value = s_pw_en;
            break;
        default:
            ret16 = ERROR_BH1790_PRM_TYPE;
            break;
    }
    
    return (ret16);
}


/////////////////////////////////////////////////////////////////////////////////
//  Local Function
/////////////////////////////////////////////////////////////////////////////////

//===============================================================================
// @brief Exchange 8bit-signed error code to 16bit-unsigned
// 
// @param[in]    : int8_t code               => BH1790 Driver Error Code
// @param[out]   : None
// @param[inout] : None
// @retval       : uint16_t
//                   System Error Code
//===============================================================================
static uint16_t errorCode_8toU16(int8_t code)
{
    uint16_t ret16 = ERROR_NONE;

    switch (code) {
        case BH1790_RC_OK:             ret16 = ERROR_NONE; break;
        case BH1790_RC_NO_EXIST:       ret16 = ERROR_PW_EXIST_SENSOR; break;
        case BH1790_RC_I2C_ERR:        ret16 = ERROR_PW_I2C; break;
        default:                       ret16 = ERROR_PW_UNKNOWN; break;
    }

    return (ret16);
}
