#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
triface_PROGRAMMER_TYPE = none
triface_PROGRAMMER_PORT = none

# Files included by the user.
triface_USER_CSRC = \
    $(triface_SRC_PATH)/main.c \
    $(triface_SRC_PATH)/protocol.c \
    #

# Files included by the user.
triface_USER_PCSRC = \
	#

# Files included by the user.
triface_USER_CPPASRC = \
	#

# Files included by the user.
triface_USER_CXXSRC = \
	#

# Files included by the user.
triface_USER_ASRC = \
	#

# Flags included by the user.
triface_USER_LDFLAGS = \
	#

# Flags included by the user.
triface_USER_CPPAFLAGS = \
	#

# Flags included by the user.
triface_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	#
