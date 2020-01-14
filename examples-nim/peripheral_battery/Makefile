CMSIS_INC=${CMSIS_BASE}/Include
ING_REL=../..
ING_BUNDLE=../../bundles/typical/inc
SYMDEFS=-Wl,--just-symbols=../../bundles/typical/symdefs.g

APPNAME=peripheral_battery

STARTUP=src/gstartup_ing91800.s

APPDEFS=-D USE_STDPERIPH_DRIVER

PROJ_C_FILES=
PROJ_C_FILES+=${ING_REL}/src/FWlib/cm32gpm3_uart.c

include makefile.conf
