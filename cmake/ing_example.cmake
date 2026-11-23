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
# 新增芯片家族时在这里补三行：CPU、FPU、BUNDLE。
# BUNDLE 对应 bundles/typical/ 下的目录名，决定 ROM 符号表。
# ---------------------------------------------------------------------------
set(ING_CHIP_ing918_CPU    cortex-m3)
set(ING_CHIP_ing918_FPU    "")
set(ING_CHIP_ing918_BUNDLE ING9187xx)

set(ING_CHIP_ing916_CPU    cortex-m4)
set(ING_CHIP_ing916_FPU    fpv4-sp-d16)
set(ING_CHIP_ing916_BUNDLE ING9168xx)

# ING20（ING208xx）：Cortex-M3，与 ING918 同核但 ROM/RAM 布局不同。
# 注意系列名是 ING208xx；bundles/*/ING20xx 是残缺的旧目录，不可用。
set(ING_CHIP_ing20_CPU    cortex-m3)
set(ING_CHIP_ing20_FPU    "")
set(ING_CHIP_ing20_BUNDLE ING208xx)

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
set(ING_BUNDLE "${ING_CHIP_${CHIP}_BUNDLE}")

# C 语言标准，可用 -DING_C_STANDARD=gnu17 覆盖
# 取值 gnu99 / gnu11 / gnu17；扩展由 C_EXTENSIONS 打开，
# CMake 的 C_STANDARD 属性只接受版本号，故这里拆出数字。
if(NOT DEFINED ING_C_STANDARD)
    set(ING_C_STANDARD gnu11 CACHE STRING "C 标准：gnu99 / gnu11 / gnu17")
endif()
if(NOT ING_C_STANDARD MATCHES "^(gnu|c)(99|11|17|23)$")
    message(FATAL_ERROR "不支持的 ING_C_STANDARD=${ING_C_STANDARD}，可选 gnu99 / gnu11 / gnu17")
endif()
string(REGEX REPLACE "^(gnu|c)" "" ING_C_STD_NUM "${ING_C_STANDARD}")

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
# )
# ---------------------------------------------------------------------------
function(ing_add_example name)
    cmake_parse_arguments(ARG "" "STARTUP;LDSCRIPT" "SOURCES;DEFINES;INCLUDES" ${ARGN})

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "ing_add_example(${name}) 缺少 SOURCES")
    endif()

    add_executable(${name} ${ARG_SOURCES})
    if(ARG_STARTUP)
        target_sources(${name} PRIVATE ${ARG_STARTUP})
    endif()

    # ---- include 路径 ----
    target_include_directories(${name} PRIVATE
        "${ING_SDK_ROOT}/src/BSP"
        "${ING_SDK_ROOT}/src/FWlib"
        "${ING_SDK_ROOT}/src/StartUp/${CHIP}"
        "${ING_SDK_ROOT}/src/Tools"
        "${ING_SDK_ROOT}/src"
        "${ING_SDK_ROOT}/bundles/typical/inc"
        "${ING_SDK_ROOT}/bundles/typical/inc/os"
        "${ING_CMSIS}/Include"
        ${ARG_INCLUDES}
    )

    # ---- 编译选项 ----
    # 宏定义只作用于 C/C++：.s 启动文件不经预处理器，给汇编传 -D 无效。
    target_compile_definitions(${name} PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:__STARTUP_CLEAR_BSS>)
    foreach(_def IN LISTS ARG_DEFINES)
        target_compile_definitions(${name} PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:${_def}>)
    endforeach()

    set(_common "-mthumb" "-mcpu=${ING_CPU}" "-O0"
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

    if(ING_TOOLCHAIN_FAMILY STREQUAL "gcc")
        target_compile_options(${name} PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-deprecated>)
    endif()

    # C++：CMSIS 5.9 的 cmsis_gcc.h 使用 register 关键字，
    # ISO C++17 起为错误，这里锁 gnu++14 并加保险开关。
    set_target_properties(${name} PROPERTIES
        C_STANDARD           ${ING_C_STD_NUM}
        C_STANDARD_REQUIRED  ON
        C_EXTENSIONS         ON
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

    set(_symdefs "${ING_SDK_ROOT}/bundles/typical/${ING_BUNDLE}/symdefs.g")

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
        target_link_options(${name} PRIVATE ${_link_common}
            "--specs=nano.specs" "--specs=nosys.specs"
            "-T${ARG_LDSCRIPT}"
            "-Wl,--just-symbols=${_symdefs}"
            "-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${name}.map"
            "-u" "_printf_float")
        # GCC 下显式链接 libstdc++；ATfE 的 clang 驱动会自动带入 libc++
        target_link_libraries(${name} PRIVATE stdc++)
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
