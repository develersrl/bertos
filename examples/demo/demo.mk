#
# Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
# Copyright 2008 Bernie Innocenti <bernie@codewiz.org>
#
# Makefile fragment for DevLib demo application.
#
# Version: $Id: demo.mk 18234 2007-10-08 13:39:48Z rasky $
# Author: Bernie Innocenti <bernie@codewiz.org>
#

include bertos/fonts/fonts.mk
include bertos/emul/emul.mk

# Set to 1 for debug builds
demo_DEBUG = 1

# This is an hosted application
demo_HOSTED = 1

# Our target application
TRG += demo

#We want to use g++ for C source too
demo_CC=g++

demo_CXXSRC = \
	bertos/emul/emul.cpp \
	bertos/emul/emulwin.cpp \
	bertos/emul/emulkbd.cpp \
	bertos/drv/lcd_gfx_qt.cpp

demo_CSRC = \
	examples/demo/demo.c \
	bertos/os/hptime.c \
	bertos/gfx/bitmap.c \
	bertos/gfx/line.c \
	bertos/gfx/win.c \
	bertos/gfx/text.c \
	bertos/gfx/text_format.c \
	bertos/gui/menu.c \
	bertos/fonts/helvB10.c \
	bertos/fonts/luBS14.c \
	bertos/fonts/ncenB18.c \
	bertos/icons/logo.c \
	bertos/drv/kbd.c \
	bertos/drv/timer.c \
	bertos/drv/timer_test.c \
	bertos/drv/buzzer.c \
	bertos/drv/ser.c \
	bertos/emul/ser_posix.c \
	bertos/emul/kfile_posix.c \
	bertos/mware/formatwr.c \
	bertos/mware/hex.c \
	bertos/mware/event.c \
	bertos/mware/observer.c \
	bertos/mware/resource.c \
	bertos/mware/sprintf.c \
	bertos/struct/heap.c \
	bertos/kern/irq.c \
	bertos/kern/proc.c \
	bertos/kern/proc_test.c \
	bertos/kern/sem.c \
	bertos/kern/signal.c \
	bertos/kern/monitor.c \
	bertos/verstag.c

demo_CPPASRC = \
	bertos/emul/switch_ctx_emul.S

# FIXME: maybe this junk should go in emul/emul.mk?
$(OBJDIR)/demo/bertos/emul/emulwin.o: bertos/emul/emulwin_moc.cpp
$(OBJDIR)/demo/bertos/drv/lcd_gfx_qt.o: bertos/drv/lcd_gfx_qt_moc.cpp
$(OBJDIR)/demo/bertos/drv/timer.o: bertos/emul/timer_qt_moc.cpp
$(OBJDIR)/demo/bertos/emul/emulkbd.o: bertos/emul/emulkbd_moc.cpp

#FIXME: isn't there a way to avoid repeating the pattern rule?
bertos/emul/timer_qt_moc.cpp: bertos/emul/timer_qt.c
	$(QT_MOC) -o $@ $<


demo_CPPAFLAGS =
demo_CFLAGS = -O0 -g3 -ggdb -Iexamples/demo $(EMUL_CFLAGS)
demo_CXXFLAGS = -O0 -g3 -ggdb -Iexamples/demo $(EMUL_CFLAGS)
demo_LDFLAGS = $(EMUL_LDFLAGS)

# Debug stuff
ifeq ($(demo_DEBUG),0)
	demo_CFLAGS += -Os
	demo_CXXFLAGS += -Os
endif
