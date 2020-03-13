/////////////////////////////////////////////////////////////////////////////////
// bh1790.h
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

#ifndef BH1790_H
#define BH1790_H

#include "typeDef.h"


//===============================================================================
//  Constants
//===============================================================================
// Retern Value
#define BH1790_RC_OK                   (0)      // No error
#define BH1790_RC_NO_EXIST             (-1)     // No BH1790 exists
#define BH1790_RC_I2C_ERR              (-2)     // I2C error with BH1790

// Slave Address
#define BH1790_SLAVE_ADDRESS           (0x5BU)  // Slave Address

// Register Address
#define BH1790_PARTID                  (0x10U)  // R   : PART ID
#define BH1790_RESET                   (0x40U)  // W   : RESET
#define BH1790_MEAS_CTRL1              (0x41U)  // R/W : Measurement Control1
#define BH1790_MEAS_CTRL2              (0x42U)  // R/W : Measurement Control2
#define BH1790_MEAS_START              (0x43U)  // R/W : Measurement Start
#define BH1790_DATAOUT_LEDOFF_LSBS     (0x54U)  // R   : Low  byte of DATAOUT LEDOFF
#define BH1790_DATAOUT_LEDOFF_MSBS     (0x55U)  // R   : High byte of DATAOUT LEDOFF
#define BH1790_DATAOUT_LEDON_LSBS      (0x56U)  // R   : Low  byte of DATAOUT LEDON
#define BH1790_DATAOUT_LEDON_MSBS      (0x57U)  // R   : High byte of DATAOUT LEDON
#define BH1790_MANUFACTURERID          (0x92U)  // R   : Manufacturer ID

// Register Value
#define BH1790_PARTID_VAL              (0x0DU)
#define BH1790_MANUFACTURERID_VAL      (0xE0U)

// Config Parameters
// BH1790_RESET
// 7bit: SWRESET
#define BH1790_PRM_SWRESET             (0x01U)  // Software reset is performed

// BH1790_MEAS_CTRL1
// 7bit: RDY
#define BH1790_PRM_CTRL1_RDY           (0x01U)  // OSC block is active
// 2bit: LED_LIGHTING_FREQ
// Select LED emitting frequency
#define BH1790_PRM_CTRL1_FREQ_128HZ    (0x00U)
#define BH1790_PRM_CTRL1_FREQ_64HZ     (0x01U)
// 1-0bit: RCYCLE
// Select Data reading frequency
#define BH1790_PRM_CTRL1_RCYCLE_64HZ   (0x01U)
#define BH1790_PRM_CTRL1_RCYCLE_32HZ   (0x02U)

// BH1790_MEAS_CTRL2
// 7-6bit: LED_EN
// Select LED driver mode
#define BH1790_PRM_CTRL2_EN_NONE       (0x00U) // LED1: Pulsed,   LED2: Pulsed
#define BH1790_PRM_CTRL2_EN_LED1       (0x01U) // LED1: Constant, LED2: Pulsed
#define BH1790_PRM_CTRL2_EN_LED2       (0x02U) // LED1: Pulsed,   LED2: Constant
#define BH1790_PRM_CTRL2_EN_LED1_LED2  (0x03U) // LED1: Constant, LED2: Constant
// 5bit: LED_ON_TIME
// Select LED emitting time
#define BH1790_PRM_CTRL2_ONTIME_0_3MS  (0x00U)
#define BH1790_PRM_CTRL2_ONTIME_0_6MS  (0x01U)
// 3-0bit: LED_CURRENT
// Select LED driver current
#define BH1790_PRM_CTRL2_CUR_0MA       (0x00U)
#define BH1790_PRM_CTRL2_CUR_1MA       (0x08U)
#define BH1790_PRM_CTRL2_CUR_2MA       (0x09U)
#define BH1790_PRM_CTRL2_CUR_3MA       (0x0AU)
#define BH1790_PRM_CTRL2_CUR_6MA       (0x0BU)
#define BH1790_PRM_CTRL2_CUR_10MA      (0x0CU)
#define BH1790_PRM_CTRL2_CUR_20MA      (0x0DU)
#define BH1790_PRM_CTRL2_CUR_30MA      (0x0EU)
#define BH1790_PRM_CTRL2_CUR_60MA      (0x0FU)

// BH1790_MEAS_START
// 0bit: MEAS_ST
#define BH1790_PRM_MEAS_ST             (0x01U)  // Measurement start

// Paramter Type
#define BH1790_PRM_CTRL1_FREQ          (0U)
#define BH1790_PRM_CTRL1_RCYCLE        (1U)
#define BH1790_PRM_CTRL2_EN            (2U)
#define BH1790_PRM_CTRL2_ONTIME        (3U)
#define BH1790_PRM_CTRL2_CUR           (4U)


//===============================================================================
//  Type Definition
//===============================================================================


//===============================================================================
//  Extern Global Variables
//===============================================================================


//===============================================================================
//  Public Function Prototypes
//===============================================================================
int8_t bh1790_Init(void);
int8_t bh1790_SoftReset(void);
// Please implement the I2C I/F in the following functions
int8_t bh1790_Write(uint8_t adr, uint8_t *data, uint8_t size);
int8_t bh1790_Read (uint8_t adr, uint8_t *data, uint8_t size);


//===============================================================================
//  Macro
//===============================================================================



#endif  // BH1790_H
