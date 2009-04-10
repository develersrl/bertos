#
# $Id: lm3s1968.mk 18234 2007-10-08 13:39:48Z rasky $
# Copyright 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile fragment for DevLib lm3s1968 application.
#
# Author: Manuele Fanelli <qwert@develer.com>
#
#

# Set to 1 for debug builds
lm3s1968_DEBUG = 0


# Our target application
TRG += lm3s1968

lm3s1968_CSRC = \
	examples/lm3s1968/lm3s1968.c \
	#bertos/cpu/cortex-m3/startup_lm3s.c

# This is an hosted application
lm3s1968_PREFIX = arm-none-eabi-

lm3s1968_CPPAFLAGS = -O0 -g -gdwarf-2 -g -gen-debug -mthumb
lm3s1968_CPPFLAGS = -O0 -D'ARCH=0' -D__ARM_LM3S1968__ -g3 -gdwarf-2 -fverbose-asm -mthumb  -Iexamples/lm3s1968 -Ibertos/cpu/cortex-m3
lm3s1968_LDFLAGS = -nostartfiles -T examples/lm3s1968/standalone.ld -Wl,--no-warn-mismatch

lm3s1968_CPU = cortex-m3

