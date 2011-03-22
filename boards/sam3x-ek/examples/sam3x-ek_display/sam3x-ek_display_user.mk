#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
sam3x-ek_display_PROGRAMMER_TYPE = none
sam3x-ek_display_PROGRAMMER_PORT = none

# Files included by the user.
sam3x-ek_display_USER_CSRC = \
	$(sam3x-ek_display_SRC_PATH)/main.c \
	$(sam3x-ek_display_SRC_PATH)/bitmaps.c \
	bertos/icons/logo.c \
	bertos/fonts/gohu.c \
	#

# Files included by the user.
sam3x-ek_display_USER_PCSRC = \
	#

# Files included by the user.
sam3x-ek_display_USER_CPPASRC = \
	#

# Files included by the user.
sam3x-ek_display_USER_CXXSRC = \
	#

# Files included by the user.
sam3x-ek_display_USER_ASRC = \
	#

# Flags included by the user.
sam3x-ek_display_USER_LDFLAGS = \
	#

# Flags included by the user.
sam3x-ek_display_USER_CPPAFLAGS = \
	#

# Flags included by the user.
sam3x-ek_display_USER_CPPFLAGS = \
	-Os \
	-fno-strict-aliasing \
	-fwrapv \
	#
