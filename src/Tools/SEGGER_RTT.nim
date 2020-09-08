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
## File    : SEGGER_RTT.h
## Purpose : Implementation of SEGGER real-time transfer which allows
##           real-time communication on targets which support debugger
##           memory accesses while the CPU is running.
## Revision: $Rev: 19464 $
## ----------------------------------------------------------------------
##

import
  SEGGER_RTT_Conf

when not defined(SEGGER_RTT_ASM): ##  defined when SEGGER_RTT.h is included from assembly file
  ## ********************************************************************
  ##
  ##        Defines, fixed
  ##
  ## *********************************************************************
  ##
  ## ********************************************************************
  ##
  ##        Types
  ##
  ## *********************************************************************
  ##
  ##
  ##  Description for a circular buffer (also called "ring buffer")
  ##  which is used as up-buffer (T->H)
  ##
  type
    SEGGER_RTT_BUFFER_UP* {.importc: "SEGGER_RTT_BUFFER_UP",
                           header: "SEGGER_RTT.h", bycopy.} = object
      sName* {.importc: "sName".}: cstring ##  Optional name. Standard names so far are: "Terminal", "SysView", "J-Scope_t4i4"
      pBuffer* {.importc: "pBuffer".}: cstring ##  Pointer to start of buffer
      SizeOfBuffer* {.importc: "SizeOfBuffer".}: cuint ##  Buffer size in bytes. Note that one byte is lost, as this implementation does not fill up the buffer in order to avoid the problem of being unable to distinguish between full and empty.
      WrOff* {.importc: "WrOff".}: cuint ##  Position of next item to be written by either target.
      RdOff* {.importc: "RdOff".}: cuint ##  Position of next item to be read by host. Must be volatile since it may be modified by host.
      Flags* {.importc: "Flags".}: cuint ##  Contains configuration flags

  ##
  ##  Description for a circular buffer (also called "ring buffer")
  ##  which is used as down-buffer (H->T)
  ##
  type
    SEGGER_RTT_BUFFER_DOWN* {.importc: "SEGGER_RTT_BUFFER_DOWN",
                             header: "SEGGER_RTT.h", bycopy.} = object
      sName* {.importc: "sName".}: cstring ##  Optional name. Standard names so far are: "Terminal", "SysView", "J-Scope_t4i4"
      pBuffer* {.importc: "pBuffer".}: cstring ##  Pointer to start of buffer
      SizeOfBuffer* {.importc: "SizeOfBuffer".}: cuint ##  Buffer size in bytes. Note that one byte is lost, as this implementation does not fill up the buffer in order to avoid the problem of being unable to distinguish between full and empty.
      WrOff* {.importc: "WrOff".}: cuint ##  Position of next item to be written by host. Must be volatile since it may be modified by host.
      RdOff* {.importc: "RdOff".}: cuint ##  Position of next item to be read by target (down-buffer).
      Flags* {.importc: "Flags".}: cuint ##  Contains configuration flags

  ##
  ##  RTT control block which describes the number of buffers available
  ##  as well as the configuration for each buffer
  ##
  ##
  type
    SEGGER_RTT_CB* {.importc: "SEGGER_RTT_CB", header: "SEGGER_RTT.h", bycopy.} = object
      acID* {.importc: "acID".}: array[16, char] ##  Initialized to "SEGGER RTT"
      MaxNumUpBuffers* {.importc: "MaxNumUpBuffers".}: cint ##  Initialized to SEGGER_RTT_MAX_NUM_UP_BUFFERS (type. 2)
      MaxNumDownBuffers* {.importc: "MaxNumDownBuffers".}: cint ##  Initialized to SEGGER_RTT_MAX_NUM_DOWN_BUFFERS (type. 2)
      aUp* {.importc: "aUp".}: array[SEGGER_RTT_MAX_NUM_UP_BUFFERS,
                                  SEGGER_RTT_BUFFER_UP] ##  Up buffers, transferring information up from target via debug probe to host
      aDown* {.importc: "aDown".}: array[SEGGER_RTT_MAX_NUM_DOWN_BUFFERS,
                                      SEGGER_RTT_BUFFER_DOWN] ##  Down buffers, transferring information down from host via debug probe to target

  ## ********************************************************************
  ##
  ##        Global data
  ##
  ## *********************************************************************
  ##
  var SEGGER_RTT* {.importc: "_SEGGER_RTT", header: "SEGGER_RTT.h".}: SEGGER_RTT_CB
  template SEGGER_RTT_HASDATA*(n: untyped): untyped =
    (SEGGER_RTT.aDown[n].WrOff - SEGGER_RTT.aDown[n].RdOff)

  template SEGGER_RTT_HASDATA_UP*(n: untyped): untyped =
    (SEGGER_RTT.aUp[n].WrOff - SEGGER_RTT.aUp[n].RdOff)

  ## ********************************************************************
  ##
  ##        RTT API functions
  ##
  ## *********************************************************************
  ##
  proc SEGGER_RTT_AllocDownBuffer*(sName: cstring; pBuffer: pointer;
                                  BufferSize: cuint; Flags: cuint): cint {.
      importc: "SEGGER_RTT_AllocDownBuffer", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_AllocUpBuffer*(sName: cstring; pBuffer: pointer; BufferSize: cuint;
                                Flags: cuint): cint {.
      importc: "SEGGER_RTT_AllocUpBuffer", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_ConfigUpBuffer*(BufferIndex: cuint; sName: cstring;
                                 pBuffer: pointer; BufferSize: cuint; Flags: cuint): cint {.
      importc: "SEGGER_RTT_ConfigUpBuffer", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_ConfigDownBuffer*(BufferIndex: cuint; sName: cstring;
                                   pBuffer: pointer; BufferSize: cuint; Flags: cuint): cint {.
      importc: "SEGGER_RTT_ConfigDownBuffer", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_GetKey*(): cint {.importc: "SEGGER_RTT_GetKey",
                                 header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_HasData*(BufferIndex: cuint): cuint {.
      importc: "SEGGER_RTT_HasData", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_HasKey*(): cint {.importc: "SEGGER_RTT_HasKey",
                                 header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_HasDataUp*(BufferIndex: cuint): cuint {.
      importc: "SEGGER_RTT_HasDataUp", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_Init*() {.importc: "SEGGER_RTT_Init", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_Read*(BufferIndex: cuint; pBuffer: pointer; BufferSize: cuint): cuint {.
      importc: "SEGGER_RTT_Read", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_ReadNoLock*(BufferIndex: cuint; pData: pointer; BufferSize: cuint): cuint {.
      importc: "SEGGER_RTT_ReadNoLock", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_SetNameDownBuffer*(BufferIndex: cuint; sName: cstring): cint {.
      importc: "SEGGER_RTT_SetNameDownBuffer", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_SetNameUpBuffer*(BufferIndex: cuint; sName: cstring): cint {.
      importc: "SEGGER_RTT_SetNameUpBuffer", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_SetFlagsDownBuffer*(BufferIndex: cuint; Flags: cuint): cint {.
      importc: "SEGGER_RTT_SetFlagsDownBuffer", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_SetFlagsUpBuffer*(BufferIndex: cuint; Flags: cuint): cint {.
      importc: "SEGGER_RTT_SetFlagsUpBuffer", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_WaitKey*(): cint {.importc: "SEGGER_RTT_WaitKey",
                                  header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_Write*(BufferIndex: cuint; pBuffer: pointer; NumBytes: cuint): cuint {.
      importc: "SEGGER_RTT_Write", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_WriteNoLock*(BufferIndex: cuint; pBuffer: pointer; NumBytes: cuint): cuint {.
      importc: "SEGGER_RTT_WriteNoLock", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_WriteSkipNoLock*(BufferIndex: cuint; pBuffer: pointer;
                                  NumBytes: cuint): cuint {.
      importc: "SEGGER_RTT_WriteSkipNoLock", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_ASM_WriteSkipNoLock*(BufferIndex: cuint; pBuffer: pointer;
                                      NumBytes: cuint): cuint {.
      importc: "SEGGER_RTT_ASM_WriteSkipNoLock", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_WriteString*(BufferIndex: cuint; s: cstring): cuint {.
      importc: "SEGGER_RTT_WriteString", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_WriteWithOverwriteNoLock*(BufferIndex: cuint; pBuffer: pointer;
      NumBytes: cuint) {.importc: "SEGGER_RTT_WriteWithOverwriteNoLock",
                       header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_PutChar*(BufferIndex: cuint; c: char): cuint {.
      importc: "SEGGER_RTT_PutChar", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_PutCharSkip*(BufferIndex: cuint; c: char): cuint {.
      importc: "SEGGER_RTT_PutCharSkip", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_PutCharSkipNoLock*(BufferIndex: cuint; c: char): cuint {.
      importc: "SEGGER_RTT_PutCharSkipNoLock", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_GetAvailWriteSpace*(BufferIndex: cuint): cuint {.
      importc: "SEGGER_RTT_GetAvailWriteSpace", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_GetBytesInBuffer*(BufferIndex: cuint): cuint {.
      importc: "SEGGER_RTT_GetBytesInBuffer", header: "SEGGER_RTT.h".}
  ## ********************************************************************
  ##
  ##        RTT transfer functions to send RTT data via other channels.
  ##
  ## *********************************************************************
  ##
  proc SEGGER_RTT_ReadUpBuffer*(BufferIndex: cuint; pBuffer: pointer;
                               BufferSize: cuint): cuint {.
      importc: "SEGGER_RTT_ReadUpBuffer", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_ReadUpBufferNoLock*(BufferIndex: cuint; pData: pointer;
                                     BufferSize: cuint): cuint {.
      importc: "SEGGER_RTT_ReadUpBufferNoLock", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_WriteDownBuffer*(BufferIndex: cuint; pBuffer: pointer;
                                  NumBytes: cuint): cuint {.
      importc: "SEGGER_RTT_WriteDownBuffer", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_WriteDownBufferNoLock*(BufferIndex: cuint; pBuffer: pointer;
                                        NumBytes: cuint): cuint {.
      importc: "SEGGER_RTT_WriteDownBufferNoLock", header: "SEGGER_RTT.h".}
  ## ********************************************************************
  ##
  ##        RTT "Terminal" API functions
  ##
  ## *********************************************************************
  ##
  proc SEGGER_RTT_SetTerminal*(TerminalId: cuchar): cint {.
      importc: "SEGGER_RTT_SetTerminal", header: "SEGGER_RTT.h".}
  proc SEGGER_RTT_TerminalOut*(TerminalId: cuchar; s: cstring): cint {.
      importc: "SEGGER_RTT_TerminalOut", header: "SEGGER_RTT.h".}
  ## ********************************************************************
  ##
  ##        RTT printf functions (require SEGGER_RTT_printf.c)
  ##
  ## *********************************************************************
  ##
  proc SEGGER_RTT_printf*(BufferIndex: cuint; sFormat: cstring): cint {.varargs,
      importc: "SEGGER_RTT_printf", header: "SEGGER_RTT.h".}
## ********************************************************************
##
##        Defines
##
## *********************************************************************
##
##
##  Operating modes. Define behavior if buffer is full (not enough space for entire message)
##

const
  SEGGER_RTT_MODE_NO_BLOCK_SKIP* = (0) ##  Skip. Do not block, output nothing. (Default)
  SEGGER_RTT_MODE_NO_BLOCK_TRIM* = (1) ##  Trim: Do not block, output as much as fits.
  SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL* = (2) ##  Block: Wait until there is space in the buffer.
  SEGGER_RTT_MODE_MASK* = (3)

when not defined(SEGGER_RTT_MODE_DEFAULT):
  const
    SEGGER_RTT_MODE_DEFAULT* = SEGGER_RTT_MODE_NO_BLOCK_SKIP
##
##  Control sequences, based on ANSI.
##  Can be used to control color, and clear the screen
##

const
  RTT_CTRL_RESET* = "\e[0m"
  RTT_CTRL_CLEAR* = "\e[2J"
  RTT_CTRL_TEXT_BLACK* = "\e[2;30m"
  RTT_CTRL_TEXT_RED* = "\e[2;31m"
  RTT_CTRL_TEXT_GREEN* = "\e[2;32m"
  RTT_CTRL_TEXT_YELLOW* = "\e[2;33m"
  RTT_CTRL_TEXT_BLUE* = "\e[2;34m"
  RTT_CTRL_TEXT_MAGENTA* = "\e[2;35m"
  RTT_CTRL_TEXT_CYAN* = "\e[2;36m"
  RTT_CTRL_TEXT_WHITE* = "\e[2;37m"
  RTT_CTRL_TEXT_BRIGHT_BLACK* = "\e[1;30m"
  RTT_CTRL_TEXT_BRIGHT_RED* = "\e[1;31m"
  RTT_CTRL_TEXT_BRIGHT_GREEN* = "\e[1;32m"
  RTT_CTRL_TEXT_BRIGHT_YELLOW* = "\e[1;33m"
  RTT_CTRL_TEXT_BRIGHT_BLUE* = "\e[1;34m"
  RTT_CTRL_TEXT_BRIGHT_MAGENTA* = "\e[1;35m"
  RTT_CTRL_TEXT_BRIGHT_CYAN* = "\e[1;36m"
  RTT_CTRL_TEXT_BRIGHT_WHITE* = "\e[1;37m"
  RTT_CTRL_BG_BLACK* = "\e[24;40m"
  RTT_CTRL_BG_RED* = "\e[24;41m"
  RTT_CTRL_BG_GREEN* = "\e[24;42m"
  RTT_CTRL_BG_YELLOW* = "\e[24;43m"
  RTT_CTRL_BG_BLUE* = "\e[24;44m"
  RTT_CTRL_BG_MAGENTA* = "\e[24;45m"
  RTT_CTRL_BG_CYAN* = "\e[24;46m"
  RTT_CTRL_BG_WHITE* = "\e[24;47m"
  RTT_CTRL_BG_BRIGHT_BLACK* = "\e[4;40m"
  RTT_CTRL_BG_BRIGHT_RED* = "\e[4;41m"
  RTT_CTRL_BG_BRIGHT_GREEN* = "\e[4;42m"
  RTT_CTRL_BG_BRIGHT_YELLOW* = "\e[4;43m"
  RTT_CTRL_BG_BRIGHT_BLUE* = "\e[4;44m"
  RTT_CTRL_BG_BRIGHT_MAGENTA* = "\e[4;45m"
  RTT_CTRL_BG_BRIGHT_CYAN* = "\e[4;46m"
  RTT_CTRL_BG_BRIGHT_WHITE* = "\e[4;47m"

## ************************** End of file ***************************
