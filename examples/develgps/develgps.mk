#
# Copyright 2010 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Author: Andrea Righi <arighi@develer.com>
#

# Set to 1 for debug builds
develgps_DEBUG = 0

include bertos/fonts/fonts.mk

# Our target application
TRG += develgps

develgps_CSRC = \
	examples/develgps/develgps.c \
	examples/develgps/logo.c \
	examples/develgps/compass.c \
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
	bertos/kern/kfile.c \
	bertos/kern/monitor.c \
	bertos/kern/proc.c \
	bertos/kern/signal.c \
	bertos/net/nmea.c \
	bertos/net/nmeap/src/nmeap01.c \
	bertos/cpu/cortex-m3/drv/flash_lm3s.c \
	bertos/cpu/cortex-m3/drv/gpio_lm3s.c \
	bertos/cpu/cortex-m3/drv/clock_lm3s.c \
	bertos/cpu/cortex-m3/drv/ser_lm3s.c \
	bertos/cpu/cortex-m3/drv/kdebug_lm3s.c \
	bertos/cpu/cortex-m3/drv/ssi_lm3s.c \
	bertos/cpu/cortex-m3/drv/timer_cm3.c \
	bertos/cpu/cortex-m3/drv/irq_cm3.c \
	bertos/cpu/cortex-m3/hw/switch_ctx_cm3.c \
	bertos/cpu/cortex-m3/hw/init_lm3s.c

develgps_CPPASRC = \
	bertos/cpu/cortex-m3/hw/vectors_cm3.S \
	bertos/cpu/cortex-m3/hw/crt_cm3.S \
	#

# This is an hosted application
develgps_PREFIX = arm-none-eabi-

develgps_CPPAFLAGS = -mthumb -mno-thumb-interwork
develgps_CPPFLAGS = -D'ARCH=0' -D__ARM_LM3S1968__ -D'CPU_FREQ=(50000000L)' -D'WIZ_AUTOGEN' -mthumb -mno-thumb-interwork -Iexamples/develgps -Ibertos/cpu/cortex-m3 -fno-strict-aliasing -fwrapv
develgps_LDFLAGS = -nostartfiles -T bertos/cpu/cortex-m3/scripts/lm3s1968_rom.ld -Wl,--no-warn-mismatch -mthumb -mno-thumb-interwork

develgps_CPU = cortex-m3

develgps_PROGRAMMER_CPU = lm3s1968
develgps_PROGRAMMER_TYPE = lm3s1968
develgps_FLASH_SCRIPT = bertos/prg_scripts/arm/flash-cortex.sh
develgps_STOPFLASH_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh
develgps_DEBUG_SCRIPT = bertos/prg_scripts/arm/debug-cortex.sh
develgps_STOPDEBUG_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh

ifeq ($(develgps_DEBUG),0)
	# Production options
	develgps_CFLAGS += -O2 -fomit-frame-pointer
	develgps_CXXFLAGS += -O2 -fomit-frame-pointer
else
	# Debug options
	develgps_CPPAFLAGS += -g -gdwarf-2
	develgps_CPPFLAGS += -O0 -g3 -gdwarf-2 -fverbose-asm
endif
