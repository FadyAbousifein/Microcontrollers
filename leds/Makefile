# Makefile for building Arduino sketches on Arch Linux with arduino-cli
# Arduino core installed at: ~/.arduino15/packages/arduino/hardware/avr/1.8.6

MKVERSION = 1.0

OSNAME = $(shell uname)

# CONFIGURE THESE FOR YOUR PROJECT:
PROJECT := blinky
VERSION ?= 1.0
ARDUINO_MODEL ?= uno
PORT ?= /dev/ttyACM0

# Arduino paths for arduino-cli installation
ARDUINO_VERSION = 1.8.6
ARDUINO_BASE = $(HOME)/.arduino15/packages/arduino/hardware/avr/$(ARDUINO_VERSION)
ARDUINO_DIR ?= $(ARDUINO_BASE)
ARDUINO_CORE ?= $(ARDUINO_DIR)/cores/arduino

# Architecture
ARCH ?= avr

# Arduino IDE version (for compatibility defines)
ARDUINO ?= 10806

# Programmer
AVRDUDE_PROGRAMMER ?= arduino

# Arduino variant - will be auto-detected based on board
# Override if needed
#ARDUINO_VARIANT = $(ARDUINO_DIR)/variants/standard

# Libraries to include (uncomment as needed)
ifndef ARDUINO_LIBS
ARDUINO_LIBS =
# ARDUINO_LIBS += EEPROM
# ARDUINO_LIBS += Wire
# ARDUINO_LIBS += SPI
# ARDUINO_LIBS += Servo
endif

# User libraries directory
USER_LIBDIR ?= ./libraries
USER_LIBS ?=

# Additional libraries to link
LDLIBS ?= -lm

# Output directory
OUTPUT ?= bin

# AVR tools path (leave empty for system PATH)
AVR_TOOLS_PATH ?=

# Reset command
RESETCMD ?= stty

# Macro definitions
CDEFS ?=

############################################################################
# Below here nothing should need to be changed
############################################################################

HEXFORMAT = ihex
DEPFILE = $(OUTPUT)/Makefile.depend
TARFILE = $(PROJECT)-$(VERSION).tar

# Get board variables from boards.txt
ifdef ARDUINO_FAMILY
MODEL_PATTERN_MATCHING = $(ARDUINO_MODEL)\|$(ARDUINO_FAMILY)
else
MODEL_PATTERN_MATCHING = $(ARDUINO_MODEL)
endif

getboardvar = $(shell \
	sed "/^\($(MODEL_PATTERN_MATCHING)\)\.$(1)=/ { s/.*=//; q }; d" \
		$(ARDUINO_DIR)/boards.txt \
	)

UPLOAD_RATE ?= $(call getboardvar,upload.speed)
MCU ?= $(call getboardvar,build.mcu)
F_CPU ?= $(call getboardvar,build.f_cpu)
AVRDUDE_PROGRAMMER ?= $(call getboardvar,upload.protocol)
VID ?= $(call getboardvar,build.vid)
PID ?= $(call getboardvar,build.pid)
BOARD ?= $(call getboardvar,build.board)

# Auto-detect Arduino variant if not set
ifeq ($(ARDUINO_VARIANT),)
ifeq ("$(ARDUINO_MODEL)", $(filter "$(ARDUINO_MODEL)", "mega" "mega2560"))
ARDUINO_VARIANT ?= $(ARDUINO_DIR)/variants/mega
else
ifeq "$(ARDUINO_MODEL)" "micro"
ARDUINO_VARIANT ?= $(ARDUINO_DIR)/variants/micro
else
ifeq "$(ARDUINO_MODEL)" "leonardo"
ARDUINO_VARIANT ?= $(ARDUINO_DIR)/variants/leonardo
else
ARDUINO_VARIANT ?= $(ARDUINO_DIR)/variants/standard
endif
endif
endif
endif

### Sources

# Arduino core sources
CORESRC = $(wildcard $(ARDUINO_CORE)/*.c)
CORECXXSRC = $(wildcard $(ARDUINO_CORE)/*.cpp)
COREASMSRC = $(wildcard $(ARDUINO_CORE)/*.S)

# Arduino library sources
ALIBDIRS = $(wildcard \
		$(ARDUINO_LIBS:%=$(ARDUINO_DIR)/libraries/%) \
		$(ARDUINO_LIBS:%=$(ARDUINO_DIR)/libraries/%/src) \
		$(ARDUINO_LIBS:%=$(ARDUINO_DIR)/libraries/%/src/$(ARCH)) \
		$(ARDUINO_LIBS:%=$(ARDUINO_DIR)/libraries/%/utility) \
		)
ALIBSRC = $(wildcard $(ALIBDIRS:%=%/*.c))
ALIBCXXSRC = $(wildcard $(ALIBDIRS:%=%/*.cpp))
ALIBASMSRC = $(wildcard $(ALIBDIRS:%=%/*.S))

# User library sources
ULIBDIRS = $(wildcard \
		$(USER_LIBS:%=$(USER_LIBDIR)/%) \
		$(USER_LIBS:%=$(USER_LIBDIR)/%/utility) \
		)
ULIBSRC = $(wildcard $(ULIBDIRS:%=%/*.c))
ULIBCXXSRC = $(wildcard $(ULIBDIRS:%=%/*.cpp))
ULIBASMSRC = $(wildcard $(ULIBDIRS:%=%/*.S))

# User program sources
SRC = $(wildcard *.c)
CXXSRC =
CXXSRCINO = $(wildcard *.ino) $(wildcard *.pde)
prjino := $(findstring $(PROJECT).ino,$(CXXSRCINO))
prjpde := $(findstring $(PROJECT).pde,$(CXXSRCINO))

CXXSRCINO := $(filter-out $(PROJECT).ino $(PROJECT).pde,$(CXXSRCINO))
ifneq "" "$(prjino)$(prjpde)"
CXXSRC += $(OUTPUT)$(if $(OUTPUT),/)$(PROJECT).cpp
endif
CXXSRC += $(wildcard *.cpp)
ASRC = $(wildcard *.S)

# vpath setup
ifneq "$(ARDUINO_CORE)" ""
  vpath % $(ARDUINO_CORE)
endif
ifneq "$(ALIBDIRS)" ""
  vpath % $(ALIBDIRS)
endif
ifneq "$(ULIBDIRS)" ""
  vpath % $(ULIBDIRS)
endif
vpath % .

### Include directories
CINCS = \
	-I$(ARDUINO_CORE) \
	-I$(ARDUINO_VARIANT) \
	$(ALIBDIRS:%=-I%) \
	$(ULIBDIRS:%=-I%) \
	-I.

### Object files
COREOBJ = $(addprefix $(OUTPUT)/,$(notdir \
			$(CORESRC:.c=.c.o) \
			$(CORECXXSRC:.cpp=.cpp.o) \
			$(COREASMSRC:.S=.S.o) \
		))

ALIBOBJ = $(addprefix $(OUTPUT)/,$(notdir \
			$(ALIBSRC:.c=.c.o) \
			$(ALIBCXXSRC:.cpp=.cpp.o) \
			$(ALIBASMSRC:.S=.S.o) \
		))

ULIBOBJ = $(addprefix $(OUTPUT)/,$(notdir \
			$(ULIBSRC:.c=.c.o) \
			$(ULIBCXXSRC:.cpp=.cpp.o) \
			$(ULIBASMSRC:.S=.S.o) \
		))

OBJ = $(addprefix $(OUTPUT)/,$(notdir \
			$(SRC:.c=.c.o) \
			$(CXXSRC:.cpp=.cpp.o) \
			$(ASRC:.S=.S.o) \
		))

ALLOBJ = $(OBJ) $(ULIBOBJ) $(ALIBOBJ) $(COREOBJ)
ALLDEPS = $(ALLOBJ:%.o=%.d)

### Macro definitions
CDEFS += -DF_CPU=$(F_CPU) -DARDUINO=$(ARDUINO)
CDEFS += -DARDUINO_$(BOARD)
CDEFS += -DARDUINO_ARCH_$(shell echo $(ARCH) | tr '[a-z]' '[A-Z]')

### Compiler flags
CSTANDARD = -std=gnu99
CXXSTANDARD = -std=gnu++11

OPT_OPTIMS = -Os
OPT_OPTIMS += -ffunction-sections -fdata-sections
OPT_OPTIMS += -mrelax

OPT_DEBUG = -g

ifndef OPT_WARN
OPT_WARN = -Wall
OPT_WARN += -Wextra
endif

ifndef OPT_OTHER
OPT_OTHER =
OPT_OTHER += -DUSB_VID=$(VID) -DUSB_PID=$(PID)
OPT_OTHER += -fno-use-cxa-atexit
endif

CFLAGS = -mmcu=$(MCU) \
		$(OPT_OPTIMS) $(OPT_DEBUG) $(CSTANDARD) $(CDEFS) \
		$(OPT_WARN) $(OPT_OTHER)

CXXFLAGS = -mmcu=$(MCU) \
		$(OPT_OPTIMS) $(OPT_DEBUG) $(CXXSTANDARD) $(CDEFS) \
		$(OPT_WARN) $(OPT_OTHER)

### Assembler flags
ASTANDARD = -x assembler-with-cpp
ASFLAGS = -mmcu=$(MCU) $(CDEFS) $(ASTANDARD)

### Linker flags
LDFLAGS = -mmcu=$(MCU)
LDFLAGS += $(OPT_OPTIMS)
LDFLAGS += -Wl,--gc-sections

### Programming flags
AVRDUDE_FLAGS =
AVRDUDE_FLAGS += -D
AVRDUDE_FLAGS += -q
AVRDUDE_FLAGS += -p $(MCU) -c $(AVRDUDE_PROGRAMMER) -b $(UPLOAD_RATE)
AVRDUDE_FLAGS += -P $(PORT)

AVRDUDE_WRITE_FLASH = -U flash:w:$(OUTPUT)/$(PROJECT).hex:i

### Programs
AVRPREFIX = avr-
CC = $(AVR_TOOLS_PATH)$(AVRPREFIX)gcc
CXX = $(AVR_TOOLS_PATH)$(AVRPREFIX)g++
OBJCOPY = $(AVR_TOOLS_PATH)$(AVRPREFIX)objcopy
OBJDUMP = $(AVR_TOOLS_PATH)$(AVRPREFIX)objdump
AR = $(AVR_TOOLS_PATH)$(AVRPREFIX)ar
SIZE = $(AVR_TOOLS_PATH)$(AVRPREFIX)size
NM = $(AVR_TOOLS_PATH)$(AVRPREFIX)nm
AVRDUDE = $(AVR_TOOLS_PATH)avrdude
RM = rm -f
RMDIR = rmdir
MV = mv -f
ifeq "$(OSNAME)" "Linux"
    STTY = stty -F $(PORT)
else
    STTY = stty -f $(PORT)
endif

### Implicit rules
.SUFFIXES: .ino .pde .elf .hex .eep .lss .listing .sym .symbol
.SUFFIXES: .cpp .c .S .o .a

%.cpp.o $(OUTPUT)/%.cpp.o: %.cpp
	$(CXX) -o $@ -c $(CXXFLAGS) $< \
	  -MMD -MP -MF"$(@:%.cpp.o=%.d)" -MT"$@ $(@:%.cpp.o=%.d)" \
	  $(CINCS)

%.c.o $(OUTPUT)/%.c.o: %.c
	$(CC) -o $@ -c $(CFLAGS) $< \
	  -MMD -MP -MF"$(@:%.c.o=%.d)" -MT"$@ $(@:%.c.o=%.d)" \
	  $(CINCS)

%.S.o $(OUTPUT)/%.S.o: %.S
	$(CC) -o $@ -c $(ASFLAGS) $< \
	  -MMD -MP -MF"$(@:%.S.o=%.d)" -MT"$@ $(@:%.S.o=%.d)" \
	  $(CINCS)

%.hex: %.elf
	$(OBJCOPY) -O $(HEXFORMAT) -R .eeprom $< $@

%.eep: %.elf
	-$(OBJCOPY) -j .eeprom \
	--set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 \
	-O $(HEXFORMAT) $< $@

%.lss %.listing: %.elf
	$(OBJDUMP) -h -S $< > $@

%.sym %.symbol: %.elf
	$(NM) -n $< > $@

$(OUTPUT)/%.cpp: %.ino
	@echo >  $@ "// Automatically generated by Makefile. Don't edit."
	@echo >> $@ "#include <Arduino.h>"
	@cat  >> $@ $< $(CXXSRCINO)

$(OUTPUT)/%.cpp: %.pde
	@echo > $@ "// Automatically generated by Makefile. Don't edit."
	@echo >> $@ "#if ARDUINO >= 100"
	@echo >> $@ "#include <Arduino.h>"
	@echo >> $@ "#else"
	@echo >> $@ "#include <WProgram.h>"
	@echo >> $@ "#endif"
	@cat  >> $@ $< $(CXXSRCINO)

### Explicit rules
.PHONY: all build elf hex eep lss lst sym listing symbol size
.PHONY: reset reset_stty upload up clean depend mkout showvars help

all: elf hex listing symbol size

build: elf hex

elf: $(OUTPUT) $(OUTPUT)/$(PROJECT).elf
hex: $(OUTPUT) $(OUTPUT)/$(PROJECT).hex
eep: $(OUTPUT) $(OUTPUT)/$(PROJECT).eep
lss: $(OUTPUT) $(OUTPUT)/$(PROJECT).lss
lst: $(OUTPUT) $(OUTPUT)/$(PROJECT).lss
sym: $(OUTPUT) $(OUTPUT)/$(PROJECT).sym
listing: $(OUTPUT) $(OUTPUT)/$(PROJECT).listing
symbol: $(OUTPUT) $(OUTPUT)/$(PROJECT).symbol

help:
	@echo "Arduino Makefile for Arch Linux (arduino-cli)"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Compile and create hex file (default)"
	@echo "  upload (up)  - Upload to Arduino board"
	@echo "  clean        - Remove all generated files"
	@echo "  size         - Show program size"
	@echo "  reset        - Reset Arduino board"
	@echo "  showvars     - Show makefile variables"
	@echo ""
	@echo "Current configuration:"
	@echo "  PROJECT:      $(PROJECT)"
	@echo "  BOARD:        $(ARDUINO_MODEL)"
	@echo "  PORT:         $(PORT)"
	@echo "  MCU:          $(MCU)"
	@echo "  F_CPU:        $(F_CPU)"

showvars:
	@echo "PROJECT = $(PROJECT)"
	@echo "ARDUINO_MODEL = $(ARDUINO_MODEL)"
	@echo "PORT = $(PORT)"
	@echo "MCU = $(MCU)"
	@echo "F_CPU = $(F_CPU)"
	@echo "UPLOAD_RATE = $(UPLOAD_RATE)"
	@echo "AVRDUDE_PROGRAMMER = $(AVRDUDE_PROGRAMMER)"
	@echo "ARDUINO_DIR = $(ARDUINO_DIR)"
	@echo "ARDUINO_CORE = $(ARDUINO_CORE)"
	@echo "ARDUINO_VARIANT = $(ARDUINO_VARIANT)"
	@echo "ARDUINO_LIBS = $(ARDUINO_LIBS)"

mkout $(OUTPUT):
	mkdir -p $(OUTPUT)

$(OUTPUT)/libcore.a: $(COREOBJ)
	$(AR) rcsv $@ $(COREOBJ)

$(OUTPUT)/$(PROJECT).elf: $(OBJ) $(ULIBOBJ) $(ALIBOBJ) $(OUTPUT)/libcore.a
	$(CC) $(LDFLAGS) -o $@ -Wl,-u,main $(OBJ) $(ULIBOBJ) $(ALIBOBJ) -L$(OUTPUT) -lcore $(LDLIBS)

size:
	@echo
	@$(SIZE) $(OUTPUT)/$(PROJECT).elf
	@echo
	@$(SIZE) --target=$(HEXFORMAT) $(OUTPUT)/$(PROJECT).hex

reset: reset_$(RESETCMD)

reset_stty:
	$(STTY) -hupcl; sleep 0.1
	$(STTY) hupcl; sleep 0.1
	$(STTY) -hupcl

upload up: hex reset
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)

clean:
	-$(RM) $(OUTPUT)/$(PROJECT).cpp
	-$(RM) $(OUTPUT)/$(PROJECT).elf
	-$(RM) $(OUTPUT)/$(PROJECT).hex
	-$(RM) $(OUTPUT)/$(PROJECT).eep
	-$(RM) $(OUTPUT)/$(PROJECT).map
	-$(RM) $(OUTPUT)/$(PROJECT).lss
	-$(RM) $(OUTPUT)/$(PROJECT).sym
	-$(RM) $(OUTPUT)/libcore.a
	-$(RM) $(ALLOBJ)
	-$(RM) $(ALLDEPS)
	-test ! -d $(OUTPUT) || $(RMDIR) $(OUTPUT)

-include $(ALLDEPS)
