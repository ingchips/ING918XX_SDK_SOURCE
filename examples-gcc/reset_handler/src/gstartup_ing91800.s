    .syntax    unified
    .arch    armv7-m

/* reset_handler 工程专用的 GNU 启动文件。
 *
 * 与通用模板的区别：这个工程的 Reset_Handler 本身就是被测对象——它打开
 * 定时器时钟、把定时器配成看门狗、然后跳到一个由工具写入的固定地址
 * （原工程里是 0xFEED4000），既不走 app_main，也不搬运 .data/清 .bss
 * （工程里没有 C 代码）。
 *
 * 所以这里保留原逻辑，只把 ARMCC 语法（AREA/PROC/DCD/EXPORT）换成
 * GNU as 语法。符号名沿用原工程的 __initial_sp / __heap_base / __heap_limit。
 */

    .section .stack
    .align    3
    .globl    __StackLimit
    .globl    __initial_sp
__StackLimit:
__initial_sp:

    .section .heap
    .align    3
    .globl    __HeapBase
    .globl    __HeapLimit
    .globl    __heap_base
    .globl    __heap_limit
__HeapBase:
__heap_base:
__HeapLimit:
__heap_limit:

    .section .isr_vector
    .align    2
    .globl    __Vectors
__Vectors:
    .long    Reset_Handler

    .text
    .thumb
    .thumb_func
    .align    2
    .globl    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:

    /* AHB_SYSCTRL->SYSCTRL_ClkGate |= (1 << 6) */
    ldr    r1, =0x40070000
    ldr    r0, [r1, #0x04]
    orr    r0, r0, #0x40
    str    r0, [r1, #0x04]

    /* APB_TMR0->CTL = (1 << bsTMR_CTL_RELOAD) */
    ldr    r1, =0x40001000
    movs    r0, #0x02
    str    r0, [r1, #0x08]
    /* APB_TMR0->CMP = TMR_CLK_FREQ */
    ldr    r0, =24000000
    str    r0, [r1, #0x04]
    /* APB_TMR0->CTL |= TMR_EN | WatchDog_EN */
    ldr    r0, [r1, #0x08]
    orr    r0, r0, #0x21
    str    r0, [r1, #0x08]
    /* APB_TMR0->LOCK = 1 */
    movs    r0, #0x01
    str    r0, [r1, #0x0c]

    /* 跳到工具写入的地址（原工程注释：will be modified by tool） */
    ldr    r0, =0xFEED4000
    bx     r0

    .pool
    .size    Reset_Handler, . - Reset_Handler
