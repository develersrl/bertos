#
# $Id: lm3s1968.mk 18234 2007-10-08 13:39:48Z rasky $
# Copyright 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile fragment for DevLib lm3s1968 application.
#
# Author: Manuele Fanelli <qwert@develer.com>
#
#

# Set to 1 for debug builds
lm3s1968_DEBUG = 0

# Our target application
TRG += lm3s1968

lm3s1968_CSRC = \
	examples/lm3s1968/lm3s1968.c \
	bertos/cpu/cortex-m3/drv/irq.c \
	bertos/cpu/cortex-m3/drv/timer.c \
	bertos/cpu/cortex-m3/startup_lm3s.c

# This is an hosted application
lm3s1968_PREFIX = arm-none-eabi-

lm3s1968_CPPAFLAGS = -O0 -g -gdwarf-2 -g -gen-debug -mthumb -fno-strict-aliasing -fwrapv
lm3s1968_CPPFLAGS = -O0 -D'ARCH=0' -D__ARM_LM3S1968__ -D'CPU_FREQ=(48023000L)' -g3 -gdwarf-2 -fverbose-asm -mthumb  -Iexamples/lm3s1968 -Ibertos/cpu/cortex-m3 -fno-strict-aliasing -fwrapv
lm3s1968_LDFLAGS = -nostartfiles -T bertos/cpu/cortex-m3/scripts/lm3s1968_rom.ld -Wl,--no-warn-mismatch -fno-strict-aliasing -fwrapv

lm3s1968_CPU = cortex-m3

lm3s1968_PROGRAMMER_CPU = lm3s1968
lm3s1968_PROGRAMMER_TYPE = lm3s1968
lm3s1968_FLASH_SCRIPT = bertos/prg_scripts/arm/flash-cortex.sh
lm3s1968_STOPFLASH_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh
lm3s1968_DEBUG_SCRIPT = bertos/prg_scripts/arm/debug.sh
lm3s1968_STOPDEBUG_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh

# Debug stuff
ifeq ($(demo_DEBUG),0)
	demo_CFLAGS += -Os -fomit-frame-pointer
	demo_CXXFLAGS += -Os -fomit-frame-pointer
endif
