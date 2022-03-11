CMSIS_INC="${CMSIS_BASE}/Include"
ING_REL=../..
ING_BUNDLE=../../bundles/typical/inc
SYMDEFS=-Wl,--just-symbols=../../bundles/typical/ING9187xx/symdefs.g

APPNAME=peripheral_ibeacon

STARTUP=src/gstartup_ing91800.s

APPDEFS=-DUSE_STDPERIPH_DRIVER

PROJECT_FILES  = src/main.c
PROJECT_FILES += ../../examples/${APPNAME}/src/profile.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_uart.c

include makefile.conf
