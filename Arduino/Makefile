#
#             LUFA Library
#     Copyright (C) Dean Camera, 2014.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.org
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

# Run "make help" for target help.

# Set the MCU accordingly to your device (e.g. at90usb1286 for a Teensy 2.0++, or atmega16u2 for an Arduino UNO R3)
MCU          = atmega16u2
ARCH         = AVR8
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
INCLUDE_DIR  = include
TARGET       = Joystick
SRC_DIR      = src
SRC          = $(SRC_DIR)/Joystick.c $(LUFA_SRC_USB)
LUFA_PATH    = lufa/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -Iinclude/
LD_FLAGS     = -lm

ARDUINO_PATH = /home/adrian/Schreibtisch/arduino-1.8.10-linux64/arduino-1.8.10/hardware/tools/avr/avr
INCLULDE_DIR += $(ARDUINO_PATH)/include/
CC_FLAGS += -I$(ARDUINO_PATH)/include/ -mmcu=$(MCU)

EMULATOR_TARGET		= emulator
EMULATOR_CC      	= gcc
EMULATOR_CC_FLAGS   = -Iinclude/ -I$(ARDUINO_PATH)/include/ -DMOCK_AVR
EMULATOR_OBJ_DIR 	= emulator_obj
EMULATOR_SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
EMULATOR_OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(EMULATOR_OBJ_DIR)/%.o,$(EMULATOR_SRC_FILES))

# Default target
all:
# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_dfu.mk
include $(LUFA_PATH)/Build/lufa_hid.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk

# Target for LED/buzzer to alert when print is done
with-alert: all
with-alert: CC_FLAGS += -DALERT_WHEN_DONE

flash: all
	sudo dfu-programmer $(MCU) erase
	sudo dfu-programmer $(MCU) flash $(TARGET).hex
	sudo dfu-programmer $(MCU) reset

clean-emu:
	rm -rf $(EMULATOR_OBJ_DIR) $(EMULATOR_TARGET)

run-emu:
	socat -d -d PTY,link=/tmp/faketty0,raw,echo=0 EXEC:./$(EMULATOR_TARGET)
