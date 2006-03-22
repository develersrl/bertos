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
# Revision 1.1  2006/03/22 09:51:53  bernie
# Add build infrastructure.
#
#

#
# Programmer type
#
# AVR ISP dongle that blows up easily
DPROG = -V -c stk500 -P /dev/ttyS0

# STK200 parallel cable
#DPROG = -c stk200 -E noreset

# PonyProg serial programmer
#DPROG = -c dasa2

#OPTCFLAGS = -O3 # DANGER - DO NOT USE - DOESN'T WORK WITH 3.4.1 & 3.5 (HEAD)
OPTCFLAGS = -ffunction-sections -fdata-sections 

# For AVRStudio
#DEBUGCFLAGS = -gdwarf-2

# For GDB
DEBUGCFLAGS = -g

#
# define some variables based on the AVR base path in $(AVR)
#
CROSS   = 
CC      = $(CROSS)gcc
AS      = $(CROSS)$(CC) -x assembler-with-cpp
LD      = $(CROSS)$(CC)
OBJCOPY = $(CROSS)objcopy
INSTALL = cp -a
RM      = rm -f
RM_R    = rm -rf
RN      = mv
MKDIR_P = mkdir -p

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
SHELL   = /bin/sh
DOXYGEN = doxygen
UISP    = uisp
AVRDUDE = avrdude

# output format can be srec, ihex (avrobj is always created)
FORMAT = srec
#FORMAT = ihex

WARNFLAGS = \
	-W -Wformat -Wall -Wundef -Wpointer-arith -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wsign-compare -Wstrict-prototypes \
	-Wmissing-prototypes -Wmissing-noreturn

# default compiler flags
CFLAGS = $(INCDIR) $(OPTCFLAGS) $(DEBUGCFLAGS) $(WARNFLAGS) \
	-MMD -MP -Wa,-anhlmsd=$(@:.o=.lst) -std=gnu99

# default compiler assembly flags
CPPAFLAGS = $(DEBUGCFLAGS) -MMD

# default assembler flags
ASFLAGS	= $(DEBUGCFLAGS)

# default linker flags
#LDFLAGS	= -Wl,-Map=$(@:%.elf=%.map),--cref -Wl,--reduce-memory-overheads
LDFLAGS	= -Wl,-Map=$(@:%.elf=%.map),--cref -Wl,--gc-sections

# flags for avrdude
AVRDUDEFLAGS = $(DPROG)

# additional libs
LIB = -lm
