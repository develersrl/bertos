#
# $Id$
# Copyright 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile fragment for DevLib triface application.
#
# Author: Bernardo Innocenti <bernie@develer.com>
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
	drv/timer.c \
	drv/ser.c \
	cpu/avr/drv/ser_avr.c \
	cpu/avr/drv/sipo.c \
	mware/formatwr.c \
	mware/hex.c \
	mware/hashtable.c \
	mware/readline.c \
	mware/parser.c \
	mware/event.c \
	kern/kfile.c \
	net/keytag.c \
	drv/buzzer.c \
	#

triface_PCSRC += mware/formatwr.c


triface_CFLAGS = -O2 -D'ARCH=0' -fno-strict-aliasing -Iapp/triface -Iapp/triface/hw -Icpu/avr
triface_LDFLAGS = -Wl

triface_MCU = atmega64
triface_CROSS = avr-

# Debug stuff
ifeq ($(triface_DEBUG),1)
	triface_CFLAGS += -D_DEBUG
	triface_PCSRC += drv/kdebug.c
endif

