#
# $Id$
# Copyright 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile fragment for DevLib triface application.
#
# Author: Bernardo Innocenti <bernie@develer.com>
#
# $Log$
# Revision 1.2  2006/06/01 12:29:21  marco
# Add first simple protocol command (version request).
#
# Revision 1.1  2006/05/18 00:41:47  bernie
# New triface devlib application.
#
#


# Set to 1 for debug builds
triface_DEBUG = 1

# Our target application
TRG += triface

triface_CSRC = \
	app/triface/triface.c \
	app/triface/protocol.c \
	drv/timer.c \
	drv/ser.c \
	drv/ser_avr.c \
	mware/formatwr.c \
	mware/hex.c \
	mware/hashtable.c \
	mware/readline.c \
	mware/parser.c \

#	drv/buzzer.c

triface_PCSRC += mware/formatwr.c

triface_CFLAGS = -Os -D'ARCH=0' -Iapp/triface/hw -Iapp/triface
triface_MCU = atmega128

# Debug stuff
ifeq ($(triface_DEBUG),1)
	triface_CFLAGS += -D_DEBUG
	triface_PCSRC += drv/kdebug.c
endif

