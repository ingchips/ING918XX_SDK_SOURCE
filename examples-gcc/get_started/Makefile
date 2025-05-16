CMSIS_INC="${CMSIS_BASE}/Include"
ING_REL=../..
ING_BUNDLE=../../bundles/typical/inc
SYMDEFS=-Wl,--just-symbols=../../bundles/typical/ING9187xx/symdefs.g

APPNAME=get_started

STARTUP=src/gstartup_ing91800.s

APPDEFS=-D USE_STDPERIPH_DRIVER -D BOARD_USE_KEYS -D BOARD_USE_RGB_LED -D BOARD_USE_THERMO -D BOARD_USE_ACCEL -D BOARD_ID=2 -D I2C_PORT=I2C_PORT_0

PROJECT_FILES  = src/main.c
PROJECT_FILES += ../../examples/get_started/src/profile.c
PROJECT_FILES += ../../examples/get_started/src/container.cpp
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_uart.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_pinctrl.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_sysctrl.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_timer.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_gpio.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_pwm.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_adc.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_i2c.c
PROJECT_FILES += ${ING_REL}/src/BSP/iic.c
PROJECT_FILES += ${ING_REL}/src/BSP/board.c
PROJECT_FILES += ${ING_REL}/src/Tools/att_db_util.c

include makefile.conf
