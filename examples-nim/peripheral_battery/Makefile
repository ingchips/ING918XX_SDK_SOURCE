CMSIS_INC=${CMSIS_BASE}/Include
ING_REL=../..
ING_BUNDLE=../../bundles/typical/inc
SYMDEFS=-Wl,--just-symbols=../../bundles/typical/ING9185xx/symdefs.g

APPNAME=peripheral_battery

STARTUP=src/gstartup_ing91800.s

APPDEFS=-D USE_STDPERIPH_DRIVER

PROJ_C_FILES=
PROJ_C_FILES += ${ING_REL}/src/FWlib/peripheral_uart.c
PROJ_C_FILES += ${ING_REL}/src/Tools/malloc_override.c

include makefile.conf
