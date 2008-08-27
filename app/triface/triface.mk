#
# $Id$
# Copyright 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile fragment for DevLib triface application.
#
# Author: Bernie Innocenti <bernie@codewiz.org>
#
#

# Set to 1 for debug builds
triface_DEBUG = 1

# Our target application
TRG += triface

triface_hfuse = 0x88
triface_lfuse = 0xff
triface_efuse = 0xff
triface_lock = 0x2f
triface_CSRC = \
	app/triface/triface.c \
	app/triface/protocol.c \
	app/triface/hw/hw_adc.c \
	bertos/drv/timer.c \
	bertos/drv/ser.c \
	bertos/drv/buzzer.c \
	bertos/cpu/avr/drv/ser_avr.c \
	bertos/cpu/avr/drv/sipo.c \
	bertos/mware/formatwr.c \
	bertos/mware/hex.c \
	bertos/struct/hashtable.c \
	bertos/mware/readline.c \
	bertos/mware/parser.c \
	bertos/mware/event.c \
	bertos/kern/kfile.c \
	bertos/net/keytag.c \
	#

triface_PCSRC += bertos/mware/formatwr.c


triface_CFLAGS = -O2 -D'ARCH=(ARCH_TRIFACE)' -fno-strict-aliasing -Iapp/triface -Ibertos/cpu/avr
triface_LDFLAGS = -Wl

triface_MCU = atmega64
triface_CROSS = avr-

# Set to 1 for debug builds
boot_DEBUG = 0

# Our target application
TRG += boot

boot_MCU = atmega64
boot_CSRC = \
	app/triface/boot/main.c \
	bertos/net/xmodem.c \
	bertos/drv/ser.c \
	bertos/cpu/avr/drv/ser_avr.c \
	bertos/cpu/avr/drv/flash_avr.c \
	bertos/drv/timer.c \
	bertos/algo/crc.c \
	bertos/mware/hex.c \
	bertos/kern/kfile.c \
	#
boot_CROSS = avr-
boot_CPPFLAGS = -D'ARCH=(ARCH_TRIFACE|ARCH_BOOT)' -Iapp/triface/boot -Ibertos/cpu/avr
boot_CFLAGS = -Os -mcall-prologues
boot_LDSCRIPT = app/triface/boot/boot.ld
boot_LDFLAGS = -Wl,--relax
