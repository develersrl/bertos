#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
sam3n-ek_kernel_PROGRAMMER_TYPE = none
sam3n-ek_kernel_PROGRAMMER_PORT = none

# Files included by the user.
sam3n-ek_kernel_USER_CSRC = \
	$(sam3n-ek_kernel_SRC_PATH)/main.c \
	bertos/cpu/cortex-m3/drv/spi_dma_sam3.c \
	#

# Files included by the user.
sam3n-ek_kernel_USER_PCSRC = \
	#

# Files included by the user.
sam3n-ek_kernel_USER_CPPASRC = \
	#

# Files included by the user.
sam3n-ek_kernel_USER_CXXSRC = \
	#

# Files included by the user.
sam3n-ek_kernel_USER_ASRC = \
	#

# Flags included by the user.
sam3n-ek_kernel_USER_LDFLAGS = \
	#

# Flags included by the user.
sam3n-ek_kernel_USER_CPPAFLAGS = \
	#

# Flags included by the user.
sam3n-ek_kernel_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	#
