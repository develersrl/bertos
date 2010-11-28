#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
arduino_aprs_PROGRAMMER_TYPE = none
arduino_aprs_PROGRAMMER_PORT = none

# Files included by the user.
arduino_aprs_USER_CSRC = \
	$(arduino_aprs_SRC_PATH)/main.c \
	$(arduino_aprs_HW_PATH)/hw/hw_afsk.c \
	#

# Files included by the user.
arduino_aprs_USER_PCSRC = \
	#

# Files included by the user.
arduino_aprs_USER_CPPASRC = \
	#

# Files included by the user.
arduino_aprs_USER_CXXSRC = \
	#

# Files included by the user.
arduino_aprs_USER_ASRC = \
	#

# Flags included by the user.
arduino_aprs_USER_LDFLAGS = \
	#

# Flags included by the user.
arduino_aprs_USER_CPPAFLAGS = \
	#

# Flags included by the user.
arduino_aprs_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	#
