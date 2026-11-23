# CMake 构建说明

本目录把 `examples-gcc/` 下的示例从手写 Makefile 迁移到 CMake，并同时支持两套工具链：

| 工具链 | 说明 | 工具链文件 |
| --- | --- | --- |
| Arm GNU Toolchain | `arm-none-eabi-gcc`，即现有 `makefile.conf` 使用的编译器 | `cmake/toolchain-arm-none-eabi.cmake` |
| Arm Toolchain for Embedded (ATfE) | Arm 官方 LLVM 发行版，`clang` + `lld` + `newlib-nano` | `cmake/toolchain-atfe.cmake` |

原有 Makefile **未被删除**，两套构建方式可并存对照。

## 依赖

### 1. Arm GNU Toolchain

任意 `arm-none-eabi-gcc` 12 以上版本。通过 `-DARM_GCC_ROOT=<路径>` 或环境变量 `ARM_GCC_ROOT` 指定根目录，默认值见工具链文件。

### 2. ATfE（可选，用于 LLVM 验证）

1. 下载 `ATfE-23.1.0-Windows-x86_64.zip`（或对应平台的 `ATfE-23.1.0-Linux-x86_64.tar.xz`）：
   <https://github.com/arm/arm-toolchain/releases/tag/release-23.1.0-ATfE>
2. 下载同版本的 `ATfE-newlib-nano-overlay-23.1.0.zip`，**解压到主包目录之上**。
   叠加后应存在 `<ATFE_ROOT>/bin/newlib-nano.cfg` 与 `<ATFE_ROOT>/lib/clang-runtimes/newlib-nano/`。
3. 通过 `-DATFE_ROOT=<路径>` 或环境变量 `ATFE_ROOT` 指定根目录。

> ATfE 默认 C 库是 picolibc。本项目走 `--config=newlib-nano.cfg` 切到 newlib-nano，
> 以贴近现有 GNU 工具链的 `--specs=nano.specs` 行为。未装 overlay 时配置阶段会直接报错。

### 3. CMSIS

通过环境变量 `CMSIS_BASE` 或 `-DING_CMSIS=<路径>` 指定，需包含 `Include/` 子目录。

## 构建

```bash
# GCC
cmake -B build-gcc --toolchain=cmake/toolchain-arm-none-eabi.cmake -G Ninja \
      -DCHIP=ing918 -DING_EXAMPLES=get_started
cmake --build build-gcc

# ATfE / LLVM
cmake -B build-atfe --toolchain=cmake/toolchain-atfe.cmake -G Ninja \
      -DCHIP=ing918 -DING_EXAMPLES=get_started
cmake --build build-atfe
```

也可用预设：

```bash
cmake --preset gcc-ing918
cmake --build --preset gcc-ing918
```

产物位于 `build-*/<示例名>/`：`<示例名>.elf`、`<示例名>.bin`、`<示例名>.map`。

## 可配置项

| 变量 | 默认 | 说明 |
| --- | --- | --- |
| `CHIP` | `ing918` | 芯片家族，决定 `-mcpu`/`-mfpu`/启动文件/ROM 符号表 |
| `ING_C_STANDARD` | `gnu11` | C 标准，可选 `gnu99` / `gnu11` / `gnu17` |
| `ING_EXAMPLES` | `get_started` | 要构建的示例列表，分号分隔 |
| `ING_CMSIS` | `$CMSIS_BASE` | CMSIS Core 目录 |
| `ARM_GCC_ROOT` | 见工具链文件 | GNU 工具链根目录 |
| `ATFE_ROOT` | 见工具链文件 | ATfE 安装根目录 |

新增芯片家族：在 `cmake/ing_example.cmake` 的芯片参数表里补 `CPU`、`FPU`、`BUNDLE` 三项。

## 两套工具链的关键差异（实测）

迁移过程中踩到的坑，均已在本仓库处理：

1. **`--just-symbols` 不兼容**
   GNU ld 用 `-Wl,--just-symbols=bundles/typical/<chip>/symdefs.g` 导入 ROM 符号表。
   lld 会报 `symdefs.g: not an ELF file`。改为生成一个包装链接脚本：

   ```ld
   INCLUDE <symdefs.g 绝对路径>
   INCLUDE <原链接脚本>
   ```

   lld 支持链接脚本中的 `符号 = 0x...;` 赋值与 `INCLUDE`，导入后符号类型为 `A`（绝对地址）。

2. **`.heap (COPY)` 在 lld 下失效**
   原链接脚本用 `.heap (COPY)` 单独成段。lld 会报
   `ignoring memory region assignment for non-allocatable section '.heap'`，
   并且**真的不分配 RAM 地址**，导致 `__HeapBase`/`__HeapLimit` 变成段内偏移
   （`0x0` / `0x2000`），堆会落到非法地址。

   修复：把 `*(.heap*)` 并入 NOLOAD 的 `.bss` 段，堆符号随 `.bss` 落在 RAM 内。
   修复后两套工具链的堆栈符号一致：

   | 符号 | GCC | ATfE |
   | --- | --- | --- |
   | `__HeapBase` | `0x20008630` | `0x20008948` |
   | `__HeapLimit` | `0x2000a630` | `0x2000a948` |
   | `__StackLimit` | `0x2000ff80` | `0x2000ff80` |
   | `__StackTop` | `0x20010000` | `0x20010000` |

3. **CMSIS 5.9 在 C++17 下报错**
   `cmsis_gcc.h` 使用 `register` 关键字，clang 在 `-std=c++17` 下视为错误
   （GCC 仅告警）。本仓库统一锁 `gnu++14` 并附加 `-Wno-register`。
   长期方案是升级到 CMSIS 6。

4. **C++ 标准库不同**
   GCC 用 `libstdc++`（需显式 `-lstdc++`），ATfE 用 `libc++`（由 clang 驱动自动带入）。
   `std::function` 的实现差异会让符号名与代码体积明显不同，属正常现象。

5. **`--specs` 语义映射**

   | 用途 | GNU | ATfE |
   | --- | --- | --- |
   | newlib-nano | `--specs=nano.specs` | `--config=newlib-nano.cfg` |
   | 无 semihosting | `--specs=nosys.specs` | 默认行为，另加 `-lnosys` |
   | 浮点 printf | `-u _printf_float` | 不需要 |

6. **启动文件**
   `gstartup_ing91800.s` 是 GNU 语法汇编，clang 集成汇编器可直接接受，无需改动。
   但 `.s` 文件不经预处理器，`-D__STARTUP_CLEAR_BSS` 对汇编无效（原 Makefile 同样如此），
   故 CMake 中宏定义只作用于 C/C++。

## 实测结果（get_started，Cortex-M3，gnu11）

| 工具链 | text | data | bss（含 8 KB 堆） | .bin |
| --- | --- | --- | --- | --- |
| arm-none-eabi-gcc 15.2 | 37084 | 568 | 11992 | 37652 |
| ATfE 23.1.0 | 31860 | 208 | 13144 | 32068 |

GCC 侧与迁移前 Makefile 构建的尺寸完全一致（`37084 / 568 / 3796`，其中 bss 不含堆），
可作为迁移正确性的对照基线。

ATfE 版体积更小，原因是 `libc++` 的 `std::function` 内联更彻底，且不需要
`-u _printf_float` 强制拉入浮点 printf。两者功能等价：关键 SDK 符号、
静态构造（`_GLOBAL__sub_I_container.cpp`）、堆栈符号均已核对。

## 多芯片支持

### 切芯片脚本 `tools/ingw_switch_chip.py`

复刻 ingWizard 的切芯片逻辑（`data/common.cs`、`gcc.cs`、`gcc.fix` 与
`checkprojform.pas` 的 `ConfigProjRam`），面向**编译验证**只改决定能否编过的内容：

| 内容 | 默认 | 说明 |
| --- | --- | --- |
| `get_started_<family>.ld` | ✅ | 按 bundle meta + series.json 重算 FLASH/RAM，各 family 并存 |
| `src/gstartup_<family>00.s` | ✅ | 缺失时按模板生成（ing916 自动用 armv7e-m + .fpu） |
| `makefile.conf` / `Makefile` / `<proj>.ld` | ❌ | 需 `--with-makefile`，避免破坏其他芯片的基线 |
| `flash_download.ini` | ❌ | 需 `--with-flash-ini`；烧录配置，CI 不需要 |
| `.ingw` 的 series/family | ❌ | 需 `--with-ingw`，避免与未同步的 Makefile 冲突 |

```bash
python tools/ingw_switch_chip.py --project examples-gcc/get_started --series ING9168xx --dry-run
python tools/ingw_switch_chip.py --project examples-gcc/get_started --series ING9168xx
```

### 内存计算（与 ingWizard 一致）

```
app_rom_addr = meta.app.base
app_rom_size = series.flash_base + series.flash_size - app_rom_addr
app_ram_addr = align8(meta.ram.base + meta.ram.size)
app_ram_size = meta.ram.base + sys_ram - app_ram_addr
sys_ram      = series.sys_ram[bundle] or series.def_sys_ram or series.ram_size // 2
```

验证结果与 SDK 既有资产完全吻合：

| 芯片 | 计算值 | 既有资产对照 |
| --- | --- | --- |
| ING9187xx | FLASH 0x28000/376832，RAM 0x20007520/35552 | `get_started.ld` 一致 |
| ING9168xx | FLASH 0x202B000/348160，RAM 0x200054D8/11048 | `bqb_rf.sct` 的 RAM 一致 |
| ING208xx | FLASH 0x202D000/339968，RAM 0x20005338/19656 | 区域末尾 0x2000A000 与 40 KB sys_ram 一致 |

### 芯片支持现状（get_started）

| 芯片 | 内核 | 启动文件 | 链接脚本 | 构建 |
| --- | --- | --- | --- | --- |
| ING9187xx | Cortex-M3 | `gstartup_ing91800.s` | `get_started.ld` | GCC ✅ / ATfE ✅ |
| ING208xx | Cortex-M3 | `gstartup_ing2000.s`（生成） | `get_started_ing20.ld`（生成） | GCC ✅ / ATfE ✅ |
| ING9168xx | Cortex-M4F | `gstartup_ing91600.s`（生成） | `get_started_ing916.ld`（生成） | 资产可用，**demo 不支持** |

> ING916 的启动文件与链接脚本已用最小探针验证：`__HeapBase=0x200054D8`、
> `__StackTop=0x20008000`、`app_main` 落在 `0x202B008`，均正确。
> 但 `examples/get_started/src/main.c` 里有 `#error "916 chip does not support this demo"`，
> 因此 get_started 无法作为 916 的编译验证载体，需换用支持 916 的示例
> （`peripheral_console_realtime`、`pawr`、`hid_mouse`、`voice_remote_ctrl` 等）。

### 实测尺寸

| 芯片 | 工具链 | text | data | bss（含堆） |
| --- | --- | --- | --- | --- |
| ING918 | GCC 15.2 | 37084 | 568 | 11992 |
| ING918 | ATfE 23.1.0 | 31860 | 208 | 13144 |
| ING20 | GCC 15.2 | 35996 | 464 | 11992 |
| ING20 | ATfE 23.1.0 | 27120 | 92 | 13144 |

## 尚未覆盖

- `ing916` 的 demo 载体：见上，需另选示例并迁移到 CMake。
- `central_fota`：额外依赖 `zig` 编译 `fota_client.zig`。
- `smart_home_hub`：额外依赖 `nim` 与 `NIM_PATH`。
- ATfE 编译 `.s` 时 CMake 会附加 `-MD/-MT/-MF`，clang 报 `unused-command-line-argument`，
  属无害噪音。
