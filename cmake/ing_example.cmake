# ING SDK 示例工程的公共构建逻辑。
#
# 目标：同一份工程描述，既能在 arm-none-eabi-gcc 下构建，也能在
# Arm Toolchain for Embedded (LLVM/ATfE) 下构建。
#
# 由顶层 CMakeLists.txt include 一次，各示例 CMakeLists.txt 只调用
# ing_add_example()。

include_guard(GLOBAL)

# ---------------------------------------------------------------------------
# 芯片家族参数表
#
# 新增芯片家族时在这里补三项：CPU、FPU、SERIES。
# SERIES 是 bundles/<bundle>/<SERIES>/ 的目录名，决定 ROM 符号表。
# ---------------------------------------------------------------------------
set(ING_CHIP_ing918_CPU    cortex-m3)
set(ING_CHIP_ing918_FPU    "")
set(ING_CHIP_ing918_SERIES ING9187xx)

# ING916 = Cortex-M4F。ingWizard 用 softfp，不是 hard。
set(ING_CHIP_ing916_CPU    cortex-m4)
set(ING_CHIP_ing916_FPU    fpv4-sp-d16)
set(ING_CHIP_ing916_SERIES ING9168xx)

# ING20（ING208xx）：Cortex-M3，与 ING918 同核但 ROM/RAM 布局不同。
# 注意系列名是 ING208xx；bundles/*/ING20xx 是残缺的旧目录，不可用。
set(ING_CHIP_ing20_CPU    cortex-m3)
set(ING_CHIP_ing20_FPU    "")
set(ING_CHIP_ing20_SERIES ING208xx)

# 目标芯片家族，可用 -DCHIP= 覆盖
if(NOT DEFINED CHIP)
    set(CHIP ing918 CACHE STRING "目标芯片家族：ing918 / ing916 / ing20")
endif()
set(CHIP "${CHIP}" CACHE STRING "目标芯片家族：ing918 / ing916 / ing20" FORCE)

if(NOT DEFINED ING_CHIP_${CHIP}_CPU)
    message(FATAL_ERROR "未知芯片家族 CHIP=${CHIP}，请在 cmake/ing_example.cmake 中登记")
endif()
set(ING_CPU    "${ING_CHIP_${CHIP}_CPU}")
set(ING_FPU    "${ING_CHIP_${CHIP}_FPU}")
# ING918 有 ING9187xx / ING9188xx 两个 ROM 版本，符号表与内存布局都不同。
# 具体用哪个由工程决定（生成的 CMakeLists 会显式 set），这里只提供默认值。
if(NOT DEFINED ING_SERIES)
    set(ING_SERIES "${ING_CHIP_${CHIP}_SERIES}")
endif()

# bundle 目录名（typical / noos_mini / mini / exp ...），可用 -DING_BUNDLE_DIR= 覆盖。
# 单个示例也可以在自己的 CMakeLists 里用 BUNDLE 参数覆盖。
if(NOT DEFINED ING_BUNDLE_DIR)
    set(ING_BUNDLE_DIR typical CACHE STRING "bundle 目录名：typical / noos_mini / mini ...")
endif()

# C 语言标准，可用 -DING_C_STANDARD= 覆盖。
#
# 默认 gnu90：Keil4（ARMCC 5.03）的语言级别是 C89/C90，但它通过
# __inline 等扩展关键字支持 CMSIS，所以严格的 -std=c90 反而编不过
# （CMSIS 的 __STATIC_INLINE 展开成 C99 的 inline）。gnu90 在语义上
# 对齐 C90，同时保留必要的关键字扩展，是与 Keil4 最接近的基线。
#
# 取值：c89/c90/c99/c11/c17（严格）或 gnu89/gnu90/gnu99/gnu11/gnu17。
if(NOT DEFINED ING_C_STANDARD)
    set(ING_C_STANDARD gnu90 CACHE STRING "C 标准：gnu90 对齐 Keil4，gnu99/gnu11 更宽松")
endif()
if(NOT ING_C_STANDARD MATCHES "^(gnu|c)(89|90|99|11|17|23)$")
    message(FATAL_ERROR "不支持的 ING_C_STANDARD=${ING_C_STANDARD}，可选 c89/c90/c99/c11/gnu*")
endif()
string(REGEX REPLACE "^(gnu|c)" "" ING_C_STD_NUM "${ING_C_STANDARD}")
if(ING_C_STANDARD MATCHES "^gnu")
    set(ING_C_EXTENSIONS ON)
else()
    set(ING_C_EXTENSIONS OFF)
endif()

# SDK 根目录：本文件位于 <sdk>/cmake/ 下
get_filename_component(ING_SDK_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

# CMSIS Core 目录，CI 用环境变量注入
if(NOT DEFINED ING_CMSIS)
    if(DEFINED ENV{CMSIS_BASE})
        set(ING_CMSIS "$ENV{CMSIS_BASE}")
    else()
        set(ING_CMSIS "C:/ING_SDK/ING_SDK/wizard/CMSIS" CACHE PATH "CMSIS Core 目录")
    endif()
endif()

# ---------------------------------------------------------------------------
# ing_add_example(<target>
#     SOURCES   <源文件...>
#     STARTUP   <启动文件>        # .s，可选
#     LDSCRIPT  <链接脚本>        # .ld，可选
#     DEFINES   <宏...>           # 可选
#     INCLUDES  <额外 include>    # 可选
#     BUNDLE    <bundle 目录名>   # 可选，覆盖 ING_BUNDLE_DIR
#     OPTIMIZE  <-O 选项>         # 可选，默认 -O0
#     NO_PRINTF_FLOAT             # 可选，GCC 下不加 -u _printf_float
# )
# ---------------------------------------------------------------------------
function(ing_add_example name)
    cmake_parse_arguments(ARG "NO_PRINTF_FLOAT" "STARTUP;LDSCRIPT;BUNDLE;OPTIMIZE"
                          "SOURCES;DEFINES;INCLUDES" ${ARGN})

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "ing_add_example(${name}) 缺少 SOURCES")
    endif()

    set(_bundle_dir "${ARG_BUNDLE}")
    if(NOT _bundle_dir)
        set(_bundle_dir "${ING_BUNDLE_DIR}")
    endif()

    # 校验：工程 .ingw 里声明的 series 必须与实际链接的一致。
    # ING9187xx 与 ING9188xx 是两颗不同的 ROM（符号地址、app.base 都不同），
    # 用错会链接成功但固件跑不起来。
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.ingw")
        file(READ "${CMAKE_CURRENT_SOURCE_DIR}/${name}.ingw" _ingw_json)
        string(JSON _ingw_series ERROR_VARIABLE _json_err GET "${_ingw_json}" series)
        # 只在同一芯片家族内比较：跨芯片构建（918 工程用 -DCHIP=ing20）时
        # series 本来就该不同，不算错误。
        set(_ingw_family "")
        if(_ingw_series MATCHES "^ING918")
            set(_ingw_family "ing918")
        elseif(_ingw_series MATCHES "^ING916")
            set(_ingw_family "ing916")
        elseif(_ingw_series MATCHES "^ING20")
            set(_ingw_family "ing20")
        endif()

        if(NOT _json_err AND _ingw_family STREQUAL CHIP
           AND NOT _ingw_series STREQUAL ING_SERIES)
            message(FATAL_ERROR
                "${name}: .ingw 声明 series=${_ingw_series}，但链接使用 ${ING_SERIES}。"
                "两者的 ROM 符号地址与内存布局不同，产物会跑不起来。"
                "请在 CMakeLists 里 set(ING_SERIES ${_ingw_series})")
        endif()
    endif()

    add_executable(${name} ${ARG_SOURCES})
    if(ARG_STARTUP)
        target_sources(${name} PRIVATE ${ARG_STARTUP})
    endif()

    # ---- include 路径 ----
    target_include_directories(${name} PRIVATE
        "${ING_SDK_ROOT}/src/BSP"
        "${ING_SDK_ROOT}/src/FWlib"
        # 目录名在 git 里是 StartUP（大写 UP）。Windows 不区分大小写，
        # 但 Linux CI 区分，写成 StartUp 会导致找不到 ingsoc.h。
        "${ING_SDK_ROOT}/src/StartUP/${CHIP}"
        "${ING_SDK_ROOT}/src/Tools"
        "${ING_SDK_ROOT}/src"
        "${ING_SDK_ROOT}/bundles/${_bundle_dir}/inc"
        "${ING_SDK_ROOT}/bundles/${_bundle_dir}/inc/os"
        "${ING_CMSIS}/Include"
        ${ARG_INCLUDES}
    )

    # ---- 编译选项 ----
    # 宏定义只作用于 C/C++：.s 启动文件不经预处理器，给汇编传 -D 无效。
    target_compile_definitions(${name} PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:__STARTUP_CLEAR_BSS>)
    foreach(_def IN LISTS ARG_DEFINES)
        target_compile_definitions(${name} PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:${_def}>)
    endforeach()

    # 优化级别默认 -O0；部分示例的 Makefile 用 -O3（如 LiteOS），可用 OPTIMIZE 对齐
    set(_optimize "${ARG_OPTIMIZE}")
    if(NOT _optimize)
        set(_optimize "-O0")
    endif()

    set(_common "-mthumb" "-mcpu=${ING_CPU}" "${_optimize}"
                "-ffunction-sections" "-fdata-sections" "-fno-builtin")
    if(ING_FPU)
        list(APPEND _common "-mfpu=${ING_FPU}" "-mfloat-abi=softfp")
    endif()

    # 只把 C/C++ 专有选项给 C/C++ 编译；汇编（.s）只需要 -mthumb -mcpu，
    # 否则 clang 会对每个 C 选项报 unused-command-line-argument。
    foreach(_opt IN LISTS _common)
        target_compile_options(${name} PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:${_opt}>)
    endforeach()
    target_compile_options(${name} PRIVATE
        $<$<COMPILE_LANGUAGE:ASM>:-mthumb>
        $<$<COMPILE_LANGUAGE:ASM>:-mcpu=${ING_CPU}>)
    if(ING_FPU)
        # 带 FPU 的芯片，其 .S（如 LiteOS 的 los_dispatch.S）需要同样的浮点选项
        target_compile_options(${name} PRIVATE
            $<$<COMPILE_LANGUAGE:ASM>:-mfpu=${ING_FPU}>
            $<$<COMPILE_LANGUAGE:ASM>:-mfloat-abi=softfp>)
    endif()

    if(ING_TOOLCHAIN_FAMILY STREQUAL "gcc")
        target_compile_options(${name} PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-deprecated>)
        # GCC 14+ 把 -Wincompatible-pointer-types 升级为错误。SDK 里存在
        # uint32_t 与 size_t 混用的回调赋值（如 noos_impl.c 的 .malloc = malloc），
        # 在 GCC 15 上直接编译失败，这里降级为警告以保持可构建。
        target_compile_options(${name} PRIVATE
            $<$<COMPILE_LANGUAGE:C>:-Wno-error=incompatible-pointer-types>)
    endif()

    # C++：CMSIS 5.9 的 cmsis_gcc.h 使用 register 关键字，
    # ISO C++17 起为错误，这里锁 gnu++14 并加保险开关。
    set_target_properties(${name} PROPERTIES
        C_STANDARD           ${ING_C_STD_NUM}
        C_STANDARD_REQUIRED  ON
        C_EXTENSIONS         ${ING_C_EXTENSIONS}
        CXX_STANDARD         14
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS       ON
    )
    target_compile_options(${name} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wno-register>)

    # ---- 链接 ----
    set(_link_common "-mthumb" "-mcpu=${ING_CPU}" "-Wl,--gc-sections")
    if(ING_FPU)
        list(APPEND _link_common "-mfpu=${ING_FPU}" "-mfloat-abi=softfp")
    endif()

    set(_symdefs "${ING_SDK_ROOT}/bundles/${_bundle_dir}/${ING_SERIES}/symdefs.g")

    if(ING_TOOLCHAIN_IS_LLVM)
        # lld 不支持 --just-symbols 读取 GNU 链接脚本格式的 symdefs.g
        # （会报 "not an ELF file"），改用链接脚本 INCLUDE 引入。
        if(NOT ARG_LDSCRIPT)
            message(FATAL_ERROR "ing_add_example(${name}) 在 LLVM 工具链下必须提供 LDSCRIPT")
        endif()
        file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${name}_link.ld"
            "INCLUDE ${_symdefs}\nINCLUDE ${ARG_LDSCRIPT}\n")
        target_link_options(${name} PRIVATE ${_link_common}
            "-T${CMAKE_CURRENT_BINARY_DIR}/${name}_link.ld"
            "-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${name}.map"
            # 等价 GCC 的 --specs=nosys.specs：提供 syscall 桩
            "-lnosys")
    else()
        set(_gcc_link
            "--specs=nano.specs" "--specs=nosys.specs"
            "-T${ARG_LDSCRIPT}"
            "-Wl,--just-symbols=${_symdefs}"
            "-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${name}.map")
        # 原 Makefile 里写的是 LFLAGS+=-u _printf_float；部分示例把它注释掉了，
        # 用 NO_PRINTF_FLOAT 保持与之一致（能省下约 8 KB 的 _dtoa_r/_vfprintf_r）。
        if(NOT ARG_NO_PRINTF_FLOAT)
            list(APPEND _gcc_link "-u" "_printf_float")
        endif()
        target_link_options(${name} PRIVATE ${_link_common} ${_gcc_link})
        # GCC 下显式链接 libstdc++ 与 libm；ATfE 的 clang 驱动会自动带入
        target_link_libraries(${name} PRIVATE stdc++ m)
    endif()

    # ---- 产物：elf + bin ----
    # 链接脚本与符号表变化必须触发重新链接
    set(_link_deps "${_symdefs}")
    if(ARG_LDSCRIPT)
        list(APPEND _link_deps "${ARG_LDSCRIPT}")
    endif()

    set_target_properties(${name} PROPERTIES
        SUFFIX ".elf"
        LINK_DEPENDS "${_link_deps}"
    )

    add_custom_command(TARGET ${name} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary
                "$<TARGET_FILE:${name}>" "${CMAKE_CURRENT_BINARY_DIR}/${name}.bin"
        COMMENT "生成 ${name}.bin"
    )
    add_custom_command(TARGET ${name} POST_BUILD
        COMMAND ${CMAKE_SIZE} "$<TARGET_FILE:${name}>"
        COMMENT "输出 ${name} 尺寸"
    )
endfunction()
