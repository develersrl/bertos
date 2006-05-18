#
# $Id$
# Copyright 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile fragment for DevLib demo application.
#
# Author: Bernardo Innocenti <bernie@develer.com>
#
# $Log$
# Revision 1.1  2006/05/18 00:41:47  bernie
# New triface devlib application.
#
#


# Set to 1 for debug builds
demo_DEBUG = 1

# Our target application
TRG += triface

triface_CSRC = \
	app/triface/triface.c \
	drv/timer.c \
	drv/ser.c \
	drv/ser_avr.c \
	mware/formatwr.c \
	mware/hex.c \

#	drv/buzzer.c

triface_CFLAGS = -Os -D'ARCH=0' -Iapp/triface/hw -Iapp/triface
triface_MCU = atmega128

# Debug stuff
ifeq ($(triface_DEBUG),1)
	triface_CFLAGS += -D_DEBUG
#	triface_PCSRC += drv/kdebug.c
endif

