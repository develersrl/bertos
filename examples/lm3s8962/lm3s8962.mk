# Copyright 2010 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Author: Andrea Righi <arighi@develer.com>
#

# Set to 1 for debug builds
lm3s8962_DEBUG = 0

include bertos/fonts/fonts.mk

# Our target application
TRG += lm3s8962

lm3s8962_CSRC = \
	examples/lm3s8962/lm3s8962.c \
	bertos/gfx/bitmap.c \
	bertos/gfx/line.c \
	bertos/gfx/win.c \
	bertos/gfx/text.c \
	bertos/gfx/text_format.c \
	bertos/gui/menu.c \
	bertos/fonts/gohu.c \
	bertos/fonts/luBS14.c \
	bertos/icons/logo.c \
	bertos/mware/formatwr.c \
	bertos/mware/hex.c \
	bertos/mware/sprintf.c \
	bertos/mware/event.c \
	bertos/struct/heap.c \
	bertos/drv/timer.c \
	bertos/drv/ser.c \
	bertos/drv/kbd.c \
	bertos/drv/lcd_rit128x96.c \
	bertos/io/kfile.c \
	bertos/kern/monitor.c \
	bertos/kern/proc_test.c \
	bertos/kern/proc.c \
	bertos/kern/signal.c \
	bertos/cpu/cortex-m3/drv/gpio_lm3s.c \
	bertos/cpu/cortex-m3/drv/clock_lm3s.c \
	bertos/cpu/cortex-m3/drv/ser_lm3s.c \
	bertos/cpu/cortex-m3/drv/kdebug_lm3s.c \
	bertos/cpu/cortex-m3/drv/ssi_lm3s.c \
	bertos/cpu/cortex-m3/drv/timer_cm3.c \
	bertos/cpu/cortex-m3/drv/irq_cm3.c \
	bertos/cpu/cortex-m3/hw/switch_ctx_cm3.c \
	bertos/cpu/cortex-m3/hw/init_cm3.c

lm3s8962_CPPASRC = \
	bertos/cpu/cortex-m3/hw/vectors_cm3.S \
	bertos/cpu/cortex-m3/hw/crt_cm3.S \
	#

# This is an hosted application
lm3s8962_PREFIX = arm-none-eabi-

lm3s8962_CPPAFLAGS = -mthumb -mno-thumb-interwork
lm3s8962_CPPFLAGS = -D'ARCH=0' -D__ARM_LM3S8962__ -D'CPU_FREQ=(50000000L)' -D'WIZ_AUTOGEN' -mthumb -mno-thumb-interwork -Iexamples/lm3s8962 -Ibertos/cpu/cortex-m3 -fno-strict-aliasing -fwrapv
lm3s8962_LDFLAGS = -nostartfiles -T bertos/cpu/cortex-m3/scripts/lm3s8962_rom.ld -Wl,--no-warn-mismatch -mthumb -mno-thumb-interwork

lm3s8962_CPU = cortex-m3

lm3s8962_PROGRAMMER_CPU = lm3s
lm3s8962_PROGRAMMER_TYPE = luminary
lm3s8962_FLASH_SCRIPT = bertos/prg_scripts/arm/flash-lm3s.sh
lm3s8962_STOPFLASH_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh
lm3s8962_DEBUG_SCRIPT = bertos/prg_scripts/arm/debug.sh
lm3s8962_STOPDEBUG_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh

ifeq ($(lm3s8962_DEBUG),0)
	# Production options
	lm3s8962_CFLAGS += -O2 -fomit-frame-pointer
	lm3s8962_CXXFLAGS += -O2 -fomit-frame-pointer
else
	# Debug options
	lm3s8962_CPPAFLAGS += -g -gdwarf-2
	lm3s8962_CPPFLAGS += -O0 -g3 -gdwarf-2 -fverbose-asm
endif
