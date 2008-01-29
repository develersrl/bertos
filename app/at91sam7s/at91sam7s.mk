#
# $Id: at91sam7s.mk 18234 2007-10-08 13:39:48Z rasky $
# Copyright 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile fragment for DevLib at91sam7s application.
#
# Author: Bernardo Innocenti <bernie@develer.com>
#
#

# Set to 1 for debug builds
at91sam7s_DEBUG = 1

# Our target application
TRG += at91sam7s

at91sam7s_CSRC = \
	app/at91sam7s/at91sam7s.c \
	drv/timer.c \
	drv/ser.c \
	cpu/arm/drv/sysirq_at91.c \
	cpu/arm/drv/ser_at91.c \
	mware/event.c \
	mware/formatwr.c \
	mware/hex.c \
	kern/kfile.c \
	kern/proc.c \
	kern/proc_test.c \
	kern/monitor.c \
	kern/signal.c

at91sam7s_CPPASRC = \
	cpu/arm/hw/crtat91sam7_rom.S \
	kern/switch_arm.S

at91sam7s_CROSS = arm-elf-

at91sam7s_CPPAFLAGS = -O0 -g -gdwarf-2 -g -gen-debug
at91sam7s_CPPFLAGS = -O0 -D'ARCH=0' -D__ARM_AT91SAM7S256__ -g3 -gdwarf-2 -fverbose-asm -Iapp/at91sam7s/hw -Iapp/at91sam7s -Icpu/arm
at91sam7s_LDFLAGS = -nostartfiles -T cpu/arm/scripts/at91sam7_256_ram.ld -Wl,--no-warn-mismatch

at91sam7s_CPU = arm7tdmi

# Debug stuff
ifeq ($(at91sam7s_DEBUG),1)
	at91sam7s_CFLAGS += -D_DEBUG
	at91sam7s_CSRC += drv/kdebug.c
endif

