# 交叉编译工具链：Arm GNU Toolchain (arm-none-eabi-gcc)
#
# 用法：
#   cmake -B build --toolchain=cmake/toolchain-arm-none-eabi.cmake -DCHIP=ing918
#
# 可用 -DARM_GCC_ROOT=<路径> 或环境变量 ARM_GCC_ROOT 指定工具链根目录。

set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

if(NOT DEFINED ARM_GCC_ROOT)
    if(DEFINED ENV{ARM_GCC_ROOT})
        set(ARM_GCC_ROOT "$ENV{ARM_GCC_ROOT}")
    elseif(WIN32)
        set(ARM_GCC_ROOT "C:/gcc/gcc_arm/arm-none-eabi-toolchain-15.2")
    else()
        set(ARM_GCC_ROOT "/opt/arm-gcc")
    endif()
endif()

find_program(ING_C_COMPILER   NAMES arm-none-eabi-gcc     HINTS "${ARM_GCC_ROOT}/bin" REQUIRED)
find_program(ING_CXX_COMPILER NAMES arm-none-eabi-g++     HINTS "${ARM_GCC_ROOT}/bin" REQUIRED)
find_program(ING_ASM_COMPILER NAMES arm-none-eabi-gcc     HINTS "${ARM_GCC_ROOT}/bin" REQUIRED)
find_program(ING_AR           NAMES arm-none-eabi-ar      HINTS "${ARM_GCC_ROOT}/bin" REQUIRED)
find_program(ING_OBJCOPY      NAMES arm-none-eabi-objcopy HINTS "${ARM_GCC_ROOT}/bin" REQUIRED)
find_program(ING_SIZE         NAMES arm-none-eabi-size    HINTS "${ARM_GCC_ROOT}/bin" REQUIRED)

set(CMAKE_C_COMPILER   "${ING_C_COMPILER}")
set(CMAKE_CXX_COMPILER "${ING_CXX_COMPILER}")
set(CMAKE_ASM_COMPILER "${ING_ASM_COMPILER}")
set(CMAKE_AR           "${ING_AR}")
set(CMAKE_OBJCOPY      "${ING_OBJCOPY}")
set(CMAKE_SIZE         "${ING_SIZE}")

# 裸机目标：编译器自检只做编译，不做链接
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 供 ing_example.cmake 区分工具链行为
set(ING_TOOLCHAIN_FAMILY  "gcc" CACHE INTERNAL "")
set(ING_TOOLCHAIN_IS_LLVM OFF   CACHE INTERNAL "")
