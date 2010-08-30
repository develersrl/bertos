#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
kernel_PROGRAMMER_TYPE = none
kernel_PROGRAMMER_PORT = none

# Files included by the user.
kernel_USER_CSRC = \
	bertos/cpu/cortex-m3/drv/ssi_lm3s.c \
	$(kernel_SRC_PATH)/main.c \
	#

# Files included by the user.
kernel_USER_PCSRC = \
	#

# Files included by the user.
kernel_USER_CPPASRC = \
	#

# Files included by the user.
kernel_USER_CXXSRC = \
	#

# Files included by the user.
kernel_USER_ASRC = \
	#

# Flags included by the user.
kernel_USER_LDFLAGS = \
	#

# Flags included by the user.
kernel_USER_CPPAFLAGS = \
	#

# Flags included by the user.
kernel_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	#
