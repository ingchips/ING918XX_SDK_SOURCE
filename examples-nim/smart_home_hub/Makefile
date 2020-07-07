CMSIS_INC=${CMSIS_BASE}/Include
ING_REL=../..
ING_BUNDLE=../../bundles/typical/inc
SYMDEFS=-Wl,--just-symbols=../../bundles/typical/symdefs.g

APPNAME=smart_home_hub

STARTUP=src/gstartup_ing91800.s

APPDEFS=-D USE_STDPERIPH_DRIVER

PROJ_C_FILES=
PROJ_C_FILES += ${ING_REL}/src/FWlib/peripheral_uart.c
PROJ_C_FILES += ${ING_REL}/src/Tools/malloc_override.c

include makefile.conf
