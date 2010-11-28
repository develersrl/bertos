#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
arduino-mega_context_switch_PROGRAMMER_TYPE = none
arduino-mega_context_switch_PROGRAMMER_PORT = none

# Files included by the user.
arduino-mega_context_switch_USER_CSRC = \
	$(arduino-mega_context_switch_SRC_PATH)/main.c \
	#

# Files included by the user.
arduino-mega_context_switch_USER_PCSRC = \
	#

# Files included by the user.
arduino-mega_context_switch_USER_CPPASRC = \
	#

# Files included by the user.
arduino-mega_context_switch_USER_CXXSRC = \
	#

# Files included by the user.
arduino-mega_context_switch_USER_ASRC = \
	#

# Flags included by the user.
arduino-mega_context_switch_USER_LDFLAGS = \
	#

# Flags included by the user.
arduino-mega_context_switch_USER_CPPAFLAGS = \
	#

# Flags included by the user.
arduino-mega_context_switch_USER_CPPFLAGS = \
	-O2 \
	-fno-strict-aliasing \
	-fwrapv \
	#
