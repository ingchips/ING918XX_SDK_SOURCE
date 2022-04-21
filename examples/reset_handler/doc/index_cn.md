# Reset Handler

[English](index.md)

SDK 提供的 `platform.bin` 功能固定、不可更改。通过平台补丁工具可以将开发者编写的一段程序作为补丁插入到
`platform.bin` 已有的中断服务程序之前，在一定程序上定制 `platform.bin` 的功能。

对补丁程序存有以下限制和注意事项：

1. 进入补丁程序时，未配置栈空间。因此需要保证：

    1. 程序中不存在压、弹栈操作，或者
    1. 程序自行配置栈空间；

1. 工具会将补丁程序附加到 `platform.bin` 最后一页 eFlash 存储空间的空白处，所以其大小受限；

1. 补丁程序的加载位置不固定。

本示例在系统复位之后立即配置看门狗。这个示例使用汇编开发，未使用栈空间，体积也很小。补丁工具会识别
`Reset_Handler` 函数的最后两行代码，自动调整。

```asm
Reset_Handler   PROC
                EXPORT  Reset_Handler

                ; ...

                LDR  R0, =0xFEED4000    ; will be modified by tool
                BX R0

                ENDP
```

补丁程序准备好之后，在 `Wizard` 里选择菜单 Tools → More → Patching Platform 打开补丁工具。选择
`platform.bin`、补丁程序，然后点击 "Make Patch" 生成带有补丁程序的新的 `platform.bin`，烧录时使用
用这个文件替换掉原有的 `platform.bin` 即可。