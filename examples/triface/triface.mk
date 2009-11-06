#
# $Id$
# Copyright 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile fragment for DevLib triface application.
#
# Author: Bernie Innocenti <bernie@codewiz.org>
#
#

# Set to 1 for debug builds
triface_DEBUG = 1

# Our target application
TRG += triface
CPU = atmega1281
BOOT_ADDR_START = 0x1E000

triface_PREFIX = avr-

ifeq ($(CPU), atmega1281)
triface_hfuse = 0x98
triface_lfuse = 0x3d
triface_efuse = 0x7f
triface_lock = 0x2f
else
triface_hfuse = 0x88
triface_lfuse = 0xff
triface_efuse = 0xff
triface_lock = 0x2f
endif

triface_MCU = $(CPU)

triface_CSRC = \
	examples/triface/triface.c \
	examples/triface/protocol.c \
	examples/triface/hw/hw_adc.c \
	bertos/cpu/avr/drv/ser_avr.c \
	bertos/cpu/avr/drv/timer_avr.c \
	bertos/drv/timer.c \
	bertos/drv/ser.c \
	bertos/drv/buzzer.c \
	bertos/drv/sipo.c \
	bertos/mware/formatwr.c \
	bertos/mware/hex.c \
	bertos/mware/readline.c \
	bertos/mware/parser.c \
	bertos/mware/event.c \
	bertos/struct/hashtable.c \
	bertos/kern/kfile.c \
	bertos/net/keytag.c \
	#

triface_PCSRC += bertos/mware/formatwr.c

triface_CFLAGS = -O2 -D'ARCH=(ARCH_TRIFACE)' -D'CPU_FREQ=(14745600UL)'  -D'WIZ_AUTOGEN' \
				-fno-strict-aliasing -fwrapv -Iexamples/triface -Ibertos/cpu/avr

triface_LDFLAGS = -Wl

triface_PROGRAMMER_CPU = atmega1281
triface_PROGRAMMER_TYPE = stk500v2
triface_PROGRAMMER_PORT = /dev/ttyS1
triface_FLASH_SCRIPT = bertos/prg_scripts/avr/flash.sh
triface_STOPFLASH_SCRIPT = bertos/prg_scripts/none.sh
triface_DEBUG_SCRIPT = bertos/prg_scripts/nodebug.sh
triface_STOPDEBUG_SCRIPT = bertos/prg_scripts/none.sh

# Set to 1 for debug builds
boot_DEBUG = 0

# Our target application
TRG += boot

boot_MCU = $(CPU)
boot_CSRC = \
	examples/triface/boot/main.c \
	bertos/cpu/avr/drv/ser_avr.c \
	bertos/cpu/avr/drv/flash_avr.c \
	bertos/drv/timer.c \
	bertos/cpu/avr/drv/timer_avr.c \
	bertos/drv/ser.c \
	bertos/net/xmodem.c \
	bertos/algo/crc.c \
	bertos/mware/hex.c \
	bertos/kern/kfile.c \
	#

boot_PREFIX = avr-

boot_CPPFLAGS = -D'ARCH=(ARCH_TRIFACE|ARCH_BOOT)' -D'CPU_FREQ=(14745600UL)' -D'WIZ_AUTOGEN' \
				-fno-strict-aliasing -fwrapv -Iexamples/triface/boot -Ibertos/cpu/avr

boot_CFLAGS = -Os -mcall-prologues
boot_LDFLAGS = -Wl,--relax -Wl,--section-start=.text=$(BOOT_ADDR_START)


boot_PROGRAMMER_CPU = $(triface_PROGRAMMER_CPU)
boot_PROGRAMMER_TYPE = $(triface_PROGRAMMER_TYPE)
boot_PROGRAMMER_PORT = $(triface_PROGRAMMER_PORT)
boot_FLASH_SCRIPT = $(triface_FLASH_SCRIPT)
boot_STOPFLASH_SCRIPT = $(triface_STOPFLASH_SCRIPT)
boot_DEBUG_SCRIPT = $(triface_DEBUG_SCRIPT)
boot_STOPDEBUG_SCRIPT = $(triface_STOPDEBUG_SCRIPT)
