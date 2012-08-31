#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
arduino_1wire_PROGRAMMER_TYPE = none
arduino_1wire_PROGRAMMER_PORT = none

# Files included by the user.
arduino_1wire_USER_CSRC = \
	$(arduino_1wire_SRC_PATH)/main.c \
	#

# Files included by the user.
arduino_1wire_USER_PCSRC = \
	#

# Files included by the user.
arduino_1wire_USER_CPPASRC = \
	#

# Files included by the user.
arduino_1wire_USER_CXXSRC = \
	#

# Files included by the user.
arduino_1wire_USER_ASRC = \
	#

# Flags included by the user.
arduino_1wire_USER_LDFLAGS = \
	#

# Flags included by the user.
arduino_1wire_USER_CPPAFLAGS = \
	#

# Flags included by the user.
arduino_1wire_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	#
