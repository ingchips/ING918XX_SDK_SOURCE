# Reset Handler

[中文版](index_cn.md)

`platform.bin` provided by SDK is pre-built, and function-fixed. With the help of binary patching tool,
a small program can be inserted before ISRs defined by `platform.bin` are called, making it possible
to customize `platform.bin`.

There are following limitations for a patch program:

1. When entering the patch program, stack has not been initialized, therefore, it must be guaranteed that,

    1. There are no _push_ or _pop_ operations, or,
    1. The program take in charge of stack allocation for its own usage;

1. Binary patching tool will append the patch to free space of the last Flash page storing `platform.bin`,
    so its size is limited;

1. The load address of the patch program is not fixed.

This example configures the watchdog right after the system resets. This example is written in assembly,
do not use stack, and its size is very small. Patching tool will recognize the last two lines of
`Reset_Handler`, and modify it automatically.

```asm
Reset_Handler   PROC
                EXPORT  Reset_Handler

                ; ...

                LDR  R0, =0xFEED4000    ; will be modified by tool
                BX R0

                ENDP
```

When the patch program is ready, open patching tool from the main menu of _Wizard_:
Tools → More → Patching Platform. Choose `platform.bin` and patch program, then click "Make Patch" to
generate a new `platform.bin` with the patch attached. When downloading, use this one to replace the
original `platform.bin`.