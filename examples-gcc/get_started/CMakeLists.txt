# get_started 示例（原 examples-gcc/get_started/Makefile 的 CMake 等价物）
#
# 源文件列表来自 makefile.conf 的 PROJECT_FILES。

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
        src/gstartup_ing91800.s
    LDSCRIPT
        ${CMAKE_CURRENT_SOURCE_DIR}/get_started.ld
    DEFINES
        USE_STDPERIPH_DRIVER
        I2C_PORT=I2C_PORT_0
)
