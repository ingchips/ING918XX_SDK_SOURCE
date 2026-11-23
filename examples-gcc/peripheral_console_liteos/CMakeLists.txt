# peripheral_console_liteos 示例（LiteOS-M 上的串口控制台）
#
# 源文件列表来自 examples-gcc/peripheral_console_liteos/Makefile 的
# PROJECT_FILES / OS_FILES / ASMS_SOURCES。
#
# 该工程用 noos_typical bundle，并且用 ING916（Cortex-M4F）。LiteOS 的
# 架构目录按芯片切换：916 → cortex-m4，918/20 → cortex-m3。

set(_liteos "${CMAKE_CURRENT_SOURCE_DIR}/liteos_m")

if(CHIP STREQUAL "ing916")
    set(_arch_dir "${_liteos}/arch/arm/cortex-m4/gcc")
else()
    set(_arch_dir "${_liteos}/arch/arm/cortex-m3/gcc")
endif()

set(_startup "${CMAKE_CURRENT_SOURCE_DIR}/src/gstartup_${CHIP}00.s")
if(NOT EXISTS "${_startup}")
    message(FATAL_ERROR
        "缺少 ${CHIP} 的启动文件：${_startup}\n"
        "可运行：python tools/ingw_switch_chip.py "
        "--project examples-gcc/peripheral_console_liteos --series <SERIES>")
endif()

set(_ldscript "${CMAKE_CURRENT_SOURCE_DIR}/peripheral_console_liteos_${CHIP}.ld")
if(NOT EXISTS "${_ldscript}")
    set(_ldscript "${CMAKE_CURRENT_SOURCE_DIR}/peripheral_console_liteos.ld")
endif()

set(_liteos_sources
    # kernel
    ${_liteos}/kernel/src/los_event.c
    ${_liteos}/kernel/src/los_init.c
    ${_liteos}/kernel/src/los_mux.c
    ${_liteos}/kernel/src/los_queue.c
    ${_liteos}/kernel/src/los_sched.c
    ${_liteos}/kernel/src/los_sem.c
    ${_liteos}/kernel/src/los_sortlink.c
    ${_liteos}/kernel/src/los_swtmr.c
    ${_liteos}/kernel/src/los_task.c
    ${_liteos}/kernel/src/los_tick.c
    ${_liteos}/kernel/src/mm/los_membox.c
    ${_liteos}/kernel/src/mm/los_memory.c
    # arch common + 架构相关
    ${_liteos}/arch/arm/common/los_common_interrupt.c
    ${_arch_dir}/los_context.c
    ${_arch_dir}/los_interrupt.c
    ${_arch_dir}/los_mpu.c
    ${_arch_dir}/los_timer.c
    # utils
    ${_liteos}/utils/los_debug.c
    ${_liteos}/utils/los_error.c
    ${_liteos}/utils/los_hook.c
    # libsec
    ${_liteos}/lib/libsec/src/memcpy_s.c
    ${_liteos}/lib/libsec/src/memset_s.c
    ${_liteos}/lib/libsec/src/strncpy_s.c
    # components
    ${_liteos}/components/signal/los_signal.c
    ${_liteos}/components/power/los_pm.c
)

set(_liteos_asm
    ${_arch_dir}/los_dispatch.S
    ${_arch_dir}/los_exc.S
)

ing_add_example(peripheral_console_liteos
    SOURCES
        src/main.c
        src/gen_os_impl.c
        ${ING_SDK_ROOT}/examples/peripheral_console/src/profile.c
        ${ING_SDK_ROOT}/examples/peripheral_console/src/service_console.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_gpio.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_pinctrl.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_sysctrl.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_timer.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_uart.c
        ${ING_SDK_ROOT}/src/Tools/trace.c
        ${ING_SDK_ROOT}/src/BSP/eflash.c
        ${_liteos_sources}
        ${_liteos_asm}
    STARTUP
        ${_startup}
    LDSCRIPT
        ${_ldscript}
    BUNDLE
        noos_typical
    # 原 Makefile 的 release 目标用 -O3，且把 -u _printf_float 注释掉了
    OPTIMIZE
        -O3
    NO_PRINTF_FLOAT
    DEFINES
        USE_STDPERIPH_DRIVER
        POWER_SAVING
        LISTEN_TO_POWER_SAVING
    INCLUDES
        ${_liteos}/kernel/include
        ${_liteos}/utils
        ${_liteos}/utils/internal
        ${_liteos}/arch/include
        ${_liteos}/arch/arm/common
        ${_liteos}/lib/libsec/include
        ${_liteos}/lib/libsec/src
        ${_liteos}/lib/libsec/src/include
        ${_liteos}/components/signal
        ${_liteos}/components/power
        ${_arch_dir}
        ${ING_SDK_ROOT}/examples/peripheral_console/src
        # target_config.h 在本工程 src/ 下，makefile.conf 的 APP_INC 含 ./src
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
