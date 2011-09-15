#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
sam3x-ek_codec_PROGRAMMER_TYPE = none
sam3x-ek_codec_PROGRAMMER_PORT = none

# Files included by the user.
sam3x-ek_codec_USER_CSRC = \
	$(sam3x-ek_codec_SRC_PATH)/main.c \
	bertos/cpu/cortex-m3/drv/i2s_sam3.c \
	bertos/cpu/cortex-m3/drv/hsmci_sam3.c \
	bertos/cpu/cortex-m3/drv/sd_sam3.c \
	bertos/drv/sd_spi.c \
	#

# Files included by the user.
sam3x-ek_codec_USER_PCSRC = \
	#

# Files included by the user.
sam3x-ek_codec_USER_CPPASRC = \
	#

# Files included by the user.
sam3x-ek_codec_USER_CXXSRC = \
	#

# Files included by the user.
sam3x-ek_codec_USER_ASRC = \
	#

# Flags included by the user.
sam3x-ek_codec_USER_LDFLAGS = \
	#

# Flags included by the user.
sam3x-ek_codec_USER_CPPAFLAGS = \
	#

# Flags included by the user.
sam3x-ek_codec_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	#
