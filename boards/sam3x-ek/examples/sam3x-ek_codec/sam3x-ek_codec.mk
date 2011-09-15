#
# Wizard autogenerated makefile.
# DO NOT EDIT, use the sam3x-ek_codec_user.mk file instead.
#

# Constants automatically defined by the selected modules
sam3x-ek_codec_DEBUG = 1

# Our target application
TRG += sam3x-ek_codec

sam3x-ek_codec_PREFIX = "arm-none-eabi-"

sam3x-ek_codec_SUFFIX = ""

sam3x-ek_codec_SRC_PATH = boards/sam3x-ek/examples/sam3x-ek_codec

sam3x-ek_codec_HW_PATH = boards/sam3x-ek

# Files automatically generated by the wizard. DO NOT EDIT, USE sam3x-ek_codec_USER_CSRC INSTEAD!
sam3x-ek_codec_WIZARD_CSRC = \
	bertos/cpu/cortex-m3/drv/adc_sam3.c \
	bertos/cpu/cortex-m3/drv/i2c_sam3.c \
	bertos/cpu/cortex-m3/drv/timer_cm3.c \
	bertos/cpu/cortex-m3/drv/dmac_sam3.c \
	bertos/drv/adc.c \
	bertos/drv/eeprom.c \
	bertos/drv/i2c.c \
	bertos/drv/i2c_bitbang.c \
	bertos/drv/lcd_hx8347.c \
	bertos/drv/sd.c \
	bertos/drv/timer.c \
	bertos/drv/wm8731.c \
	bertos/fs/fat.c \
	bertos/fs/fatfs/diskio.c \
	bertos/fs/fatfs/ff.c \
	bertos/io/kblock.c \
	bertos/io/kfile.c \
	bertos/io/kfile_block.c \
	bertos/mware/event.c \
	bertos/mware/formatwr.c \
	bertos/mware/hex.c \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE sam3x-ek_codec_USER_PCSRC INSTEAD!
sam3x-ek_codec_WIZARD_PCSRC = \
	 \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE sam3x-ek_codec_USER_CPPASRC INSTEAD!
sam3x-ek_codec_WIZARD_CPPASRC = \
	 \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE sam3x-ek_codec_USER_CXXSRC INSTEAD!
sam3x-ek_codec_WIZARD_CXXSRC = \
	 \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE sam3x-ek_codec_USER_ASRC INSTEAD!
sam3x-ek_codec_WIZARD_ASRC = \
	 \
	#

sam3x-ek_codec_CPPFLAGS = -D'CPU_FREQ=(73728000UL)' -D'ARCH=(ARCH_DEFAULT)' -D'WIZ_AUTOGEN' -I$(sam3x-ek_codec_HW_PATH) -I$(sam3x-ek_codec_SRC_PATH) $(sam3x-ek_codec_CPU_CPPFLAGS) $(sam3x-ek_codec_USER_CPPFLAGS)
#sam3x-ek_codec_CPPFLAGS = -D'CPU_FREQ=(84000000UL)' -D'ARCH=(ARCH_DEFAULT)' -D'WIZ_AUTOGEN' -I$(sam3x-ek_codec_HW_PATH) -I$(sam3x-ek_codec_SRC_PATH) $(sam3x-ek_codec_CPU_CPPFLAGS) $(sam3x-ek_codec_USER_CPPFLAGS)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
sam3x-ek_codec_LDFLAGS = $(sam3x-ek_codec_CPU_LDFLAGS) $(sam3x-ek_codec_WIZARD_LDFLAGS) $(sam3x-ek_codec_USER_LDFLAGS)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
sam3x-ek_codec_CPPAFLAGS = $(sam3x-ek_codec_CPU_CPPAFLAGS) $(sam3x-ek_codec_WIZARD_CPPAFLAGS) $(sam3x-ek_codec_USER_CPPAFLAGS)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
sam3x-ek_codec_CSRC = $(sam3x-ek_codec_CPU_CSRC) $(sam3x-ek_codec_WIZARD_CSRC) $(sam3x-ek_codec_USER_CSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
sam3x-ek_codec_PCSRC = $(sam3x-ek_codec_CPU_PCSRC) $(sam3x-ek_codec_WIZARD_PCSRC) $(sam3x-ek_codec_USER_PCSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
sam3x-ek_codec_CPPASRC = $(sam3x-ek_codec_CPU_CPPASRC) $(sam3x-ek_codec_WIZARD_CPPASRC) $(sam3x-ek_codec_USER_CPPASRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
sam3x-ek_codec_CXXSRC = $(sam3x-ek_codec_CPU_CXXSRC) $(sam3x-ek_codec_WIZARD_CXXSRC) $(sam3x-ek_codec_USER_CXXSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
sam3x-ek_codec_ASRC = $(sam3x-ek_codec_CPU_ASRC) $(sam3x-ek_codec_WIZARD_ASRC) $(sam3x-ek_codec_USER_ASRC)

# CPU specific flags and options, defined in the CPU definition files.
# Automatically generated by the wizard. PLEASE DO NOT EDIT!
sam3x-ek_codec_CPU_CPPASRC = bertos/cpu/cortex-m3/hw/crt_cm3.S bertos/cpu/cortex-m3/hw/vectors_cm3.S
sam3x-ek_codec_CPU_CPPAFLAGS = -g -gdwarf-2 -mthumb -mno-thumb-interwork
sam3x-ek_codec_CPU_CPPFLAGS = -O0 -g3 -gdwarf-2 -mthumb -mno-thumb-interwork -fno-strict-aliasing -fwrapv -fverbose-asm -Ibertos/cpu/cortex-m3/ -D__ARM_SAM3X8__
sam3x-ek_codec_CPU_CSRC = bertos/cpu/cortex-m3/hw/init_cm3.c bertos/cpu/cortex-m3/drv/irq_cm3.c bertos/cpu/cortex-m3/drv/clock_sam3.c
sam3x-ek_codec_PROGRAMMER_CPU = sam3
sam3x-ek_codec_CPU_LDFLAGS = -mthumb -mno-thumb-interwork -nostartfiles -Wl,--no-warn-mismatch -Wl,-dT bertos/cpu/cortex-m3/scripts/sam3x8_rom.ld
sam3x-ek_codec_STOPFLASH_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh
sam3x-ek_codec_CPU = cortex-m3
sam3x-ek_codec_STOPDEBUG_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh
sam3x-ek_codec_DEBUG_SCRIPT = bertos/prg_scripts/arm/debug.sh
sam3x-ek_codec_FLASH_SCRIPT = bertos/prg_scripts/arm/flash-sam3.sh

include $(sam3x-ek_codec_SRC_PATH)/sam3x-ek_codec_user.mk
