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

## CMSIS 版本：5.9 与 6.1 的差异

ING916 是 Cortex-M4F，其 FPU 路径在 CMSIS 5.9 下会撞上 clang 的硬限制：

```c
/* CMSIS 5.9 cmsis_gcc.h */
__ASM volatile ("VMSR fpscr, %0" : : "r" (fpscr) : "vfpcc");
```

`vfpcc` 是 GCC 的伪寄存器，clang 报 `unknown register name 'vfpcc' in asm`，
且是 error。该函数只在 `__FPU_PRESENT && __FPU_USED` 时编译，所以无 FPU 的
ING918/ING20 不会触发，只有 ING916 会。

**CMSIS 6.1.0 已移除这个写法**：

```c
/* CMSIS 6.1 cmsis_gcc.h */
__builtin_arm_set_fpscr(fpscr);   /* clang 原生支持 */
```

### 实测结论

用 CMSIS 6.1.0 重建全部组合，产物与 CMSIS 5.9 **逐字节一致**：

| 组合 | CMSIS 5.9 | CMSIS 6.1 |
| --- | --- | --- |
| gcc / ing918 / get_started | 37084/568/11992 | 相同 |
| atfe / ing918 / get_started | 31860/208/13144 | 相同 |
| gcc / ing20 / get_started | 35996/464/11992 | 相同 |
| atfe / ing20 / get_started | 27120/92/13144 | 相同 |
| gcc / ing916 / peripheral_console_realtime | 29344/560/5424 | 相同 |
| atfe / ing916 / peripheral_console_realtime | **编译失败** | 17756/184/5424 |

因此 CI 的 `build-matrix` 已切换到 CMSIS 6.1.0，ING916 也纳入了 ATfE 通道。

### 对现有工程的影响

- **仓库内 Keil/IAR/Segger 工程不受影响**：它们引用 ingWizard 安装目录下的
  CMSIS（5.9），本次只改了 CI 下载的版本。
- 本地 CMake 构建默认仍读 `CMSIS_BASE` 环境变量；要用 6.1 需自行指向
  `CMSIS_6-6.1.0/CMSIS/Core`，或用 `-DING_CMSIS=<路径>`。
- 若要统一升级到 CMSIS 6，需单独评估：目录结构仍是 `CMSIS/Core/Include`，
  但部分旧头（如 `core_cmInstr.h`）在 6.x 已并入 `cmsis_gcc.h`，Keil 工程的
  include 列表可能需要同步调整。

### 另一处 clang 兼容修复

`src/BSP/eflash.c` 的 `security_page_read()` 是 `__attribute__((naked))`，
函数体内有 `(void)addr; (void)prog;` 两条 C 语句，clang 不允许。
已加 `!defined(__clang__)` 条件，GCC 行为不变。

## Keil 工程的编译校验（CI 内部）

`examples/` 下的工程是 Keil 工程，CI 里额外用
`.github/scripts/keil2cmake.py` 把它们转成 CMake 再编译一遍，确保
"改了代码导致 Keil 工程在 GCC 下编不过"能立刻暴露。

```bash
python .github/scripts/keil2cmake.py     --project examples/peripheral_ibeacon/peripheral_ibeacon.uvprojx     --chip ing918 --out build/keil-import
```

脚本做的事：

1. 读 `.uvprojx` 的 Target、`Device`、`<Files>` 源文件列表、`IncludePath`、`Define`
2. 过滤掉启动汇编与 `symdefs.*`（这些由 SDK 提供）
3. 从 `examples-gcc/<proj>/` 找配套的 `src/gstartup_<chip>00.s` 与 `<proj>_<chip>.ld`
   （缺失时先用 `tools/ingw_switch_chip.py` 生成）
4. 生成 `<out>/<proj>/CMakeLists.txt`，路径统一写成 `${ING_SDK_ROOT}/...`

约束：

- **只在 CI 里用**，产物写到 `build/keil-import/`（已 gitignore），不回写仓库
- 不改动 `examples/` 下任何 Keil 工程文件
- 与 `C:\ming\source\KeilFormat` 无关：那个项目服务于 `compile_commands.json`
  和 UV4 构建，用途不同

工作流 `.github/workflows/keil-import.yml` 目前覆盖
`peripheral_ibeacon / ing918`，新增工程只需在矩阵里加一行。

## 全量工程的编译验证（build-all）

`build-matrix` 只覆盖手写 CMake 的 6 个示例。`examples/` 下还有 55 个 Keil
工程，`build-all` 把它们全部按三个芯片生成 CMake 并编译，用来回答
"切芯片还能不能编过"。

### 生成器 `tools/gen_all_cmake.py`

```bash
# 生成全部（54 个工程 × 3 芯片）
python tools/gen_all_cmake.py --out build/gen

# 生成后立刻编译；--group/--groups 用于 CI 分片
python tools/gen_all_cmake.py --out build/gen --probe --group 0 --groups 12
```

对每个「工程 × 芯片」：

1. 解析 `.uvprojx` 得到源文件、include、宏（`tools/keil_uvprojx.py`）
2. 读 `.ingw` 的 bundle，用 `tools/ing_chip.py` 算 FLASH/RAM
3. 现场生成 `gstartup_<family>00.s` 与 `<proj>.ld`（Keil 的 `.s` 是 ARMCC
   语法、链接信息在 `.sct` 里，都不能直接给 GCC 用）
4. 改写 ARMCC 专用源文件：`portable/RVDS` → `portable/GCC`、
   `context_rvds.S` → `context_gcc.S`（目标文件存在时才替换）
5. 按需链接预编译库：`ing<chip>_uecc.lib`、`SigMesh.lib`、`power_ctrl.lib`
6. 写 `CMakeLists.txt`，路径统一 `${ING_SDK_ROOT}/...`

产物全部在 `build/gen/`（已 gitignore），不修改任何 Keil 工程。

### 语言标准：gnu90

Keil4（ARMCC 5.03）的工程是 `uC99=0`，即纯 C89/C90。验证用的标准因此定为
`gnu90`：

| 标准 | 结果 | 原因 |
| --- | --- | --- |
| `-std=c90` | **无法编译** | CMSIS 的 `__STATIC_INLINE` 展开成 `inline`，C90 没有这个关键字 |
| `-std=gnu90` | 可用 | C90 语义 + GNU 提供 `inline`，与 ARMCC 的 `__inline` 扩展等价 |
| `-std=c99/gnu99` | 可用但更宽松 | 会放过"用了 C99 语法但 Keil4 编不过"的代码 |

`ing_example.cmake` 的默认值已改为 `gnu90`。

### CMSIS 版本分工

| 工作流 | CMSIS | 原因 |
| --- | --- | --- |
| `build-matrix`（ATfE） | 6.1.0（下载） | CMSIS 6 用 `__builtin_arm_set_fpscr` 取代 `vfpcc`，ING916 才能在 clang 下编译 |
| `build-all` / `keil-import` | 4.30（仓库内 `third_party/cmsis`） | 这是 ingWizard 配给 Keil4 的版本；CMSIS 5.6+ 的 `cmsis_gcc.h` 用了 `for` 循环内声明，与 gnu90 冲突 |

`third_party/cmsis/Include` 就是 ingWizard 安装目录里那份，锁定版本避免漂移。

### 配置清单 `.github/project-matrix.yaml`

测哪些组合由这份清单决定，它是**验收契约**：清单里声明成立的组合必须全部
编过，任何一个失败 CI 就红；不测的工程写 `skip: true` 并注明原因。

```yaml
defaults:
  chips: [ing918, ing916, ing20]
  c_standard: gnu90

projects:
  # 需要 C99 的工程（第三方库用了 C99 语法）
  voice_remote_ctrl:
    c_standard: c99
    note: libsbc 使用 C99 循环内声明

  # 只测部分芯片（该芯片缺 ROM 符号 / 缺外设 / 资源不足）
  get_started:
    chips: [ing918, ing20]
    note: demo 自身 #error 拒绝 916

  # 不测（历史遗留或结构性不兼容）
  mesh-light:
    skip: true
    note: SigMesh.lib 依赖 ARMCC printf 引擎内部符号
```

要点：

- **新增工程不用登记**，默认按 `defaults` 测（3 芯片 + gnu90）
- 只有"不该按默认测"的工程才需要加一条
- 把某工程从 `skip` 改成测、或把 `chips` 从部分扩到全部，就是"这项该修了"的信号
- `c_standard` 会写进生成的 `CMakeLists.txt`（`set(ING_C_STANDARD ...)`），
  单独构建该工程时也生效

### 当前验收结果（2026-11-27）

**清单内 135 个组合全部通过**，5 个工程按清单 skip。

生成器已经处理掉的通用问题：

| 问题 | 处理方式 |
| --- | --- |
| Keil 的 `.s` 是 ARMCC 语法 | 现场生成 GNU 启动文件 |
| 链接信息在 `.sct` 里 | 按 bundle meta + series.json 生成 `.ld` |
| `portable/RVDS` 是 ARMCC 移植层 | 自动替换为 `portable/GCC`（目标存在时） |
| FreeRTOS 需要 `__initial_sp` | 链接脚本 `PROVIDE(__initial_sp = __StackTop)` |
| SigMesh.lib 引用 `__2printf` | 链接脚本 `PROVIDE(__2printf = printf)` |
| `-Diprintf=platform_printf` 与 newlib 声明冲突 | 对这类工程加 `-D__STRICT_ANSI__` |
| 按芯片预编译的 `.o`（如 `ing918_fota_client.o`） | glob `src/*<chip>*.o` 作为外部对象链接 |
| 缺 uECC / SigMesh / power_ctrl 库 | 按源文件特征与工程名链接 |
| 工程自己的堆栈大小（很多 Heap_Size 是 0） | 从工程 `startup_*.s` 解析后写进生成的启动文件 |

### 清单里 skip 的工程

| 工程 | 原因 |
| --- | --- |
| `cherry_usb` | CherryUSB 要求高于 Keil4 的 C99 约束；USB 仅 916/20 有 |
| `mesh-light` | SigMesh.lib 依赖 ARMCC printf 引擎内部符号（`_printf_percent` 等） |
| `peripheral_console_rt-thread` | 仓库只有 `context_rvds.S`，没有 GCC 汇编版本（历史遗留） |
| `platform_companion` | ROM 配套固件，链接布局与 APP 不同 |
| `reset_handler` | 纯汇编工程，没有 C/C++ 源文件 |

### 清单里只测部分芯片的工程（11 个）

每条都是实测确认过的，不是猜测：

| 工程 | 测 | 排除 | 实测原因 |
| --- | --- | --- | --- |
| `get_started` | 918/20 | 916 | `examples/get_started/src/main.c:19` 直接 `#error "916 chip does not support this demo"` |
| `central_cte` | 20 | 918 | 918 的 symdefs 无 `ll_scanner_enable_iq_sampling`（CTE 测向是 916/20 的 ROM 功能） |
| `central_cte` | 20 | 916 | RAM 溢出 344 字节（ING916 只有 11 KB） |
| `ext_raw_packet` | 916/20 | 918 | 918 的 symdefs 无 `ll_raw_packet_set_tx_cte` |
| `periodic_advertiser` | 916/20 | 918 | 918 的 symdefs 无 `gap_set_connectionless_cte_tx_param` |
| `periodic_scanner` | 916/20 | 918 | 918 的 symdefs 无 `gap_set_connectionless_iq_sampling_enable` |
| `peripheral_led` | 916/20 | 918 | 918 的 symdefs 无 `gap_set_connection_cte_tx_param` |
| `pawr` | 916/20 | 918 | `setup_soc.cgen` 用了 `SYSCTRL_ITEM_APB_PinCtrl`/`_UART0`/`_WDT`，918 的 SYSCTRL 头没有这些枚举 |
| `peripheral_band` | 918/916 | 20 | `main.c:109` `#error SOFTWARE_RTC_DHMS must be defined`，且 `PLATFORM_CB_IRQ_RTC` 在 20 上不存在（20 无 RTC） |
| `peripheral_pedometer` | 918/916 | 20 | `board.c:493` `#error The board has no pins to connect to the iic device`（ING2086_DK 未定义 iic 引脚） |
| `io_over_ble_mas` | 918/20 | 916 | RAM 溢出 648 字节 |
| `io_over_ble_sla` | 918/20 | 916 | RAM 溢出 504 字节 |

> 注：`central_cte` 的两条原因不同——918 是缺 ROM 符号，916 是 RAM 不够。

**已修复并恢复三芯片的**：`peripheral_battery` 之前在 918 上因
`adc_prepare_calibration` / `adc_calibrate` 未定义而失败。实测发现这两个符号
由工程自带的预编译对象 `src/Tools/adc_cali.o` 提供，而生成器当时只链接
文件名含芯片名的 `.o`。现在改为读取 `.uvprojx` 里声明的所有 `.o`/`.lib`
（文件名含 `ing918/ing916/ing20` 的按芯片过滤），该项已恢复为三芯片验证。

