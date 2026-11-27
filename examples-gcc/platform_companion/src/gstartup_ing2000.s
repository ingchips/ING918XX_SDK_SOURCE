    .syntax    unified
    .arch    armv7-m

/* platform_companion（ROM 配套固件）专用 GNU 启动文件。
 *
 * 与通用模板的唯一差别：入口是 main()，不是 APP 的 app_main()。
 * 其余（.data 搬运、.bss 清零）与通用模板一致，因为 main.c 是普通 C 代码。
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
    .equ    Heap_Size, 0
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
    .globl    __Vectors
__Vectors:
    .long    0            /* Top of Stack */
    .long    Reset_Handler         /* Reset Handler */

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

    /* 入口是 main，不是 app_main */
    ldr     r0, =main
    blx     r0

    pop     {r1, pc}

    .pool
    .size    Reset_Handler, . - Reset_Handler
