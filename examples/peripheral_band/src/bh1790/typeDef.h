/////////////////////////////////////////////////////////////////////////////////
// typeDef.h
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

#ifndef TYPE_DEF_H
#define TYPE_DEF_H

#include <stdint.h>


//===============================================================================
//  Constants
//===============================================================================


//===============================================================================
//  Type Definition
//===============================================================================
//  Basic Type
typedef unsigned char       uint8_t;     // 8bit  unsigned integer type
typedef signed char         int8_t;      // 8bit    signed integer type
typedef unsigned long long  uint64_t;    // 64bit unsigned integer type
typedef signed long long    int64_t;     // 64bit   signed integer type
typedef float               float32_t;   // 32bit floating point number type
typedef double              float64_t;   // 64bit floating point number type

// A data structure that store a pair of uint16_t data
typedef struct {
    uint16_t on;
    uint16_t off;
} u16_pair_t;


//===============================================================================
//  Extern Global Variables
//===============================================================================


//===============================================================================
//  Public Function Prototypes
//===============================================================================


//===============================================================================
//  Macro
//===============================================================================



#endif  // TYPE_DEF_H
