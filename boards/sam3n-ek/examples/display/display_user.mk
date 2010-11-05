#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
display_PROGRAMMER_TYPE = none
display_PROGRAMMER_PORT = none

# Files included by the user.
display_USER_CSRC = \
	$(display_SRC_PATH)/main.c \
	$(display_SRC_PATH)/bitmaps.c \
	bertos/icons/logo.c \
	bertos/fonts/gohu.c \
	bertos/cpu/arm/drv/spi_dma_at91.c \
	#

# Files included by the user.
display_USER_PCSRC = \
	#

# Files included by the user.
display_USER_CPPASRC = \
	#

# Files included by the user.
display_USER_CXXSRC = \
	#

# Files included by the user.
display_USER_ASRC = \
	#

# Flags included by the user.
display_USER_LDFLAGS = \
	#

# Flags included by the user.
display_USER_CPPAFLAGS = \
	#

# Flags included by the user.
display_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	-O2 \
	#
