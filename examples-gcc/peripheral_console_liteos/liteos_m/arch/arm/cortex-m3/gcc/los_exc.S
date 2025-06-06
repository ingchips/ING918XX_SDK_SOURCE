/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

.syntax unified
.arch armv7-m
.thumb
.section .text


    .global  HalExcNMI
    .global  HalExcHardFault
    .global  HalExcMemFault
    .global  HalExcBusFault
    .global  HalExcUsageFault
    .global  HalExcSvcCall

    .extern HalExcHandleEntry
    .extern g_uwExcTbl
    .extern g_taskScheduled

.equ OS_FLG_BGD_ACTIVE,   0x0002

.equ OS_EXC_CAUSE_NMI,   16
.equ OS_EXC_CAUSE_HARDFAULT,   17

.equ HF_DEBUGEVT,   20
.equ HF_VECTBL,   21

.equ FLAG_ADDR_VALID,   0x10000
.equ FLAG_HWI_ACTIVE,   0x20000
.equ FLAG_NO_FLOAT,     0x10000000

.equ OS_NVIC_FSR,           0xE000ED28      //include BusFault/MemFault/UsageFault State Register
.equ OS_NVIC_HFSR,          0xE000ED2C      //HardFault State Register
.equ OS_NVIC_BFAR,          0xE000ED38
.equ OS_NVIC_MMAR,          0xE000ED34
.equ OS_NVIC_ACT_BASE,      0xE000E300
.equ OS_NVIC_SHCSRS,        0xE000ED24
.equ OS_NVIC_SHCSR_MASK,    0xC00

    .type HalExcNMI, %function
    .global HalExcNMI
HalExcNMI:
    .fnstart
    .cantunwind
    MOV  R0, #OS_EXC_CAUSE_NMI
    MOV  R1, #0
    B  osExcDispatch
    .fnend

    .type HalExcHardFault, %function
    .global HalExcHardFault
HalExcHardFault:
    .fnstart
    .cantunwind
    MOV  R0, #OS_EXC_CAUSE_HARDFAULT
    LDR  R2, =OS_NVIC_HFSR
    LDR  R2, [R2]

    MOV  R1, #HF_DEBUGEVT
    ORR  R0, R0, R1, LSL #0x8
    TST  R2, #0x80000000
    BNE  osExcDispatch     // DEBUGEVT

    AND  R0, R0 , #0x000000FF
    MOV  R1, #HF_VECTBL
    ORR  R0, R0, R1, LSL #0x8
    TST  R2, #0x00000002
    BNE  osExcDispatch     // VECTBL

    //if not DEBUGEVT and VECTBL then is FORCED
    AND  R0, R0, #0x000000FF

    LDR  R2, =OS_NVIC_FSR
    LDR  R2, [R2]

    TST  R2, #0x8000     // BFARVALID
    BNE  _HFBusFault     // BusFault

    TST  R2, #0x80       // MMARVALID
    BNE  _HFMemFault     // MemFault

    MOV  R12,#0
    B    osHFExcCommonBMU
    .fnend

    .type _HFBusFault, %function

_HFBusFault:
    .fnstart
    .cantunwind
    LDR  R1, =OS_NVIC_BFAR
    LDR  R1, [R1]
    MOV  R12, #FLAG_ADDR_VALID
    B    osHFExcCommonBMU
    .fnend

    .type _HFMemFault, %function

_HFMemFault:
    .fnstart
    .cantunwind
    LDR  R1, =OS_NVIC_MMAR
    LDR  R1, [R1]
    MOV  R12, #FLAG_ADDR_VALID
    .fnend

    .type osHFExcCommonBMU, %function
    .global osHFExcCommonBMU
osHFExcCommonBMU:
    .fnstart
    .cantunwind
    CLZ  R2, R2
    LDR  R3, =g_uwExcTbl
    ADD  R3, R3, R2
    LDRB R2, [R3]
    ORR  R0, R0, R2, LSL #0x8
    ORR  R0, R0 ,R12
    B    osExcDispatch
    .fnend

    .type HalExcSvcCall, %function
    .global HalExcSvcCall
HalExcSvcCall:
    .fnstart
    .cantunwind
    TST   LR, #0x4
    ITE   EQ
    MRSEQ R1, MSP
    BNE   _svcCallFromPsp
    B     _svcCall
_svcCallFromPsp:
#ifdef LOSCFG_SECURE
    PUSH  {R0-R12, LR}
    MOV   R0, SP
    CPSIE I
    BL    OsSyscallHandle
    CPSID I
    MRS   R12, PSP
    STM   R12, {R0-R1}
    POP   {R0-R12, LR}
    BX    LR
#endif
    MRS   R1, PSP
_svcCall:
    LDR   R0, [R1,#24]
    LDRB  R0, [R0,#-2]
    MOV   R1, #0
    B     osExcDispatch
    .fnend

    .type HalExcBusFault, %function
    .global HalExcBusFault
HalExcBusFault:
    .fnstart
    .cantunwind
    LDR  R0, =OS_NVIC_FSR
    LDR  R0, [R0]

    TST  R0, #0x8000  // BFARVALID
    BEQ  _ExcBusNoADDR
    LDR  R1, =OS_NVIC_BFAR
    LDR  R1, [R1]
    MOV  R12, #FLAG_ADDR_VALID
    AND  R0, R0, #0x1F00

    B    osExcCommonBMU
    .fnend

    .type _ExcBusNoADDR, %function
    .global _ExcBusNoADDR
_ExcBusNoADDR:
    .fnstart
    .cantunwind
    MOV  R12,#0
    B    osExcCommonBMU
    .fnend

    .type HalExcMemFault, %function
    .global HalExcMemFault
HalExcMemFault:
    .fnstart
    .cantunwind
    LDR  R0, =OS_NVIC_FSR
    LDR  R0, [R0]

    TST  R0, #0x80 // MMARVALID
    BEQ  _ExcMemNoADDR
    LDR  R1, =OS_NVIC_MMAR
    LDR  R1, [R1]
    MOV  R12, #FLAG_ADDR_VALID
    AND  R0, R0, #0x1B

    B    osExcCommonBMU
    .fnend

    .type _ExcMemNoADDR, %function
    .global _ExcMemNoADDR
_ExcMemNoADDR:
    .fnstart
    .cantunwind
    MOV  R12,#0
    B    osExcCommonBMU
    .fnend

    .type HalExcUsageFault, %function
    .global HalExcUsageFault
HalExcUsageFault:
    .fnstart
    .cantunwind
    LDR  R0, =OS_NVIC_FSR
    LDR  R0, [R0]

    MOVW R1, #0x030F
    LSL  R1, R1, #16
    AND  R0, R0, R1
    MOV  R12, #0

    .fnend

    .type osExcCommonBMU, %function
    .global osExcCommonBMU
osExcCommonBMU:
    .fnstart
    .cantunwind
    CLZ  R0, R0
    LDR  R3, =g_uwExcTbl
    ADD  R3, R3, R0
    LDRB R0, [R3]
    ORR  R0, R0, R12
    .fnend
// R0 -- EXCCAUSE(bit 16 is 1 if EXCADDR valid),  R1 -- EXCADDR

    .type osExcDispatch, %function
    .global osExcDispatch
osExcDispatch:
    .fnstart
    .cantunwind
    LDR   R2, =OS_NVIC_ACT_BASE
    MOV   R12, #8                       // R12 is hwi check loop counter
    .fnend

    .type _hwiActiveCheck, %function
    .global _hwiActiveCheck
_hwiActiveCheck:
    .fnstart
    .cantunwind
    LDR   R3, [R2]                      // R3 store active hwi register when exc
    CMP   R3, #0
    BEQ   _hwiActiveCheckNext

    // exc occurred in IRQ
    ORR   R0, R0, #FLAG_HWI_ACTIVE
    RBIT  R2, R3
    CLZ   R2, R2
    AND   R12, R12, #1
    ADD   R2, R2, R12, LSL #5               // calculate R2 (hwi number) as pid
    .fnend

    .type _ExcInMSP, %function
    .global _ExcInMSP
_ExcInMSP:
    .fnstart
    .cantunwind
    CMP   LR, #0xFFFFFFE9
    BNE   _NoFloatInMsp
    ADD   R3, R13, #104
    PUSH  {R3}
    MRS   R12, PRIMASK                  // store message-->exc: disable int?
    PUSH {R4-R12}                       // store message-->exc: {R4-R12}
    B     _handleEntry
    .fnend

    .type _NoFloatInMsp, %function
    .global _NoFloatInMsp
_NoFloatInMsp:
    .fnstart
    .cantunwind
    ADD   R3, R13, #32
    PUSH  {R3}                           // store message-->exc: MSP(R13)

    MRS   R12, PRIMASK                   // store message-->exc: disable int?
    PUSH {R4-R12}                        // store message-->exc: {R4-R12}
    ORR   R0, R0, #FLAG_NO_FLOAT
    B     _handleEntry
    .fnend

    .type _hwiActiveCheckNext, %function
    .global _hwiActiveCheckNext
_hwiActiveCheckNext:
    .fnstart
    .cantunwind
    ADD   R2, R2, #4                        // next NVIC ACT ADDR
    SUBS  R12, R12, #1
    BNE   _hwiActiveCheck

    /*NMI interrupt excption*/
    LDR   R2, =OS_NVIC_SHCSRS
    LDRH  R2,[R2]
    LDR   R3,=OS_NVIC_SHCSR_MASK
    AND   R2, R2,R3
    CMP   R2,#0
    BNE   _ExcInMSP
    // exc occurred in Task or Init or exc
    // reserved for register info from task stack

    LDR  R2, =g_taskScheduled
    LDR  R2, [R2]
    TST  R2, #1                         // OS_FLG_BGD_ACTIVE
    BEQ  _ExcInMSP                      // if exc occurred in Init then branch


    CMP   LR, #0xFFFFFFED               //auto push floating registers
    BNE   _NoFloatInPsp

    // exc occurred in Task
    MOV   R2,  R13
    SUB   R13, #96                      // add 8 Bytes reg(for STMFD)

    MRS   R3,  PSP
    ADD   R12, R3, #104
    PUSH  {R12}                         // save task SP

    MRS   R12, PRIMASK
    PUSH {R4-R12}

    // copy auto saved task register

    LDMFD R3!, {R4-R11}                 // R4-R11 store PSP reg(auto push when exc in task)
    STMFD R2!, {R4-R11}
    B     _handleEntry
    .fnend

    .type _NoFloatInPsp, %function
    .global _NoFloatInPsp
_NoFloatInPsp:
    .fnstart
    .cantunwind
    MOV   R2,  R13                      // no auto push floating registers
    SUB   R13, #32                      // add 8 Bytes reg(for STMFD)

    MRS   R3,  PSP
    ADD   R12, R3, #32
    PUSH  {R12}                         // save task SP

    MRS   R12, PRIMASK
    PUSH {R4-R12}

    LDMFD R3, {R4-R11}                  // R4-R11 store PSP reg(auto push when exc in task)
    STMFD R2!, {R4-R11}
    ORR   R0, R0, #FLAG_NO_FLOAT
    .fnend

    .type _handleEntry, %function
    .global _handleEntry
_handleEntry:
    .fnstart
    .cantunwind
    MOV R3, R13                         // R13:the 4th param
    CPSID I
    CPSID F
    B  HalExcHandleEntry

    NOP
    .fnend
