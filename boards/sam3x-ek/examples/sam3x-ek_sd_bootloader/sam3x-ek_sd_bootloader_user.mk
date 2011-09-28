#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
sam3x-ek_sd_bootloader_PROGRAMMER_TYPE = none
sam3x-ek_sd_bootloader_PROGRAMMER_PORT = none

# Files included by the user.
sam3x-ek_sd_bootloader_USER_CSRC = \
	$(sam3x-ek_sd_bootloader_SRC_PATH)/main.c \
	bertos/cpu/cortex-m3/drv/dmac_sam3.c \
	bertos/cpu/cortex-m3/drv/hsmci_sam3.c \
	bertos/cpu/cortex-m3/io/sam3_chipid.c \
	#

# Files included by the user.
sam3x-ek_sd_bootloader_USER_PCSRC = \
	#

# Files included by the user.
sam3x-ek_sd_bootloader_USER_CPPASRC = \
	#

# Files included by the user.
sam3x-ek_sd_bootloader_USER_CXXSRC = \
	#

# Files included by the user.
sam3x-ek_sd_bootloader_USER_ASRC = \
	#

# Flags included by the user.
sam3x-ek_sd_bootloader_USER_LDFLAGS = \
	#

# Flags included by the user.
sam3x-ek_sd_bootloader_USER_CPPAFLAGS = \
	#

# Flags included by the user.
sam3x-ek_sd_bootloader_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	-Os \
	#
