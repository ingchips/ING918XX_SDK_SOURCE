# 交叉编译工具链：Arm Toolchain for Embedded (ATfE, LLVM/clang + lld)
#
# 用法：
#   cmake -B build-atfe --toolchain=cmake/toolchain-atfe.cmake -DCHIP=ing918
#
# 前置条件：ATfE 主包 + ATfE-newlib-nano-overlay 已叠加安装，
# 使 <ATFE_ROOT>/bin/newlib-nano.cfg 与 <ATFE_ROOT>/lib/clang-runtimes/newlib-nano 存在。
#
# 可用 -DATFE_ROOT=<路径> 或环境变量 ATFE_ROOT 指定安装根目录。

set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

if(NOT DEFINED ATFE_ROOT)
    if(DEFINED ENV{ATFE_ROOT})
        set(ATFE_ROOT "$ENV{ATFE_ROOT}")
    elseif(WIN32)
        set(ATFE_ROOT "C:/gcc/ATfE-23.1.0-Windows-x86_64")
    else()
        set(ATFE_ROOT "/opt/ATfE-23.1.0")
    endif()
endif()

# try_compile 会在子工程里重新执行本文件，但子工程不继承命令行 -D 变量。
# 不显式声明的话，ATFE_ROOT 会退回下面的默认值，配置阶段直接失败。
set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES ATFE_ROOT)

if(NOT EXISTS "${ATFE_ROOT}/bin/newlib-nano.cfg")
    message(FATAL_ERROR
        "未找到 ${ATFE_ROOT}/bin/newlib-nano.cfg。"
        "请确认已把 ATfE-newlib-nano-overlay 解压到 ATfE 安装目录之上。")
endif()

find_program(ING_C_COMPILER   NAMES clang       HINTS "${ATFE_ROOT}/bin" NO_DEFAULT_PATH REQUIRED)
find_program(ING_CXX_COMPILER NAMES clang++     HINTS "${ATFE_ROOT}/bin" NO_DEFAULT_PATH REQUIRED)
find_program(ING_ASM_COMPILER NAMES clang       HINTS "${ATFE_ROOT}/bin" NO_DEFAULT_PATH REQUIRED)
find_program(ING_AR           NAMES llvm-ar     HINTS "${ATFE_ROOT}/bin" NO_DEFAULT_PATH REQUIRED)
find_program(ING_OBJCOPY      NAMES llvm-objcopy HINTS "${ATFE_ROOT}/bin" NO_DEFAULT_PATH REQUIRED)
find_program(ING_SIZE         NAMES llvm-size   HINTS "${ATFE_ROOT}/bin" NO_DEFAULT_PATH REQUIRED)

set(CMAKE_C_COMPILER   "${ING_C_COMPILER}")
set(CMAKE_CXX_COMPILER "${ING_CXX_COMPILER}")
set(CMAKE_ASM_COMPILER "${ING_ASM_COMPILER}")
set(CMAKE_AR           "${ING_AR}")
set(CMAKE_OBJCOPY      "${ING_OBJCOPY}")
set(CMAKE_SIZE         "${ING_SIZE}")

# 目标三元组与库选择：
#   --target=arm-none-eabi        裸机 Arm
#   --config=newlib-nano.cfg      切换到 newlib-nano sysroot（等价 GCC 的 --specs=nano.specs）
# 无 semihosting 是 ATfE 默认行为，不需要 GCC 的 --specs=nosys.specs。
set(_atfe_common "--target=arm-none-eabi")
set(_atfe_libc   "--config=${ATFE_ROOT}/bin/newlib-nano.cfg")

set(CMAKE_C_FLAGS_INIT          "${_atfe_common} ${_atfe_libc}")
set(CMAKE_CXX_FLAGS_INIT        "${_atfe_common} ${_atfe_libc}")
set(CMAKE_ASM_FLAGS_INIT        "${_atfe_common}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${_atfe_common} ${_atfe_libc}")

# 裸机目标：编译器自检只做编译，不做链接
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 供 ing_example.cmake 区分工具链行为
set(ING_TOOLCHAIN_FAMILY  "llvm" CACHE INTERNAL "")
set(ING_TOOLCHAIN_IS_LLVM ON     CACHE INTERNAL "")
