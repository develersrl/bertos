#
# $Id: at91sam7x.mk 18234 2007-10-08 13:39:48Z rasky $
# Copyright 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile fragment for DevLib at91sam7x application.
#
# Author: Bernie Innocenti <bernie@codewiz.org>
#
#

# Set to 1 for debug builds
at91sam7x_DEBUG = 1

# Our target application
TRG += at91sam7x

at91sam7x_CSRC = \
	examples/at91sam7/at91sam7.c \
	bertos/drv/timer.c \
	bertos/drv/ser.c \
	bertos/cpu/arm/drv/sysirq_at91.c \
	bertos/cpu/arm/drv/ser_at91.c \
	bertos/cpu/arm/drv/timer_at91.c \
	bertos/mware/event.c \
	bertos/mware/formatwr.c \
	bertos/mware/hex.c \
	bertos/mware/sprintf.c \
	bertos/struct/heap.c \
	bertos/kern/kfile.c \
	bertos/kern/proc.c \
	bertos/kern/coop.c \
	bertos/kern/preempt.c \
	bertos/kern/proc_test.c \
	bertos/kern/signal_test.c \
	bertos/kern/monitor.c \
	bertos/kern/signal.c \
	#

at91sam7x_CPPASRC = \
	bertos/cpu/arm/hw/crtat91sam7_rom.S \
	bertos/cpu/arm/hw/switch_ctx_arm.S \
	#

at91sam7x_PREFIX = arm-none-eabi-

at91sam7x_CPPAFLAGS = -O0 -g -gdwarf-2 -g -gen-debug
at91sam7x_CPPFLAGS = -O0 -D'ARCH=0' -D__ARM_AT91SAM7X256__ -D'CPU_FREQ=(48023000UL)' -D'WIZ_AUTOGEN' -g3 -gdwarf-2 -fverbose-asm -Iexamples/at91sam7 -Ibertos/cpu/arm
at91sam7x_LDFLAGS = -nostartfiles -T bertos/cpu/arm/scripts/at91sam7_256_rom.ld -Wl,--no-warn-mismatch

at91sam7x_CPU = arm7tdmi

at91sam7x_PROGRAMMER_CPU = at91sam7
at91sam7x_PROGRAMMER_TYPE = jtag-tiny
at91sam7x_FLASH_SCRIPT = bertos/prg_scripts/arm/flash.sh
at91sam7x_STOPFLASH_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh
at91sam7x_DEBUG_SCRIPT = bertos/prg_scripts/arm/debug.sh
at91sam7x_STOPDEBUG_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh
