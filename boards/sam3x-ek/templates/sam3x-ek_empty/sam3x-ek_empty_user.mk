#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
sam3x-ek_empty_PROGRAMMER_TYPE = none
sam3x-ek_empty_PROGRAMMER_PORT = none

# Files included by the user.
sam3x-ek_empty_USER_CSRC = \
	$(sam3x-ek_empty_SRC_PATH)/main.c \
	bertos/cpu/cortex-m3/drv/dmac_sam3.c \
	bertos/cpu/cortex-m3/drv/hsmci_sam3.c \
	#

# Files included by the user.
sam3x-ek_empty_USER_PCSRC = \
	#

# Files included by the user.
sam3x-ek_empty_USER_CPPASRC = \
	#

# Files included by the user.
sam3x-ek_empty_USER_CXXSRC = \
	#

# Files included by the user.
sam3x-ek_empty_USER_ASRC = \
	#

# Flags included by the user.
sam3x-ek_empty_USER_LDFLAGS = \
	#

# Flags included by the user.
sam3x-ek_empty_USER_CPPAFLAGS = \
	#

# Flags included by the user.
sam3x-ek_empty_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	#
