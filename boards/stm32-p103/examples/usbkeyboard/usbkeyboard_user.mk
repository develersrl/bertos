#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
usbkeyboard_PROGRAMMER_TYPE = none
usbkeyboard_PROGRAMMER_PORT = none

# Files included by the user.
usbkeyboard_USER_CSRC = \
	$(usbkeyboard_SRC_PATH)/main.c \
	#

# Files included by the user.
usbkeyboard_USER_PCSRC = \
	#

# Files included by the user.
usbkeyboard_USER_CPPASRC = \
	#

# Files included by the user.
usbkeyboard_USER_CXXSRC = \
	#

# Files included by the user.
usbkeyboard_USER_ASRC = \
	#

# Flags included by the user.
usbkeyboard_USER_LDFLAGS = \
	#

# Flags included by the user.
usbkeyboard_USER_CPPAFLAGS = \
	#

# Flags included by the user.
usbkeyboard_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	#
