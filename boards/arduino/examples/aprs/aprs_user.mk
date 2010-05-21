#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
aprs_PROGRAMMER_TYPE = none
aprs_PROGRAMMER_PORT = none

# Files included by the user.
aprs_USER_CSRC = \
	$(aprs_SRC_PATH)/main.c \
	$(aprs_SRC_PATH)/hw/hw_afsk.c \
	#

# Files included by the user.
aprs_USER_PCSRC = \
	#

# Files included by the user.
aprs_USER_CPPASRC = \
	#

# Files included by the user.
aprs_USER_CXXSRC = \
	#

# Files included by the user.
aprs_USER_ASRC = \
	#

# Flags included by the user.
aprs_USER_LDFLAGS = \
	#

# Flags included by the user.
aprs_USER_CPPAFLAGS = \
	#

# Flags included by the user.
aprs_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	#
