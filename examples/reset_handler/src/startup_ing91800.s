;/**************************************************************************//**
; * @file     startup_ARMCM3.s
; * @brief    CMSIS Core Device Startup File for
; *           ARMCM3 Device Series
; * @version  V1.08
; * @date     03. February 2012
; *
; * @note
; * Copyright (C) 2012 ARM Limited. All rights reserved.
; *
; * @par
; * ARM Limited (ARM) is supplying this software for use with Cortex-M 
; * processor based microcontrollers.  This file can be freely distributed 
; * within development tools that are supporting such ARM based processors. 
; *
; * @par
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; ******************************************************************************/
;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/

Stack_Size      EQU     0x00000000

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp
                EXPORT  __initial_sp


                EXPORT  __heap_base
                EXPORT  __heap_limit
                
Heap_Size       EQU     0

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors

__Vectors       DCD     Reset_Handler             ; Reset Handler
                
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors


                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler

                LDR  R1, =0x40070000    ; AHB_SYSCTRL->SYSCTRL_ClkGate |= (1 << 6);
                LDR  R0, [R1,#0x04]
                ORR  R0, R0,#0x40
                STR  R0, [R1,#0x04]

                LDR  R1, =0x40001000    ; R1 = APB_TMR0
                MOVS R0, #0x02          ; APB_TMR0->CTL = (1 << bsTMR_CTL_RELOAD);                
                STR  R0, [R1,#0x08]
                LDR  R0, =24000000      ; APB_TMR0->CMP = TMR_CLK_FREQ;
                STR  R0, [R1,#0x04]
                LDR  R0, [R1,#0x08]     ; APB_TMR0->CTL |= (1 << bsTMR_CTL_TMR_EN) | (1 << bsTMR_CTL_WatchDog_EN);
                ORR  R0, R0,#0x21
                STR  R0, [R1,#0x08]
                MOVS R0, #0x01          ; APB_TMR0->LOCK = 1;
                STR  R0, [R1,#0x0C]
                
                LDR  R0, =0xFEED4000    ; will be modified by tool
                BX R0
                
                ENDP

                ALIGN

    END