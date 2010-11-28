#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
at91sam7s-ek_kernel_PROGRAMMER_TYPE = none
at91sam7s-ek_kernel_PROGRAMMER_PORT = none

# Files included by the user.
at91sam7s-ek_kernel_USER_CSRC = \
	$(at91sam7s-ek_kernel_SRC_PATH)/main.c \
	#

# Files included by the user.
at91sam7s-ek_kernel_USER_PCSRC = \
	#

# Files included by the user.
at91sam7s-ek_kernel_USER_CPPASRC = \
	#

# Files included by the user.
at91sam7s-ek_kernel_USER_CXXSRC = \
	#

# Files included by the user.
at91sam7s-ek_kernel_USER_ASRC = \
	#

# Flags included by the user.
at91sam7s-ek_kernel_USER_LDFLAGS = \
	#

# Flags included by the user.
at91sam7s-ek_kernel_USER_CPPAFLAGS = \
	#

# Flags included by the user.
at91sam7s-ek_kernel_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	#
