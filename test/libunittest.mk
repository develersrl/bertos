#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
#
# Makefile fragment for unit testing.
#
# Author: Francesco Sacchi <batt@develer.com>
#

# Set to 1 for debug builds
libunittest_DEBUG = 1

# Our target application
#TRG += libunittest

libunittest_CSRC = \
	bertos/algo/ramp.c \
	bertos/drv/timer.c \
	bertos/fs/battfs.c \
	bertos/kern/coop.c \
	bertos/kern/idle.c \
	bertos/kern/kfile.c \
	bertos/kern/monitor.c \
	bertos/kern/proc.c \
	bertos/kern/signal.c \
	bertos/kern/sem.c \
	bertos/mware/event.c \
	bertos/mware/formatwr.c \
	bertos/mware/hex.c \
	bertos/mware/sprintf.c \
	bertos/os/hptime.c \
	#

libunittest_CPPASRC = \
	bertos/emul/switch.S \
	#

libunittest_CFLAGS = -O0 -g3 -ggdb -D"ARCH=(ARCH_EMUL | ARCH_UNITTEST)"
