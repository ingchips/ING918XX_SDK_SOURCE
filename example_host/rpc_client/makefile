PROJECT_FILES += ./lib/main.c
PROJECT_FILES += ./lib/brpc.c
PROJECT_FILES += ./lib/tools.cpp
PROJECT_FILES += ./app/$(app).c

ifndef app
$(error app is not set)
endif

PROJECT_FILES += ../../src/Tools/btstack_sync.c
PROJECT_FILES += ../../src/Tools/btstack_mt.c
PROJECT_FILES += ../../src/Tools/gatt_client_util.c

INC_PATHS += -I ./inc
INC_PATHS += -I ./lib
INC_PATHS += -I ../../bundles/exp/inc
INC_PATHS += -I ../../src/Tools
INC_PATHS += -I ../../src/StartUP/ing918
INC_PATHS += -I ../../examples/ble_rpc/src/

APPNAME = rpc.exe

OBJ_PATH=obj

TEMP1 = $(PROJECT_FILES:.c=.o)
TEMP2 = $(TEMP1:.cpp=.o)

APP_O_FILES = $(addprefix $(OBJ_PATH)/,$(notdir $(TEMP2)))

PREFIX	 = @
CC      := $(PREFIX)gcc
AS      := $(PREFIX)as
CXX     := $(PREFIX)g++
AR      := $(PREFIX)gcc-ar
NM      := $(PREFIX)gcc-nm
OBJCOPY := $(PREFIX)objcopy
OBJDUMP := $(PREFIX)objdump
SIZE    := $(PREFIX)size
RANLIB  := $(PREFIX)gcc-ranlib

CFLAGS += $(INC_PATHS) -fshort-enums -m32 -g
CXXFLAGS = $(CFLAGS)

ifeq ($(OS),Windows_NT)
LFLAGS += -lpthread -lwsock32
else
LFLAGS += -lpthread -lrt
endif

$(APPNAME): $(OBJ_PATH) $(APP_O_FILES)
	$(info LINK)
	$(CC) -o $@ $(APP_O_FILES) $(LFLAGS) $(CFLAGS)

$(OBJ_PATH)/%.o: lib/%.c
	$(info CC $<)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_PATH)/%.o: app/%.c
	$(info CC $<)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_PATH)/%.o: ../../src/Tools/%.c
	$(info CC $<)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_PATH)/%.o: lib/%.cpp
	$(info CXX $<)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_PATH)/%.o: app/%.cpp
	$(info CXX $<)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

ifeq ($(OS),Windows_NT)
$(OBJ_PATH):
	cmd /C mkdir $(OBJ_PATH)

clean:
	cmd /c del /Q /S $(APPNAME) $(OBJ_PATH)
else
$(OBJ_PATH):
	mkdir $(OBJ_PATH)

clean:
	rm -f $(APPNAME) $(APP_O_FILES)
endif

rebuild: clean $(APPNAME)