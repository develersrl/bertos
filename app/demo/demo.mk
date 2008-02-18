#
# $Id: demo.mk 18234 2007-10-08 13:39:48Z rasky $
# Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile fragment for DevLib demo application.
#
# Author: Bernardo Innocenti <bernie@develer.com>
#

include fonts/fonts.mk
include emul/emul.mk

# Set to 1 for debug builds
demo_DEBUG = 1

# Our target application
TRG += demo

# FIXME: we want to use g++ for C source too
CC = g++

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
	kern/proc_test.c \
	verstag.c

demo_CPPASRC = \
	kern/switch.S

# FIXME: maybe this junk should go in emul/emul.mk?
$(OBJDIR)/demo/emul/emulwin.o: emul/emulwin_moc.cpp 
$(OBJDIR)/demo/drv/lcd_gfx_qt.o: drv/lcd_gfx_qt_moc.cpp
$(OBJDIR)/demo/drv/timer.o: drv/timer_qt_moc.cpp
$(OBJDIR)/demo/emul/emulkbd.o: emul/emulkbd_moc.cpp

#FIXME: isn't there a way to avoid repeating the pattern rule?
drv/timer_qt_moc.cpp: drv/timer_qt.c
	$(QT_MOC) -o $@ $<


demo_CFLAGS = -D_QT=4 -D'ARCH=ARCH_EMUL' -Iapp/demo -Ihw $(EMUL_CFLAGS)
demo_CXXFLAGS = -D_QT=4 -D'ARCH=ARCH_EMUL' -Iapp/demo -Ihw $(EMUL_CFLAGS)
demo_LDFLAGS = $(EMUL_LDFLAGS)

# Debug stuff
ifeq ($(demo_DEBUG),1)
	demo_CFLAGS += -D_DEBUG
	demo_CXXFLAGS += -D_DEBUG
	demo_CSRC += drv/kdebug.c
else
	demo_CFLAGS += -Os
	demo_CXXFLAGS += -Os
endif

