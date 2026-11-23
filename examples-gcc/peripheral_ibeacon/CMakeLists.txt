# peripheral_ibeacon 示例（iBeacon 广播）
#
# 源文件列表来自 examples-gcc/peripheral_ibeacon/Makefile 的 PROJECT_FILES。
# 启动文件与链接脚本按 CHIP 选择，命名由 tools/ingw_switch_chip.py 维护。

set(_startup "${CMAKE_CURRENT_SOURCE_DIR}/src/gstartup_${CHIP}00.s")
if(NOT EXISTS "${_startup}")
    message(FATAL_ERROR
        "缺少 ${CHIP} 的启动文件：${_startup}\n"
        "可运行：python tools/ingw_switch_chip.py "
        "--project examples-gcc/peripheral_ibeacon --series <SERIES>")
endif()

set(_ldscript "${CMAKE_CURRENT_SOURCE_DIR}/peripheral_ibeacon_${CHIP}.ld")
if(NOT EXISTS "${_ldscript}")
    set(_ldscript "${CMAKE_CURRENT_SOURCE_DIR}/peripheral_ibeacon.ld")
endif()

ing_add_example(peripheral_ibeacon
    SOURCES
        src/main.c
        ${ING_SDK_ROOT}/examples/peripheral_ibeacon/src/profile.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_uart.c
    STARTUP
        ${_startup}
    LDSCRIPT
        ${_ldscript}
    DEFINES
        USE_STDPERIPH_DRIVER
)
