# peripheral_console_realtime 示例（无 RTOS 的串口控制台）
#
# 对应 examples/peripheral_console_realtime 的 Keil 工程。该示例原本只提供
# ING916 的启动文件，用 noos_mini bundle，因此在这里显式指定 BUNDLE。
#
# 启动文件与链接脚本按 CHIP 选择，命名由 tools/ingw_switch_chip.py 维护。

set(_startup "${CMAKE_CURRENT_SOURCE_DIR}/src/gstartup_${CHIP}00.s")
if(NOT EXISTS "${_startup}")
    message(FATAL_ERROR
        "缺少 ${CHIP} 的启动文件：${_startup}\n"
        "可运行：python tools/ingw_switch_chip.py "
        "--project examples-gcc/peripheral_console_realtime --series <SERIES>")
endif()

set(_ldscript "${CMAKE_CURRENT_SOURCE_DIR}/peripheral_console_realtime_${CHIP}.ld")
if(NOT EXISTS "${_ldscript}")
    set(_ldscript "${CMAKE_CURRENT_SOURCE_DIR}/peripheral_console_realtime.ld")
endif()

ing_add_example(peripheral_console_realtime
    SOURCES
        ${ING_SDK_ROOT}/examples/peripheral_console_realtime/src/main.c
        ${ING_SDK_ROOT}/examples/peripheral_console_realtime/src/noos_impl.c
        ${ING_SDK_ROOT}/examples/peripheral_console/src/profile.c
        ${ING_SDK_ROOT}/examples/peripheral_console/src/service_console.c
        ${ING_SDK_ROOT}/src/BSP/eflash.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_uart.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_pinctrl.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_sysctrl.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_timer.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_gpio.c
    STARTUP
        ${_startup}
    LDSCRIPT
        ${_ldscript}
    BUNDLE
        noos_mini
    DEFINES
        USE_STDPERIPH_DRIVER
)
