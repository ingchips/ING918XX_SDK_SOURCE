CMSIS_INC=${CMSIS_BASE}/Include
ING_REL=../..
ING_BUNDLE=../../bundles/typical/inc

APPNAME=peripheral_ibeacon

PROJECT_FILES=src/main.c ../../examples/${APPNAME}/src/profile.c   ${ING_REL}/src/FWlib/peripheral_uart.c   

SYMDEFS=-Wl,--just-symbols=../../bundles/typical/symdefs.g

include makefile.conf
