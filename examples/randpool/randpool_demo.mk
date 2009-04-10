#
# $Id$
# Copyright 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile fragment for DevLib randpool_demo application.
#
# Author: Daniele Basile <asterix@develer.com>
#
# $Log$
# Revision 1.1  2007/02/15 18:17:51  asterix
# Add randpool test program.
#
#


# Set to 1 for debug builds
randpool_demo_DEBUG = 0

randpool_demo_HOSTED = 1

# Our target application
TRG += randpool_demo

randpool_demo_CSRC = \
	examples/randpool/randpool_demo.c \
	algos/randpool.c \
	algos/md2.c \
	drv/timer.c \
	os/hptime.c

randpool_demo_CFLAGS = -O0 -D'ARCH=0' -Iexamples/randpool/ -Iexamples/randpool/hw

# Debug stuff
ifeq ($(randpool_demo_DEBUG),1)
	randpool_demo_CFLAGS += -D_DEBUG
	randpool_demo_PCSRC += drv/kdebug.c
endif

