#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
kernel_footprint_preempt_PROGRAMMER_TYPE = none
kernel_footprint_preempt_PROGRAMMER_PORT = none

# Files included by the user.
kernel_footprint_preempt_USER_CSRC = \
	$(kernel_footprint_preempt_SRC_PATH)/main.c \
	#

# Files included by the user.
kernel_footprint_preempt_USER_PCSRC = \
	#

# Files included by the user.
kernel_footprint_preempt_USER_CPPASRC = \
	#

# Files included by the user.
kernel_footprint_preempt_USER_CXXSRC = \
	#

# Files included by the user.
kernel_footprint_preempt_USER_ASRC = \
	#

# Flags included by the user.
kernel_footprint_preempt_USER_LDFLAGS = \
	#

# Flags included by the user.
kernel_footprint_preempt_USER_CPPAFLAGS = \
	#

# Flags included by the user.
kernel_footprint_preempt_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	#-Os \
	#
