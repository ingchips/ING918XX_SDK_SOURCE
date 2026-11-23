# central_fota 示例（BLE OTA 客户端）
#
# 源文件列表来自 examples-gcc/central_fota/Makefile 的 PROJECT_FILES。
# 该工程额外用 Zig 编译 src/fota_client.zig，产出一个 .o 参与链接。

find_program(ING_ZIG NAMES zig)
if(NOT ING_ZIG)
    message(WARNING
        "跳过 central_fota：未找到 zig 可执行文件。"
        "该示例的 src/fota_client.zig 需要 zig 编译（CI 会安装 zig 0.8.1）。")
    return()
endif()

set(_startup "${CMAKE_CURRENT_SOURCE_DIR}/src/gstartup_${CHIP}00.s")
if(NOT EXISTS "${_startup}")
    message(FATAL_ERROR
        "缺少 ${CHIP} 的启动文件：${_startup}\n"
        "可运行：python tools/ingw_switch_chip.py "
        "--project examples-gcc/central_fota --series <SERIES>")
endif()

set(_ldscript "${CMAKE_CURRENT_SOURCE_DIR}/central_fota_${CHIP}.ld")
if(NOT EXISTS "${_ldscript}")
    set(_ldscript "${CMAKE_CURRENT_SOURCE_DIR}/central_fota.ld")
endif()

# ---- Zig：fota_client.zig -> fota_client.o ----
# 参数对齐 makefile.conf 的 ZCFLAGS：
#   -Dprint_info=1 -O ReleaseSmall -target thumb-freestanding-none -mcpu cortex_m3
set(_zig_out "${CMAKE_CURRENT_BINARY_DIR}/fota_client.o")
set(_zig_mcpu "cortex_m3")
if(CHIP STREQUAL "ing916")
    set(_zig_mcpu "cortex_m4")
endif()

add_custom_command(
    OUTPUT "${_zig_out}"
    COMMAND ${ING_ZIG} build-obj
            -Dprint_info=1 -O ReleaseSmall
            -target thumb-freestanding-none
            -mcpu ${_zig_mcpu}
            -I "${CMAKE_CURRENT_SOURCE_DIR}/src"
            -I "${ING_SDK_ROOT}/src/BSP"
            -I "${ING_SDK_ROOT}/src/FWlib"
            -I "${ING_SDK_ROOT}/src/StartUP/${CHIP}"
            -I "${ING_SDK_ROOT}/src/Tools"
            -I "${ING_SDK_ROOT}/src"
            -I "${ING_SDK_ROOT}/bundles/${ING_BUNDLE_DIR}/inc"
            -I "${ING_SDK_ROOT}/bundles/${ING_BUNDLE_DIR}/inc/os"
            -I "${ING_CMSIS}/Include"
            --cache-dir "${CMAKE_CURRENT_BINARY_DIR}/zig-cache"
            -femit-bin=${_zig_out}
            "${CMAKE_CURRENT_SOURCE_DIR}/src/fota_client.zig"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/fota_client.zig"
    COMMENT "zig: fota_client.zig"
    VERBATIM
)

ing_add_example(central_fota
    SOURCES
        src/main.c
        src/profile.c
        src/gatt_client_async_helper.c
        src/sec_fota_utils.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_uart.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_pinctrl.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_sysctrl.c
        ${ING_SDK_ROOT}/src/FWlib/peripheral_timer.c
        ${ING_SDK_ROOT}/src/Tools/trace.c
        ${ING_SDK_ROOT}/src/Tools/platform_tools.c
        ${ING_SDK_ROOT}/src/Tools/platform_util.c
        ${ING_SDK_ROOT}/src/Tools/gatt_client_util.c
        ${ING_SDK_ROOT}/src/Tools/malloc_override.c
    STARTUP
        ${_startup}
    LDSCRIPT
        ${_ldscript}
    DEFINES
        USE_STDPERIPH_DRIVER
        TARGET_FAMILY=0
    # 原 Makefile 用 -Os
    OPTIMIZE
        -Os
)

# 把 zig 产物挂到目标上（外部 .o 直接参与链接）
target_sources(central_fota PRIVATE "${_zig_out}")
set_source_files_properties("${_zig_out}" PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE)
