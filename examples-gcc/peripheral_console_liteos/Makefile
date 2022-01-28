CMSIS_INC="${CMSIS_BASE}/Include"
ING_REL=../..
ING_BUNDLE=../../bundles/noos_typical/inc
SYMDEFS=-Wl,--just-symbols=../../bundles/noos_typical/ING9187xx/symdefs.g

LITEOS_BASE=$(ING_REL)/src/LiteOS

APPNAME=peripheral_console_liteos

STARTUP=src/gstartup_ing91800.s

APPDEFS=-D USE_STDPERIPH_DRIVER -D LOSCFG_ARCH_CORTEX_M3 -D LISTEN_TO_POWER_SAVING -D POWER_SAVING

PROJECT_FILES  = src/main.c
PROJECT_FILES += ../../examples/peripheral_console/src/profile.c
PROJECT_FILES += ../../examples/peripheral_console/src/service_console.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_uart.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_pinctrl.c
PROJECT_FILES += ${ING_REL}/src/FWlib/peripheral_sysctrl.c
PROJECT_FILES += ${LITEOS_BASE}/gen_os_impl.c

APP_INC = -I ../../examples/peripheral_console/src -I ./src

LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_bitmap.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_err.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_event.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_lockdep.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_misc.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_mp.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_mux.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_percpu.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_queue.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_ringbuf.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_sem.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_sortlink.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_stackinfo.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_swtmr.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_task.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_tick.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/los_heap.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/mem/bestfit_little/los_memory.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/sched/sched_sq/los_priqueue.c
LITEOS_FILES += ${LITEOS_BASE}/kernel/base/sched/sched_sq/los_sched.c
LITEOS_FILES += ${LITEOS_BASE}/lib/libsec/src/memcpy_s.c
LITEOS_FILES += ${LITEOS_BASE}/lib/libsec/src/memset_s.c
LITEOS_FILES += ${LITEOS_BASE}/lib/libsec/src/strncpy_s.c
LITEOS_FILES += ${LITEOS_BASE}/arch/arm/cortex_m/src/cpu.c
LITEOS_FILES += ${LITEOS_BASE}/arch/arm/cortex_m/src/fpb.c
LITEOS_FILES += ${LITEOS_BASE}/arch/arm/cortex_m/src/task.c

LITEOS_ASM_FILES += ${LITEOS_BASE}/arch/arm/cortex_m/src/dispatch.S
LITEOS_ASM_FILES += ${LITEOS_BASE}/arch/arm/cortex_m/s/exception.S

LITEOS_INC += -I ${LITEOS_BASE}
LITEOS_INC += -I ${LITEOS_BASE}/kernel/include
LITEOS_INC += -I ${LITEOS_BASE}/kernel/base/include
LITEOS_INC += -I ${LITEOS_BASE}/arch/arm/cortex_m/include
LITEOS_INC += -I ${LITEOS_BASE}/targets/bsp/include
LITEOS_INC += -I ${LITEOS_BASE}/lib/libsec/include

PROJECT_FILES += $(LITEOS_FILES)

include makefile.conf
