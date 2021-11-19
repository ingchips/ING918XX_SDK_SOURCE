CMSIS_INC="${CMSIS_BASE}/Include"
ING_REL=../..
ING_BUNDLE=../../bundles/typical/inc
SYMDEFS=-Wl,--just-symbols=../../bundles/typical/ING9187xx/symdefs.g

APPNAME=central_fota

STARTUP=src/gstartup_ing91800.s

APPDEFS=-D USE_STDPERIPH_DRIVER

PROJECT_FILES = src/main.c
PROJECT_FILES += src/profile.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_uart.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_pinctrl.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_sysctrl.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_timer.c
PROJECT_FILES += ${ING_REL}/src/Tools/trace.c
PROJECT_FILES += ${ING_REL}/src/Tools/platform_tools.c
PROJECT_FILES += ${ING_REL}/src/Tools/gatt_client_util.c
PROJECT_FILES += ${ING_REL}/src/Tools/malloc_override.c

PROJECT_FILES += gatt_client_async_helper.c sec_fota_utils.c

ZIG_FILES = src/fota_client.zig

ZCFLAGS= -Dprint_info=1 -O ReleaseSmall

include makefile.conf
