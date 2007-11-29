#
# $Id$
# Copyright 2002,2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Based on:
#   GCC-AVR standard Makefile part 1
#   Volker Oth 1/2000
#
# Author: Bernardo Innocenti <bernie@develer.com>
#
# $Log$
#

#
# Programmer type
# see local pgm_config.mk for programmer customization.
-include pgm_config.mk
DPROG ?= -V -c stk500 -P /dev/ttyS0

# AVR ISP dongle that blows up easily
#DPROG = -V -c stk500 -P /dev/ttyS0
#DPROG = -V -c jtag2slow
#-P /dev/ttyUSB0

# STK200 parallel cable
#DPROG = -c stk200 -E noreset

# JTAG ICE mkII
#DPROG = avarice --mkII -j usb -l

# PonyProg serial programmer
#DPROG = -c dasa2

# Set to 1 to build for embedded devices.
# e.g. produce target.elf instead of target and target_nostrip
EMBEDDED_TARGET = 1
#EMBEDDED_TARGET = 0

OPTCFLAGS = -ffunction-sections -fdata-sections
#OPTCFLAGS = -funsafe-loop-optimizations

# For AVRStudio
#DEBUGCFLAGS = -gdwarf-2

# For GDB
DEBUGCFLAGS = -ggdb

#
# define some variables based on the AVR base path in $(AVR)
#
CROSS   = avr-
CC      = $(CROSS)gcc
CXX     = $(CROSS)g++
AS      = $(CC) -x assembler-with-cpp
LD      = $(CC)
OBJCOPY = $(CROSS)objcopy
STRIP   = $(CROSS)strip
INSTALL = cp -a
RM      = rm -f
RM_R    = rm -rf
RN      = mv
MKDIR_P = mkdir -p
SHELL   = /bin/sh
CHECKER = sparse
DOXYGEN = doxygen
AVRDUDE = avrdude
FLEXCAT = $(top_srcdir)/tools/flexcat/flexcat

# For conversion from ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) \
	--debugging \
	--change-section-address .data-0x800000 \
	--change-section-address .bss-0x800000 \
	--change-section-address .noinit-0x800000 \
	--change-section-address .eeprom-0x810000

INCDIR  = -I. -Ihw
LIBDIR  = lib
OBJDIR  = obj
OUTDIR  = images

# output format can be srec, ihex (avrobj is always created)
FORMAT = srec
#FORMAT = ihex

# Compiler flags for generating dependencies
DEP_FLAGS = -MMD -MP

# Compiler flags for generating source listings
LIST_FLAGS = -Wa,-anhlmsd=$(@:.o=.lst)

# Linker flags for generating map files
ifeq ($(EMBEDDED_TARGET), 1)
MAP_FLAGS = -Wl,-Map=$(@:%.elf=%.map),--cref
else
MAP_FLAGS =
endif

# Compiler warning flags for both C and C++
WARNFLAGS = \
	-W -Wformat -Wall -Wundef -Wpointer-arith -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wsign-compare \
	-Wmissing-noreturn \
	-Wextra -Wstrict-aliasing=2 \
#	-Wunsafe-loop-optimizations

# Compiler warning flags for C only
C_WARNFLAGS = \
	-Wmissing-prototypes -Wstrict-prototypes

# Default C preprocessor flags (for C, C++ and cpp+as)
CPPFLAGS = $(INCDIR)

# Default C compiler flags
CFLAGS = $(OPTCFLAGS) $(DEBUGCFLAGS) $(WARNFLAGS) $(C_WARNFLAGS) \
	$(DEP_FLAGS) $(LIST_FLAGS) -std=gnu99

# Default C++ compiler flags
CXXFLAGS = $(OPTCFLAGS) $(DEBUGCFLAGS) $(WARNFLAGS) \
	$(DEP_FLAGS) $(LIST_FLAGS)

# Default compiler assembly flags
CPPAFLAGS = $(DEBUGCFLAGS) -MMD

# Default assembler flags
ASFLAGS	= $(DEBUGCFLAGS)

# Default linker flags
#LDFLAGS = $(MAP_FLAGS)

#bernie: does not complain for missing symbols!
LDFLAGS = $(MAP_FLAGS) -Wl,--gc-sections

# Flags for avrdude
AVRDUDEFLAGS = $(DPROG)

# additional libs
LIB = -lm
