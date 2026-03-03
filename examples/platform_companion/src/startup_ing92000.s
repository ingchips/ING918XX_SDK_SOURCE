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

VER_MAJOR   EQU  20
VER_MINOR   EQU  3
VER_PATCH   EQU  67

Stack_Size      EQU     0x00000200

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
				EXPORT  __PLATFORM_VER
                EXPORT  __APP_LOAD_ADD
                EXPORT  __ALL_END
                EXPORT  __PATCH_ADD
				IMPORT  platform_patches

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
				EXPORT n00_IRQHandler  [WEAK] ;
				EXPORT n01_IRQHandler  [WEAK] ;
				EXPORT n02_IRQHandler  [WEAK] ;
				EXPORT n03_IRQHandler  [WEAK] ;
				EXPORT n04_IRQHandler  [WEAK] ;
				EXPORT n05_IRQHandler  [WEAK] ;
				EXPORT n06_IRQHandler  [WEAK] ;
				EXPORT n07_IRQHandler  [WEAK] ;
				EXPORT n08_IRQHandler  [WEAK] ;
				EXPORT n09_IRQHandler  [WEAK] ;
				EXPORT n10_IRQHandler  [WEAK] ;
				EXPORT n11_IRQHandler  [WEAK] ;
				EXPORT n12_IRQHandler  [WEAK] ;
				EXPORT n13_IRQHandler  [WEAK] ;
				EXPORT n14_IRQHandler  [WEAK] ;
				EXPORT n15_IRQHandler  [WEAK] ;
				EXPORT n16_IRQHandler  [WEAK] ;
				EXPORT n17_IRQHandler  [WEAK] ;
				EXPORT n18_IRQHandler  [WEAK] ;
				EXPORT n19_IRQHandler  [WEAK] ;
				EXPORT n20_IRQHandler  [WEAK] ;
				EXPORT n21_IRQHandler  [WEAK] ;
				EXPORT n22_IRQHandler  [WEAK] ;
				EXPORT n23_IRQHandler  [WEAK] ;
				EXPORT n24_IRQHandler  [WEAK] ;
				EXPORT n25_IRQHandler  [WEAK] ;
				EXPORT n26_IRQHandler  [WEAK] ;
				EXPORT n27_IRQHandler  [WEAK] ;
				EXPORT n28_IRQHandler  [WEAK] ;
				EXPORT n29_IRQHandler  [WEAK] ;
				EXPORT n30_IRQHandler  [WEAK] ;
				EXPORT n31_IRQHandler  [WEAK] ;
				EXPORT n32_IRQHandler  [WEAK] ;
				EXPORT n33_IRQHandler  [WEAK] ;
				EXPORT n34_IRQHandler  [WEAK] ;

                DCD n00_IRQHandler
                DCD n01_IRQHandler
                DCD n02_IRQHandler
                DCD n03_IRQHandler
                DCD n04_IRQHandler
                DCD n05_IRQHandler
                DCD n06_IRQHandler
                DCD n07_IRQHandler
                DCD n08_IRQHandler
                DCD n09_IRQHandler
                DCD n10_IRQHandler
                DCD n11_IRQHandler
                DCD n12_IRQHandler
                DCD n13_IRQHandler
                DCD n14_IRQHandler
                DCD n15_IRQHandler
                DCD n16_IRQHandler
                DCD n17_IRQHandler
                DCD n18_IRQHandler
                DCD n19_IRQHandler
                DCD n20_IRQHandler
                DCD n21_IRQHandler
                DCD n22_IRQHandler
                DCD n23_IRQHandler
                DCD n24_IRQHandler
                DCD n25_IRQHandler
                DCD n26_IRQHandler
                DCD n27_IRQHandler
                DCD n28_IRQHandler
                DCD n29_IRQHandler
                DCD n30_IRQHandler
                DCD n31_IRQHandler
                DCD n32_IRQHandler
                DCD n33_IRQHandler
                DCD n34_IRQHandler


__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

__PLATFORM_VER  DCD     (VER_MAJOR << 0) + (VER_MINOR << 16) + (VER_PATCH << 24)  ; Version
__APP_LOAD_ADD  DCD     0x02004000
__PATCH_ADD     DCD     platform_patches
                DCD     0xffffffff
                DCD     0xffffffff
                DCD     0xffffffff
                DCD     0xffffffff
                DCD     0xffffffff
                DCD     0xffffffff
                DCD     0xffffffff
__ALL_END

__FILLING_SIZE  EQU     EFLASH_SEC_SIZE - (__ALL_END - __Vectors)

				FILL    __FILLING_SIZE, 0xFF, 1

                ; place holder
__VECTORS_LOC   DCD     0x02002000

                AREA    |.text|, CODE, READONLY

; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler
                IMPORT  __scatterload

                LDR     R0, =0x40000008
                LDR     R1, [R0]
                ORR     R1, R1, #0x2
                STR     R1, [R0]

                LDR     R0, =0x4000001c
                LDR     R1, [R0]
                ORR     R1, R1, #0x400
                STR     R1, [R0]

POWER_CS_START_READ
                LDR      r3, =0x40100000
                MOVS     r1,#0x01
                LDR      r0,[r3,#0x54]
                UBFX     r0,r0,#12,#4
POWER_CS_READ_AGAIN
                LDR      r2,[r3,#0x54]
                UBFX     r2,r2,#12,#4
                CMP      r0,r2
                BNE      POWER_CS_START_READ
                ADDS     r1,r1,#1
                CMP      r1,#0x03
                BLT      POWER_CS_READ_AGAIN
                CMP      r0,#0x02
                BLT      POWER_CS_START_READ

                LDR     r3, =0x40110000
                LDR     r0, [r3]
                CMP     r0, #0
                BEQ     POWER_CS_START_READ

                LDR     r3, =0x40000044
                LDR     r0, [r3]
                ORR     r0, r0, #0x10000; (1ul << 16)
                STR     r0, [r3]

                LDR     R0, =__scatterload
                BX      R0

                ENDP

                ALIGN

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC
        EXPORT n00_IRQHandler [WEAK] ;
        EXPORT n01_IRQHandler [WEAK] ;
        EXPORT n02_IRQHandler [WEAK] ;
        EXPORT n03_IRQHandler [WEAK] ;
        EXPORT n04_IRQHandler [WEAK] ;
        EXPORT n05_IRQHandler [WEAK] ;
        EXPORT n06_IRQHandler [WEAK] ;
        EXPORT n07_IRQHandler [WEAK] ;
        EXPORT n08_IRQHandler [WEAK] ;
        EXPORT n09_IRQHandler [WEAK] ;
        EXPORT n10_IRQHandler [WEAK] ;
        EXPORT n11_IRQHandler [WEAK] ;
        EXPORT n12_IRQHandler [WEAK] ;
        EXPORT n13_IRQHandler [WEAK] ;
        EXPORT n14_IRQHandler [WEAK] ;
        EXPORT n15_IRQHandler [WEAK] ;
        EXPORT n16_IRQHandler [WEAK] ;
        EXPORT n17_IRQHandler [WEAK] ;
        EXPORT n18_IRQHandler [WEAK] ;
        EXPORT n19_IRQHandler [WEAK] ;
        EXPORT n20_IRQHandler [WEAK] ;
        EXPORT n21_IRQHandler [WEAK] ;
        EXPORT n22_IRQHandler [WEAK] ;
        EXPORT n23_IRQHandler [WEAK] ;
        EXPORT n24_IRQHandler [WEAK] ;
        EXPORT n25_IRQHandler [WEAK] ;
        EXPORT n26_IRQHandler [WEAK] ;
        EXPORT n27_IRQHandler [WEAK] ;
        EXPORT n28_IRQHandler [WEAK] ;
        EXPORT n29_IRQHandler [WEAK] ;
        EXPORT n30_IRQHandler [WEAK] ;
        EXPORT n31_IRQHandler [WEAK] ;
        EXPORT n32_IRQHandler [WEAK] ;
        EXPORT n33_IRQHandler [WEAK] ;
        EXPORT n34_IRQHandler [WEAK] ;

n00_IRQHandler
n01_IRQHandler
n02_IRQHandler
n03_IRQHandler
n04_IRQHandler
n05_IRQHandler
n06_IRQHandler
n07_IRQHandler
n08_IRQHandler
n09_IRQHandler
n10_IRQHandler
n11_IRQHandler
n12_IRQHandler
n13_IRQHandler
n14_IRQHandler
n15_IRQHandler
n16_IRQHandler
n17_IRQHandler
n18_IRQHandler
n19_IRQHandler
n20_IRQHandler
n21_IRQHandler
n22_IRQHandler
n23_IRQHandler
n24_IRQHandler
n25_IRQHandler
n26_IRQHandler
n27_IRQHandler
n28_IRQHandler
n29_IRQHandler
n30_IRQHandler
n31_IRQHandler
n32_IRQHandler
n33_IRQHandler
n34_IRQHandler

                B       .
                ENDP

				END
