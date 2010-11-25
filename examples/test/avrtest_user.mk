#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
avrtest_PROGRAMMER_TYPE = none
avrtest_PROGRAMMER_PORT = none

# For Nightlytest we include the all Bertos files.
include $(avrtest_SRC_PATH)/avr_src.mk

# Files included by the user.
avrtest_USER_CSRC = \
	$(avrtest_SRC_PATH)/empty_main.c \
 	$(avr_CSRC) \
	#

# Files included by the user.
avrtest_USER_PCSRC = \
	#

# Files included by the user.
avrtest_USER_CPPASRC = \
	#

# Files included by the user.
avrtest_USER_CXXSRC = \
	#

# Files included by the user.
avrtest_USER_ASRC = \
	#

# Flags included by the user.
avrtest_USER_LDFLAGS = \
	#

# Flags included by the user.
avrtest_USER_CPPAFLAGS = \
	#

# Flags included by the user.
avrtest_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	#
