    .syntax    unified
    .arch    armv7e-m
    .fpu    fpv4-sp-d16

/* ING ing916 的 GCC 启动文件。
 * 由 tools/ingw_switch_chip.py 生成，模板与 gstartup_ing91800.s 一致。
 * 本文件只提供向量表前两项、.data 搬运、.bss 清零，然后跳 app_main；
 * 完整中断向量由 ROM 提供。
 */

    .section .stack
    .align    3
    .equ    Stack_Size, 128
    .globl    __StackTop
    .globl    __StackLimit
__StackLimit:
    .space    Stack_Size
    .size    __StackLimit, . - __StackLimit
__StackTop:
    .size    __StackTop, . - __StackTop

    .section .heap
    .align    3
    .equ    Heap_Size, 4096
    .globl    __HeapBase
    .globl    __HeapLimit
__HeapBase:
    .if    Heap_Size
    .space    Heap_Size
    .endif
    .size    __HeapBase, . - __HeapBase
__HeapLimit:
    .size    __HeapLimit, . - __HeapLimit

    .section .isr_vector
    .align    2
    .globl    __isr_vector
__isr_vector:
    .long    0            /* Top of Stack */
    .long    Reset_Handler         /* Reset Handler */
    .size    __isr_vector, . - __isr_vector

    .text
    .thumb
    .thumb_func
    .align    2
    .globl    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:

    push    {r1, lr}

    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

.L_loop1:
    cmp    r2, r3
    ittt    lt
    ldrlt    r0, [r1], #4
    strlt    r0, [r2], #4
    blt    .L_loop1

    ldr    r1, =__bss_start__
    ldr    r2, =__bss_end__

    movs    r0, 0
.L_loop3:
    cmp    r1, r2
    itt    lt
    strlt    r0, [r1], #4
    blt    .L_loop3

    ldr     r0, =app_main
    blx     r0

    pop     {r1, pc}

    .pool
    .size    Reset_Handler, . - Reset_Handler
