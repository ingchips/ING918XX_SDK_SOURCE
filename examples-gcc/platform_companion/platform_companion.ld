/* platform_companion（ROM 配套固件）专用链接脚本。
 *
 * 与普通 APP 的差别（来自 examples/platform_companion/output/*.sct）：
 *   FLASH 从 0x02002000 开始，不是 APP 的 0x0202D000
 *   RAM 分成两块：0x20000000/0x200 与 0x200055C8/0x150
 *
 * .data/.bss 放在 RAM1（512 B），栈放在 RAM2 末尾。
 * 如果哪天 .data+.bss 超过 512 B，把 .bss 改到 RAM2 即可。
 */
MEMORY
{
  FLASH (rx)  : ORIGIN = 0x02002000, LENGTH = 0x00200000
  RAM1  (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00000200
  RAM2  (rwx) : ORIGIN = 0x200055C8, LENGTH = 0x00000150
}

ENTRY(Reset_Handler)

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text*)
        *(.init)
        *(.fini)
        *(.rodata*)
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH

    __etext = ALIGN(4);

    .data : AT (__etext)
    {
        __data_start__ = .;
        *(.data*)
        . = ALIGN(4);
        *(.init_array*)
        *(.fini_array*)
        __data_end__ = .;
    } > RAM1

    /* 堆与栈都不占镜像；.heap 段由启动文件提供符号 */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        __end__ = .;
        PROVIDE(end = .);
        *(.heap*)
    } > RAM1

    .stack_dummy (NOLOAD) :
    {
        *(.stack*)
    } > RAM2

    __StackTop = ORIGIN(RAM2) + LENGTH(RAM2);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);
    PROVIDE(__initial_sp = __StackTop);

    ASSERT(__StackLimit >= __bss_end__, "region RAM overflowed with stack")
}
