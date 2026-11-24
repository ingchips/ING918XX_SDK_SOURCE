#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""ing_chip.py —— 芯片系列参数、内存算法与 GNU 启动文件模板。

被三个脚本共用，避免同一套算法出现多份实现：
  * tools/ingw_switch_chip.py     切换单个工程的芯片
  * tools/gen_all_cmake.py        批量生成全量工程的 CMake 工程
  * .github/scripts/keil2cmake.py 单个 Keil 工程转 CMake

内存算法与 ingWizard 的 checkprojform.pas 保持一致：

    app_rom_addr = meta.app.base
    app_rom_size = series.flash_base + series.flash_size - app_rom_addr
    app_ram_addr = align8(meta.ram.base + meta.ram.size)
    app_ram_size = meta.ram.base + sys_ram - app_ram_addr
    sys_ram      = series.sys_ram[bundle] or series.def_sys_ram or series.ram_size // 2
"""

from __future__ import annotations

import json
from pathlib import Path

# ---------------------------------------------------------------------------
# 芯片系列参数，取自 ingWizard 的 data/series.json
# ---------------------------------------------------------------------------
SERIES = {
    "ING9187xx": {
        "family": "ing918",
        "flash_base": 16384,
        "flash_size": 524288,
        "ram_size": 131072,
    },
    "ING9188xx": {
        "family": "ing918",
        "flash_base": 16384,
        "flash_size": 524288,
        "ram_size": 131072,
    },
    "ING9168xx": {
        "family": "ing916",
        "flash_base": 33554432,
        "flash_size": 524288,
        "ram_size": 65536,
        "sys_ram": {"mini": 57344, "noos_mini": 57344},
    },
    "ING208xx": {
        "family": "ing20",
        "flash_base": 33554432,
        "flash_size": 524288,
        "ram_size": 57344,
        "def_sys_ram": 40960,
        "sys_ram": {"mini": 49152, "noos_mini": 49152},
    },
}

# 芯片家族 -> 默认系列名（每个家族一个主系列）
FAMILY_SERIES = {
    "ing918": "ING9187xx",
    "ing916": "ING9168xx",
    "ing20": "ING208xx",
}

# 每个 family 的 GCC 编译参数，对齐 ingWizard data/gcc.cs。
# 注意 ingWizard 对 ing916 用的是 softfp，不是 hard。
ARCH_FLAGS = {
    "ing918": "-mthumb -mcpu=cortex-m3",
    "ing916": "-mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp",
    "ing20": "-mthumb -mcpu=cortex-m3",
}

# 启动文件里的堆大小：ING916 可用 RAM 只有 0x2B28，8192 会挤掉 .data/.bss
HEAP_SIZE = {"ing918": 8192, "ing916": 4096, "ing20": 8192}

# GNU 汇编启动文件模板（与 examples-gcc/get_started/src/gstartup_ing91800.s 同源）
STARTUP_TEMPLATE = """    .syntax    unified
    .arch    {arch}
{fpu}
/* ING {family} 的 GCC 启动文件。
 * 由 tools/ing_chip.py 生成，模板与 gstartup_ing91800.s 一致。
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
    .equ    Heap_Size, {heap_size}
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

    push    {{r1, lr}}

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

    pop     {{r1, pc}}

    .pool
    .size    Reset_Handler, . - Reset_Handler
"""


def align(value: int, boundary: int = 8) -> int:
    return ((value + boundary - 1) // boundary) * boundary


def sys_ram_of(series: str, bundle: str) -> int:
    """复刻 GetSysRamSizeOfBundle 的回退链。"""
    spec = SERIES[series]
    size = spec.get("sys_ram", {}).get(bundle, 0)
    if not size:
        size = spec.get("def_sys_ram", 0)
    if not size:
        size = spec["ram_size"] // 2
    return size


def compute_memory(sdk_root: Path, series: str, bundle: str) -> dict:
    """复刻 checkprojform.pas 第 935/936/939 行的地址计算。"""
    meta_path = Path(sdk_root) / "bundles" / bundle / series / "meta.json"
    if not meta_path.is_file():
        raise FileNotFoundError(f"缺少 bundle 元数据：{meta_path}")

    meta = json.loads(meta_path.read_text(encoding="utf-8"))
    spec = SERIES[series]

    ram_addr = align(meta["ram"]["base"] + meta["ram"]["size"])
    ram_size = meta["ram"]["base"] + sys_ram_of(series, bundle) - ram_addr

    rom_addr = meta["app"]["base"]
    rom_size = spec["flash_base"] + spec["flash_size"] - rom_addr

    if rom_size <= 0 or ram_size <= 0:
        raise ValueError(
            f"{series}/{bundle} 计算出的区域大小非法：rom={rom_size} ram={ram_size}")

    return {
        "meta": meta,
        "rom_addr": rom_addr,
        "rom_size": rom_size,
        "ram_addr": ram_addr,
        "ram_size": ram_size,
        "family": spec["family"],
    }


def startup_text(family: str) -> str:
    """渲染某个芯片家族的 GNU 启动文件内容（LF 行尾）。"""
    arch = "armv7e-m" if family == "ing916" else "armv7-m"
    fpu = "    .fpu    fpv4-sp-d16\n" if family == "ing916" else ""
    return STARTUP_TEMPLATE.format(arch=arch, fpu=fpu, family=family,
                                   heap_size=HEAP_SIZE[family])


def series_of_family(family: str) -> str:
    if family not in FAMILY_SERIES:
        raise ValueError(f"未知芯片家族：{family}")
    return FAMILY_SERIES[family]


def bundle_available(sdk_root: Path, bundle: str, series: str) -> bool:
    """该 bundle 在目标系列下是否有符号表（决定能否链接）。"""
    return (Path(sdk_root) / "bundles" / bundle / series / "symdefs.g").is_file()
