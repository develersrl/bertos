#
# $Id$
# Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile fragment for DevLib demo application.
#
# Author: Bernardo Innocenti <bernie@develer.com>
#
# $Log$
# Revision 1.5  2006/05/27 22:42:02  bernie
# Add verstag.
#
# Revision 1.4  2006/05/27 17:16:38  bernie
# Make demos a bit more interesting.
#
# Revision 1.3  2006/05/15 07:21:24  bernie
# Use pkg-config.
#
# Revision 1.2  2006/03/27 04:49:50  bernie
# Add bouncing logo demo.
#
# Revision 1.1  2006/03/22 09:52:13  bernie
# Add demo application.
#
#


# Set to 1 for debug builds
demo_DEBUG = 1

# Our target application
TRG += demo

CC = g++
CXX = g++

demo_CXXSRC = \
	emul/emul.cpp \
	emul/emulwin.cpp \
	emul/emulkbd.cpp \
	drv/lcd_gfx_qt.cpp

demo_CSRC = \
	app/demo/demo.c \
	os/hptime.c \
	gfx/bitmap.c \
	gfx/line.c \
	gfx/win.c \
	gfx/text.c \
	gfx/text_format.c \
	gui/menu.c \
	fonts/helvB10.c \
	fonts/luBS14.c \
	fonts/ncenB18.c \
	icons/artwork.c \
	icons/logo.c \
	drv/kbd.c \
	drv/timer.c \
	drv/buzzer.c \
	drv/ser.c \
	drv/ser_posix.c \
	mware/formatwr.c \
	mware/hex.c \
	mware/event.c \
	mware/observer.c \
	mware/resource.c \
	mware/sprintf.c \
	kern/proc.c \
	kern/sem.c \
	kern/signal.c \
	kern/monitor.c \
	verstag.c

demo_ASRC = \
	kern/switch_x86_64.s


$(OBJDIR)/demo/emul/emulwin.o: emul/emulwin_moc.cpp 
$(OBJDIR)/demo/drv/lcd_gfx_qt.o: drv/lcd_gfx_qt_moc.cpp

EMUL_CFLAGS = $(shell pkg-config QtGui --cflags) -DQT_CLEAN_NAMESPACE -DQT3_SUPPORT
EMUL_LDFLAGS = $(shell pkg-config QtGui --libs)
demo_CFLAGS = -Os -D_QT=4 -D'ARCH=ARCH_EMUL' -Iapp/demo -Ihw $(EMUL_CFLAGS)
demo_CXXFLAGS = -Os -D_QT=4 -D'ARCH=ARCH_EMUL' -Iapp/demo -Ihw $(EMUL_CFLAGS)
demo_LDFLAGS = $(EMUL_LDFLAGS)

# Debug stuff
ifeq ($(demo_DEBUG),1)
	demo_CFLAGS += -D_DEBUG
#	demo_PCSRC += drv/kdebug.c
endif

