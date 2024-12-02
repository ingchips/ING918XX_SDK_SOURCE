TARGET = ing916
DEBUG = 1
# optimization
OPT = -Og
# Build path
BUILD_DIR = obj
LD_SCRIPT = ing916.ld
MAP_FILE=$(TARGET).map

#APP DEFINITIONS
APPDEFS=-D USE_STDPERIPH_DRIVER -DPOWER_SAVING -DLISTEN_TO_POWER_SAVING

# please change this to your project directory
CMSIS_BASE=../../../../wizard/CMSIS
ING_REL=../../../

# project path
LITEOS_BASE=../liteos_m
ING_BUNDLE += $(ING_REL)/bundles/noos_typical/inc
ING_SYMDEF = --just-symbols=$(ING_REL)/bundles/noos_typical/ING9168xx/symdefs.g

CMSIS_INC=$(CMSIS_BASE)/Include
#you can add your own include files here
USER_INC =

#liteos include files
OS_INC += -I ./src
OS_INC += -I $(LITEOS_BASE)/kernel/include
OS_INC += -I $(LITEOS_BASE)/arch/arm/cortex-m4/gcc
OS_INC += -I $(LITEOS_BASE)/utils
OS_INC += -I $(LITEOS_BASE)/arch/include
OS_INC += -I $(LITEOS_BASE)/lib/libsec/include
OS_INC += -I $(LITEOS_BASE)/arch/arm/common
OS_INC += -I $(LITEOS_BASE)/utils
OS_INC += -I $(LITEOS_BASE)/utils/internal
OS_INC += -I $(LITEOS_BASE)/lib/libsec/src/
OS_INC += -I $(LITEOS_BASE)/lib/libsec/src/include
OS_INC += -I $(LITEOS_BASE)/components/signal

#lite os asm sources
LOS_ASMS_SOURCES += $(LITEOS_BASE)/arch/arm/cortex-m4/gcc/los_dispatch.S
LOS_ASMS_SOURCES += $(LITEOS_BASE)/arch/arm/cortex-m4/gcc/los_exc.S

#project sources,you can add your own source files here
PROJECT_FILES = ./src/main.c
PROJECT_FILES += ./src/gen_os_impl.c
PROJECT_FILES += ./src/profile.c
PROJECT_FILES += ./src/task_sample.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_adc.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_gpio.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_i2c.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_pinctrl.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_pwm.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_rtc.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_ssp.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_sysctrl.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_timer.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_uart.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_comparator.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_dma.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_efuse.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_i2s.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_ir.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_keyscan.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_pdm.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_pte.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_qdec.c
PROJECT_FILES += $(ING_REL)/src/FWlib/peripheral_usb.c
PROJECT_FILES += $(ING_REL)/src/Tools/trace.c
PROJECT_FILES += $(ING_REL)/src/BSP/eflash.c

#lite os sources
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
OS_FILES += $(LITEOS_BASE)/arch/arm/cortex-m4/gcc/los_context.c
OS_FILES += $(LITEOS_BASE)/arch/arm/cortex-m4/gcc/los_interrupt.c
OS_FILES += $(LITEOS_BASE)/arch/arm/cortex-m4/gcc/los_mpu.c
OS_FILES += $(LITEOS_BASE)/arch/arm/cortex-m4/gcc/los_timer.c
OS_FILES += $(LITEOS_BASE)/arch/arm/common/los_common_interrupt.c
OS_FILES += $(LITEOS_BASE)/utils/los_debug.c
OS_FILES += $(LITEOS_BASE)/utils/los_error.c
OS_FILES += $(LITEOS_BASE)/utils/los_hook.c
OS_FILES += $(LITEOS_BASE)/lib/libsec/src/memcpy_s.c
OS_FILES += $(LITEOS_BASE)/lib/libsec/src/memset_s.c
OS_FILES += $(LITEOS_BASE)/lib/libsec/src/strncpy_s.c
OS_FILES += $(LITEOS_BASE)/components/signal/los_signal.c

#sdk startup file
STARTUP = src/gstartup_ing91600.s
#its sdk headers,can not change
ING_SRC_INC=-I $(ING_REL)/src/BSP -I $(ING_REL)/src/FWlib -I $(ING_REL)/src/StartUP/ing916 -I $(ING_REL)/src/Tools -I $(ING_REL)/src
ING_BUNDLE_INC=-I $(ING_BUNDLE) -I $(ING_BUNDLE)/os -I $(CMSIS_INC)
#its sdk symbols
SYMDEFS=-Wl,$(ING_SYMDEF)

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

C_SOURCES =  $(PROJECT_FILES) $(OS_FILES)

# ASM sources
ASM_SOURCES = $(STARTUP)

# ASM sources
ASMM_SOURCES = $(LOS_ASMS_SOURCES)

AS_DEFS =

STARTUP_DEFS=-D__STARTUP_CLEAR_BSS
C_DEFS = $(APPDEFS) $(STARTUP_DEFS)

AS_INCLUDES =

C_INCLUDES = $(ING_SRC_INC) $(OS_INC) $(ING_BUNDLE_INC) $(USER_INC)

MCU = -mcpu=cortex-m4

ARCH_FLAGS=-mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

ASFLAGS += $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -flto

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) $(ARCH_FLAGS) -Wall -fdata-sections -ffunction-sections -flto
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
ASFLAGS += -g -gdwarf-2
endif
CFLAGS += -MMD -MP -fno-builtin -MF"$(@:%.o=%.d)"

# Link for code size
GC=-Wl,--gc-sections

# Create map file
MAP=-Wl,-Map=$(MAP_FILE)

LDSCRIPT = $(LD_SCRIPT)

LIBS = -lc -lm -lnosys
LIBDIR =

# Use newlib-nano. To disable it, specify USE_NANO=
USE_NANO=--specs=nano.specs

# Use seimhosting or not
USE_SEMIHOST=--specs=rdimon.specs
USE_NOHOST=--specs=nosys.specs

LDFLAGS = $(MCU) $(GC) $(USE_NOHOST) $(USE_NANO) $(SYMDEFS) -T$(LDSCRIPT) $(LIBDIR) $(LIBS) $(MAP),--cref
#$(CFLAGS)

all: $(TARGET).elf $(TARGET).axf $(TARGET).hex $(TARGET).bin

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASMM_SOURCES:.S=.o)))
vpath %.S $(sort $(dir $(ASMM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(ASFLAGS) $< -o $@
$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(AS) -c $(ASFLAGS) $< -o $@

$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(TARGET).axf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

%.hex: %.elf
	$(HEX) $< $@

%.bin: %.elf
	$(BIN) $< $@

ifeq ($(OS),Windows_NT)
$(BUILD_DIR):
	cmd /C mkdir $(BUILD_DIR)

clean:
	cmd /c del /Q /S $(TARGET).axf $(TARGET).elf $(TARGET).hex $(TARGET).bin $(BUILD_DIR) $(MAP_FILE)
else
$(BUILD_DIR):
	mkdir $(BUILD_DIR)


clean:
	rm -f $(TARGET).axf $(TARGET).elf $(TARGET).bin $(TARGET).hex  -fr $(BUILD_DIR) $(MAP_FILE)
endif

flash:
	JLink -CommanderScript script.jlink