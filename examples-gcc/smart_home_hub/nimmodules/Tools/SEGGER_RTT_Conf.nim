## ********************************************************************
##                     SEGGER Microcontroller GmbH                     *
##                         The Embedded Experts                        *
## *********************************************************************
##                                                                     *
##             (c) 1995 - 2019 SEGGER Microcontroller GmbH             *
##                                                                     *
##        www.segger.com     Support: support@segger.com               *
##                                                                     *
## *********************************************************************
##                                                                     *
##        SEGGER RTT * Real Time Transfer for embedded targets         *
##                                                                     *
## *********************************************************************
##                                                                     *
##  All rights reserved.                                               *
##                                                                     *
##  SEGGER strongly recommends to not make any changes                 *
##  to or modify the source code of this software in order to stay     *
##  compatible with the RTT protocol and J-Link.                       *
##                                                                     *
##  Redistribution and use in source and binary forms, with or         *
##  without modification, are permitted provided that the following    *
##  condition is met:                                                  *
##                                                                     *
##  o Redistributions of source code must retain the above copyright   *
##    notice, this condition and the following disclaimer.             *
##                                                                     *
##  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
##  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
##  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
##  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
##  DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
##  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
##  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
##  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
##  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
##  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
##  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
##  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
##  DAMAGE.                                                            *
##                                                                     *
## *********************************************************************
##                                                                     *
##        RTT version: 6.80c                                           *
##                                                                     *
## *********************************************************************
##
## ---------------------------END-OF-HEADER------------------------------
## File    : SEGGER_RTT_Conf.h
## Purpose : Implementation of SEGGER real-time transfer (RTT) which
##           allows real-time communication on targets which support
##           debugger memory accesses while the CPU is running.
## Revision: $Rev: 18601 $
##
##

## ********************************************************************
##
##        Defines, configurable
##
## *********************************************************************
##

when not defined(SEGGER_RTT_MAX_NUM_UP_BUFFERS):
  const
    SEGGER_RTT_MAX_NUM_UP_BUFFERS* = (1) ##  Max. number of up-buffers (T->H) available on this target    (Default: 3)
when not defined(SEGGER_RTT_MAX_NUM_DOWN_BUFFERS):
  const
    SEGGER_RTT_MAX_NUM_DOWN_BUFFERS* = (1) ##  Max. number of down-buffers (H->T) available on this target  (Default: 3)
when not defined(BUFFER_SIZE_UP):
  const
    BUFFER_SIZE_UP* = (500)     ##  Size of the buffer for terminal output of target, up to host (Default: 1k)
when not defined(BUFFER_SIZE_DOWN):
  const
    BUFFER_SIZE_DOWN* = (16)    ##  Size of the buffer for terminal input to target from host (Usually keyboard input) (Default: 16)
when not defined(SEGGER_RTT_PRINTF_BUFFER_SIZE):
  const
    SEGGER_RTT_PRINTF_BUFFER_SIZE* = (64) ##  Size of buffer for RTT printf to bulk-send chars via RTT     (Default: 64)
## ********************************************************************
##
##        RTT memcpy configuration
##
##        memcpy() is good for large amounts of data,
##        but the overhead is big for small amounts, which are usually stored via RTT.
##        With SEGGER_RTT_MEMCPY_USE_BYTELOOP a simple byte loop can be used instead.
##
##        SEGGER_RTT_MEMCPY() can be used to replace standard memcpy() in RTT functions.
##        This is may be required with memory access restrictions,
##        such as on Cortex-A devices with MMU.
##

when not defined(SEGGER_RTT_MEMCPY_USE_BYTELOOP):
  const
    SEGGER_RTT_MEMCPY_USE_BYTELOOP* = 0
##
##  Example definition of SEGGER_RTT_MEMCPY to external memcpy with GCC toolchains and Cortex-A targets
##
## #if ((defined __SES_ARM) || (defined __CROSSWORKS_ARM) || (defined __GNUC__)) && (defined (__ARM_ARCH_7A__))
##   #define SEGGER_RTT_MEMCPY(pDest, pSrc, NumBytes)      SEGGER_memcpy((pDest), (pSrc), (NumBytes))
## #endif
##
##  Target is not allowed to perform other RTT operations while string still has not been stored completely.
##  Otherwise we would probably end up with a mixed string in the buffer.
##  If using  RTT from within interrupts, multiple tasks or multi processors, define the SEGGER_RTT_LOCK() and SEGGER_RTT_UNLOCK() function here.
##
##  SEGGER_RTT_MAX_INTERRUPT_PRIORITY can be used in the sample lock routines on Cortex-M3/4.
##  Make sure to mask all interrupts which can send RTT data, i.e. generate SystemView events, or cause task switches.
##  When high-priority interrupts must not be masked while sending RTT data, SEGGER_RTT_MAX_INTERRUPT_PRIORITY needs to be adjusted accordingly.
##  (Higher priority = lower priority number)
##  Default value for embOS: 128u
##  Default configuration in FreeRTOS: configMAX_SYSCALL_INTERRUPT_PRIORITY: ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
##  In case of doubt mask all interrupts: 1 << (8 - BASEPRI_PRIO_BITS) i.e. 1 << 5 when 3 bits are implemented in NVIC
##  or define SEGGER_RTT_LOCK() to completely disable interrupts.
##

when not defined(SEGGER_RTT_MAX_INTERRUPT_PRIORITY):
  const
    SEGGER_RTT_MAX_INTERRUPT_PRIORITY* = (0x00000020) ##  Interrupt priority to lock on SEGGER_RTT_LOCK on Cortex-M3/4 (Default: 0x20)
## ********************************************************************
##
##        RTT lock configuration fallback
##

when not defined(SEGGER_RTT_LOCK):
  template SEGGER_RTT_LOCK*(): void =
    ##  Lock RTT (nestable)   (i.e. disable interrupts)
    nil

when not defined(SEGGER_RTT_UNLOCK):
  template SEGGER_RTT_UNLOCK*(): void =
    ##  Unlock RTT (nestable) (i.e. enable previous interrupt lock state)
    nil

## ************************** End of file ***************************
