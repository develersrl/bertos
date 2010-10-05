#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
usbserial_PROGRAMMER_TYPE = none
usbserial_PROGRAMMER_PORT = none

# Files included by the user.
usbserial_USER_CSRC = \
	$(usbserial_SRC_PATH)/main.c \
	#

# Files included by the user.
usbserial_USER_PCSRC = \
	#

# Files included by the user.
usbserial_USER_CPPASRC = \
	#

# Files included by the user.
usbserial_USER_CXXSRC = \
	#

# Files included by the user.
usbserial_USER_ASRC = \
	#

# Flags included by the user.
usbserial_USER_LDFLAGS = \
	#

# Flags included by the user.
usbserial_USER_CPPAFLAGS = \
	#

# Flags included by the user.
usbserial_USER_CPPFLAGS = \
	-Os \
	-fno-strict-aliasing \
	-fwrapv \
	#
