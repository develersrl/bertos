#
# $Id: boot.mk 15184 2007-02-16 15:09:23Z batt $
# Copyright 2002, 2003, 2004 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile fragment for boot application.
#
# Author: Bernardo Innocenti <bernie@develer.com>
#


# Set to 1 for debug builds
boot_DEBUG = 1

# Our target application
TRG += boot

boot_MCU = atmega64
boot_CSRC = \
	boot/main.c \
	mware/xmodem.c \
	drv/ser.c \
	cpu/avr/drv/ser_avr.c \
	cpu/avr/drv/flash_avr.c \
	drv/timer.c \
	algos/crc.c

boot_CROSS = avr-
boot_CPPFLAGS = -D'ARCH=(ARCH_TRIFACE|ARCH_BOOT)' -Iboot -Icpu/avr
boot_CFLAGS = -Os -mcall-prologues
boot_LDSCRIPT = boot/boot.ld
boot_LDFLAGS = -Wl,--relax

# Debug stuff
ifeq ($(boot_DEBUG),1)
	boot_CFLAGS += -D_DEBUG
	boot_PCSRC += drv/kdebug.c mware/formatwr.c
endif

