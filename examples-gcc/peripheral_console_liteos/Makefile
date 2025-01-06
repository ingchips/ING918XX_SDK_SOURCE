CMSIS_INC=${CMSIS_BASE}/Include
ING_REL=../..
LITEOS_BASE=./liteos_m
ING_BUNDLE += ../../bundles/noos_typical/inc

CHIP_FAMILY = ing918
CHIP_FAMILY_PREFIX = ing

OS_INC += -I $(LITEOS_BASE)/kernel/include
OS_INC += -I $(LITEOS_BASE)/utils
OS_INC += -I $(LITEOS_BASE)/arch/include
OS_INC += -I $(LITEOS_BASE)/lib/libsec/include
OS_INC += -I $(LITEOS_BASE)/arch/arm/common
OS_INC += -I $(LITEOS_BASE)/utils
OS_INC += -I $(LITEOS_BASE)/utils/internal
OS_INC += -I $(LITEOS_BASE)/lib/libsec/src/
OS_INC += -I $(LITEOS_BASE)/lib/libsec/src/include
OS_INC += -I $(LITEOS_BASE)/components/signal
OS_INC += -I $(LITEOS_BASE)/components/power

APP_INC = -I ../../examples/peripheral_console/src -I ./src

SYMDEFS=-Wl,--just-symbols=../../bundles/noos_typical/ING9187xx/symdefs.g

APPNAME=peripheral_console_liteos

STARTUP = src/gstartup_$(CHIP_FAMILY)00.s

APPDEFS=-D USE_STDPERIPH_DRIVER -DPOWER_SAVING -DLISTEN_TO_POWER_SAVING

PROJECT_FILES = ./src/main.c
PROJECT_FILES += ./src/gen_os_impl.c
PROJECT_FILES += ../../examples/peripheral_console/src/profile.c
PROJECT_FILES += ../../examples/peripheral_console/src/service_console.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_gpio.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_pinctrl.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_sysctrl.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_timer.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_uart.c
PROJECT_FILES += ${ING_REL}/src/Tools/trace.c
PROJECT_FILES += ${ING_REL}/src/BSP/eflash.c

OS_FILES += $(LITEOS_BASE)/kernel/src/los_event.c
OS_FILES += $(LITEOS_BASE)/kernel/src/los_init.c
OS_FILES += $(LITEOS_BASE)/kernel/src/los_mux.c
OS_FILES += $(LITEOS_BASE)/kernel/src/los_queue.c
OS_FILES += $(LITEOS_BASE)/kernel/src/los_sched.c
OS_FILES += $(LITEOS_BASE)/kernel/src/los_sem.c
OS_FILES += $(LITEOS_BASE)/kernel/src/los_sortlink.c
OS_FILES += $(LITEOS_BASE)/kernel/src/los_swtmr.c
OS_FILES += $(LITEOS_BASE)/kernel/src/los_task.c
OS_FILES += $(LITEOS_BASE)/kernel/src/los_tick.c
OS_FILES += $(LITEOS_BASE)/kernel/src/mm/los_membox.c
OS_FILES += $(LITEOS_BASE)/kernel/src/mm/los_memory.c

OS_FILES += $(LITEOS_BASE)/arch/arm/common/los_common_interrupt.c

OS_FILES += $(LITEOS_BASE)/utils/los_debug.c
OS_FILES += $(LITEOS_BASE)/utils/los_error.c
OS_FILES += $(LITEOS_BASE)/utils/los_hook.c
OS_FILES += $(LITEOS_BASE)/lib/libsec/src/memcpy_s.c
OS_FILES += $(LITEOS_BASE)/lib/libsec/src/memset_s.c
OS_FILES += $(LITEOS_BASE)/lib/libsec/src/strncpy_s.c
OS_FILES += $(LITEOS_BASE)/components/signal/los_signal.c
OS_FILES += $(LITEOS_BASE)/components/power/los_pm.c

ifeq ($(CHIP_FAMILY), $(CHIP_FAMILY_PREFIX)916)
ASMS_SOURCES += $(LITEOS_BASE)/arch/arm/cortex-m4/gcc/los_dispatch.S
ASMS_SOURCES += $(LITEOS_BASE)/arch/arm/cortex-m4/gcc/los_exc.S

OS_FILES += $(LITEOS_BASE)/arch/arm/cortex-m4/gcc/los_context.c
OS_FILES += $(LITEOS_BASE)/arch/arm/cortex-m4/gcc/los_interrupt.c
OS_FILES += $(LITEOS_BASE)/arch/arm/cortex-m4/gcc/los_mpu.c
OS_FILES += $(LITEOS_BASE)/arch/arm/cortex-m4/gcc/los_timer.c

OS_INC += -I $(LITEOS_BASE)/arch/arm/cortex-m4/gcc
else ifeq ($(CHIP_FAMILY), $(CHIP_FAMILY_PREFIX)918)
ASMS_SOURCES += $(LITEOS_BASE)/arch/arm/cortex-m3/gcc/los_dispatch.S
ASMS_SOURCES += $(LITEOS_BASE)/arch/arm/cortex-m3/gcc/los_exc.S

OS_FILES += $(LITEOS_BASE)/arch/arm/cortex-m3/gcc/los_context.c
OS_FILES += $(LITEOS_BASE)/arch/arm/cortex-m3/gcc/los_interrupt.c
OS_FILES += $(LITEOS_BASE)/arch/arm/cortex-m3/gcc/los_mpu.c
OS_FILES += $(LITEOS_BASE)/arch/arm/cortex-m3/gcc/los_timer.c

OS_INC += -I $(LITEOS_BASE)/arch/arm/cortex-m3/gcc
else
$(error unknown CHIP_FAMILY)
endif

include makefile.conf
