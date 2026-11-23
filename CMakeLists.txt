cmake_minimum_required(VERSION 3.20)

# 顶层工程：只负责组织示例，具体构建规则在 cmake/ing_example.cmake。
# 工具链由 --toolchain=cmake/toolchain-*.cmake 在配置阶段选定。
project(ing_sdk_examples LANGUAGES C CXX ASM)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ing_example.cmake")

# 要构建的示例列表，可用 -DING_EXAMPLES="get_started;peripheral_ibeacon" 覆盖
if(NOT DEFINED ING_EXAMPLES)
    set(ING_EXAMPLES "get_started" CACHE STRING "要构建的示例列表，分号分隔")
endif()

set(_built 0)
foreach(_ex IN LISTS ING_EXAMPLES)
    set(_dir "${CMAKE_CURRENT_SOURCE_DIR}/examples-gcc/${_ex}")
    if(EXISTS "${_dir}/CMakeLists.txt")
        add_subdirectory("${_dir}" "${CMAKE_BINARY_DIR}/${_ex}")
        math(EXPR _built "${_built} + 1")
    else()
        message(WARNING "跳过 ${_ex}：缺少 ${_dir}/CMakeLists.txt")
    endif()
endforeach()

if(_built EQUAL 0)
    message(FATAL_ERROR "没有任何示例被构建，请检查 ING_EXAMPLES=${ING_EXAMPLES}")
endif()

message(STATUS "ING 构建配置：CHIP=${CHIP} C标准=${ING_C_STANDARD} 工具链=${ING_TOOLCHAIN_FAMILY}")
