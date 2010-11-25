#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
armtest_PROGRAMMER_TYPE = none
armtest_PROGRAMMER_PORT = none

include $(armtest_SRC_PATH)/arm_src.mk

# Files included by the user.
armtest_USER_CSRC = \
	$(armtest_SRC_PATH)/empty_main.c \
 	$(arm_CSRC) \
	#

# Files included by the user.
armtest_USER_PCSRC = \
	#

# Files included by the user.
armtest_USER_CPPASRC = \
	#

# Files included by the user.
armtest_USER_CXXSRC = \
	#

# Files included by the user.
armtest_USER_ASRC = \
	#

# Flags included by the user.
armtest_USER_LDFLAGS = \
	#

# Flags included by the user.
armtest_USER_CPPAFLAGS = \
	#

# Flags included by the user.
armtest_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	#
