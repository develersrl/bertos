# Copyright 2010 Develer S.r.l. (http://www.develer.com/)
#
# Makefile for the stm32p103 board example.
#
# Author: Andrea Righi <arighi@develer.com>

# Set to 1 for debug builds
stm32p103_DEBUG = 0

include bertos/fonts/fonts.mk

# Our target application
TRG += stm32p103

stm32p103_CSRC = \
	bertos/kern/proc.c \
	bertos/drv/timer.c \
	bertos/cpu/cortex-m3/drv/gpio_stm32.c \
	bertos/cpu/cortex-m3/drv/clock_stm32.c \
	bertos/cpu/cortex-m3/drv/timer_cm3.c \
	bertos/cpu/cortex-m3/drv/irq_cm3.c \
	bertos/cpu/cortex-m3/hw/switch_ctx_cm3.c \
	bertos/cpu/cortex-m3/hw/init_cm3.c \
	examples/stm32p103/main.c

stm32p103_CPPASRC = \
	bertos/cpu/cortex-m3/hw/vectors_cm3.S \
	bertos/cpu/cortex-m3/hw/crt_cm3.S \
	#

# This is an hosted application
stm32p103_PREFIX = arm-none-eabi-

stm32p103_CPPAFLAGS = -mthumb
stm32p103_CPPFLAGS = -D'ARCH=0' -D__ARM_STM32P103__ -D'CPU_FREQ=(72000000L)' -D'WIZ_AUTOGEN' -mthumb -Iexamples/stm32p103 -Ibertos/cpu/cortex-m3 -fno-strict-aliasing -fwrapv
stm32p103_LDFLAGS = -nostartfiles -T bertos/cpu/cortex-m3/scripts/stm32p103_rom.ld -Wl,--no-warn-mismatch -mthumb

stm32p103_CPU = cortex-m3

stm32p103_PROGRAMMER_CPU = stm32
stm32p103_PROGRAMMER_TYPE = jtag-tiny
stm32p103_FLASH_SCRIPT = bertos/prg_scripts/arm/flash-stm32.sh
stm32p103_STOPFLASH_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh
stm32p103_DEBUG_SCRIPT = bertos/prg_scripts/arm/debug.sh
stm32p103_STOPDEBUG_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh

ifeq ($(stm32p103_DEBUG),0)
	# Production options
	stm32p103_CFLAGS += -O0
	stm32p103_CXXFLAGS += -O0
else
	# Debug options
	stm32p103_CPPAFLAGS += -g -gdwarf-2
	stm32p103_CPPFLAGS += -O0 -g3 -gdwarf-2 -fverbose-asm
endif
