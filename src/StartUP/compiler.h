/**
 ****************************************************************************************
 *
 * @file compiler.h
 *
 * @brief Definitions of compiler specific directives.
 *
 * Copyright (C) INGCHIPS 2018
 *
 *
 ****************************************************************************************
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <stdint.h>

#define ARM_CORE

#if   defined ( __CC_ARM )
  #define __ASM            __asm                                      /*!< asm keyword for ARM Compiler          */
  #define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler       */
  #define __STATIC_INLINE  static __inline
  #if ((__TARGET_ARCH_ARM >= 6) || (__TARGET_ARCH_ARM == 0))
    #define CPU_SUPPORT_UNALIGNED_ACCESS
  #endif

#elif defined ( __ICCARM__ )
  #define __ASM            __asm                                      /*!< asm keyword for IAR Compiler          */
  #define __INLINE         inline                                     /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */
  #define __STATIC_INLINE  static inline

#elif defined ( __GNUC__ )
  #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
  #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
  #define __STATIC_INLINE  static inline
  #define __weak            __attribute__((weak))
#endif

#endif // _COMPILER_H_
