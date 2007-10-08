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
at91sam7s_DEBUG = 0

# Our target application
TRG += at91sam7s

at91sam7s_CSRC = \
	app/at91sam7s/at91sam7s.c \
	drv/timer.c \
	drv/at91/sysirq.c

at91sam7s_PCSRC += mware/formatwr.c

at91sam7s_CFLAGS = -O3 -D'ARCH=0' -Iapp/at91sam7s/hw -Iapp/at91sam7s
at91sam7s_CPU = arm7tdmi

# Debug stuff
ifeq ($(at91sam7s_DEBUG),1)
	at91sam7s_CFLAGS += -D_DEBUG
	at91sam7s_PCSRC += drv/kdebug.c
endif

