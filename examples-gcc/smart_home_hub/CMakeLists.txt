# smart_home_hub 示例（Nim + FreeRTOS）
#
# 工程用 Nim 写业务代码：configure 阶段先让 nim 生成 C 源码到 nimcache/，
# 再把这些 .c 与少量 C 源文件一起编译链接。
#
# 依赖：nim（原 Makefile 通过 NIM_PATH 指定），缺少时跳过本示例。

find_program(ING_NIM NAMES nim HINTS "$ENV{NIM_PATH}")
if(NOT ING_NIM)
    message(WARNING
        "跳过 smart_home_hub：未找到 nim 可执行文件。"
        "该示例的 src/main.nim 需要 nim 生成 C 源码（设置 NIM_PATH 或安装 nim）。")
    return()
endif()

set(_nimcache "${CMAKE_CURRENT_BINARY_DIR}/nimcache")
set(_nimflags --compileOnly --noMain --noLinking "--nimcache:${_nimcache}")

# nim.cfg 里的 --path 是相对路径，工作目录必须是工程目录
execute_process(
    COMMAND ${ING_NIM} c ${_nimflags} src/main.nim
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    RESULT_VARIABLE _nim_rc
    OUTPUT_VARIABLE _nim_out
    ERROR_VARIABLE  _nim_err
)
if(NOT _nim_rc EQUAL 0)
    message(FATAL_ERROR "nim 代码生成失败（exit ${_nim_rc}）：\n${_nim_err}")
endif()

file(GLOB _nim_c "${_nimcache}/*.c")
list(LENGTH _nim_c _nim_count)
if(_nim_count EQUAL 0)
    message(FATAL_ERROR "nim 未在 ${_nimcache} 生成任何 .c 文件")
endif()
message(STATUS "smart_home_hub: nim 生成了 ${_nim_count} 个 C 文件")

set(_startup "${CMAKE_CURRENT_SOURCE_DIR}/src/gstartup_${CHIP}00.s")
if(NOT EXISTS "${_startup}")
    message(FATAL_ERROR
        "缺少 ${CHIP} 的启动文件：${_startup}\n"
        "可运行：python tools/ingw_switch_chip.py "
        "--project examples-gcc/smart_home_hub --series <SERIES>")
endif()

set(_ldscript "${CMAKE_CURRENT_SOURCE_DIR}/smart_home_hub_${CHIP}.ld")
if(NOT EXISTS "${_ldscript}")
    set(_ldscript "${CMAKE_CURRENT_SOURCE_DIR}/smart_home_hub.ld")
endif()

ing_add_example(smart_home_hub
    SOURCES
        ${_nim_c}
        ${ING_SDK_ROOT}/src/FWlib/peripheral_uart.c
        ${ING_SDK_ROOT}/src/Tools/malloc_override.c
    STARTUP
        ${_startup}
    LDSCRIPT
        ${_ldscript}
    DEFINES
        USE_STDPERIPH_DRIVER
    INCLUDES
        # nim 生成的 C 代码按 nimmodules 里的声明引用 SDK 头
        ${CMAKE_CURRENT_SOURCE_DIR}/nimmodules
)
