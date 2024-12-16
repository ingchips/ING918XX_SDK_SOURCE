.syntax unified
.arch armv7-m
.thumb

.equ    OS_NVIC_INT_CTRL,            0xE000ED04
.equ    OS_NVIC_SYSPRI2,             0xE000ED20
.equ    OS_NVIC_PENDSV_PRI,          0xF0F00000
.equ    OS_NVIC_PENDSVSET,           0x10000000
.equ    OS_TASK_STATUS_RUNNING,      0x0010
.equ    OS_CONTROL_KERNEL_MODE,      0x2


    .section .text
    .thumb

.macro SIGNAL_CONTEXT_RESTORE
    push    {r12, lr}
    blx     OsSignalTaskContextRestore
    pop     {r12, lr}
    cmp     r0, #0
    mov     r1, r0
.endm

    .type HalStartToRun, %function
    .global HalStartToRun
HalStartToRun:
    .fnstart
    .cantunwind

    ldr     r4, =OS_NVIC_SYSPRI2
    ldr     r5, =OS_NVIC_PENDSV_PRI
    str     r5, [r4]

    mov     r0, #OS_CONTROL_KERNEL_MODE
    msr     CONTROL, r0

    ldr     r1, =g_losTask
    ldr     r0, [r1, #4]
    ldr     r12, [r0]
    add     r12, r12, #36

    ldmfd   r12!, {R0-R7}
    // add     r12, r12, #72
    msr     psp, r12
    mov     lr, r5
    cpsie   i
    bx      r6
    .fnend

    .type ArchIntLock, %function
    .global ArchIntLock
ArchIntLock:
    .fnstart
    .cantunwind

    MRS R0, PRIMASK
    CPSID I
    BX LR
    .fnend

    .type ArchIntUnLock, %function
    .global ArchIntUnLock
ArchIntUnLock:
    .fnstart
    .cantunwind

    MRS R0, PRIMASK
    CPSIE I
    BX LR
    .fnend

    .type ArchIntRestore, %function
    .global ArchIntRestore
ArchIntRestore:
    .fnstart
    .cantunwind

    MSR PRIMASK, R0
    BX LR
    .fnend

    .type ArchTaskSchedule, %function
    .global ArchTaskSchedule
ArchTaskSchedule:
    .fnstart
    .cantunwind

    ldr     r0, =OS_NVIC_INT_CTRL
    ldr     r1, =OS_NVIC_PENDSVSET
    str     r1, [r0]
    bx      lr
    dsb
    isb
    .fnend

    .type HalPendSV, %function
    .global HalPendSV
HalPendSV:
    .fnstart
    .cantunwind

    mrs     r12, PRIMASK
    cpsid   I

HalTaskSwitch:
    SIGNAL_CONTEXT_RESTORE

    push    {r12, lr}
    blx     OsSchedTaskSwitch
    pop     {r12, lr}
    cmp     r0, #0
    mov     r0, lr
    bne     TaskContextSwitch
    msr     PRIMASK, r12
    bx      lr

TaskContextSwitch:
    mov     lr, r0
    mrs     r0, psp

    stmfd   r0!, {r4-r12}

    ldr     R5, =g_losTask
    ldr     R6, [R5]
    str     R0, [R6]
    ldr     R0, [R5, #4]
    str     R0, [R5]
    ldr     R1,   [R0]

    ldmfd   R1!, {R4-R12}
    msr     PSP,  R1

    msr     PRIMASK, R12
    bx      LR
    .fnend
