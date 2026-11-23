# get_started 示例（原 examples-gcc/get_started/Makefile 的 CMake 等价物）
#
# 源文件列表来自 makefile.conf 的 PROJECT_FILES。
# 启动文件与链接脚本按 CHIP 选择，命名由 tools/ingw_switch_chip.py 维护：
#   src/gstartup_<family>00.s     每个 family 一份，可并存
#   get_started_<family>.ld       每个 family 一份；缺省回落到 get_started.ld

set(_startup "${CMAKE_CURRENT_SOURCE_DIR}/src/gstartup_${CHIP}00.s")
if(NOT EXISTS "${_startup}")
    message(FATAL_ERROR
        "缺少 ${CHIP} 的启动文件：${_startup}\n"
        "可运行：python tools/ingw_switch_chip.py --project examples-gcc/get_started --series <SERIES>")
endif()

set(_ldscript "${CMAKE_CURRENT_SOURCE_DIR}/get_started_${CHIP}.ld")
if(NOT EXISTS "${_ldscript}")
    set(_ldscript "${CMAKE_CURRENT_SOURCE_DIR}/get_started.ld")
endif()

ing_add_example(get_started
    SOURCES
        src/main.c
        ${ING_SDK_ROOT}/examples/get_started/src/profile.c
        ${ING_SDK_ROOT}/examples/get_started/src/container.cpp
        ${ING_SDK_ROOT}/src/FWlib/peripheral_uart.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_pinctrl.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_sysctrl.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_timer.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_gpio.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_pwm.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_adc.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_i2c.c
        ${ING_SDK_ROOT}/src/BSP/iic.c
        ${ING_SDK_ROOT}/src/Tools/att_db_util.c
    STARTUP
        ${_startup}
    LDSCRIPT
        ${_ldscript}
    DEFINES
        USE_STDPERIPH_DRIVER
        I2C_PORT=I2C_PORT_0
)
