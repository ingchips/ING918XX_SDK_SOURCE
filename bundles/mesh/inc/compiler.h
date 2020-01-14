// ----------------------------------------------------------------------------
// Copyright Message
// ----------------------------------------------------------------------------
//
// INGCHIPS confidential and proprietary.
// COPYRIGHT (c) 2018 by INGCHIPS
//
// All rights are reserved. Reproduction in whole or in part is
// prohibited without the written consent of the copyright owner.
//
// ----------------------------------------------------------------------------

#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <stdint.h>

#ifdef __CC_ARM
  #define __INLINE         __inline
#else
  #define __INLINE         inline
#endif

#define __ASM            __asm

#endif // _COMPILER_H_
