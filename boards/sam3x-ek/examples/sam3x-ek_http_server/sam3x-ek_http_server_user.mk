#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
sam3x-ek_http_server_PROGRAMMER_TYPE = jlink
sam3x-ek_http_server_PROGRAMMER_PORT = none

# Files included by the user.
sam3x-ek_http_server_USER_CSRC = \
	$(sam3x-ek_http_server_SRC_PATH)/main.c \
	$(sam3x-ek_http_server_SRC_PATH)/bitmaps.c \
	bertos/cpu/cortex-m3/drv/dmac_sam3.c \
	bertos/cpu/cortex-m3/drv/hsmci_sam3.c \
	bertos/cpu/cortex-m3/io/sam3_chipid.c \
	#

# Files included by the user.
sam3x-ek_http_server_USER_PCSRC = \
	#

# Files included by the user.
sam3x-ek_http_server_USER_CPPASRC = \
	#

# Files included by the user.
sam3x-ek_http_server_USER_CXXSRC = \
	#

# Files included by the user.
sam3x-ek_http_server_USER_ASRC = \
	#

# Flags included by the user.
sam3x-ek_http_server_USER_LDFLAGS = \
	#

# Flags included by the user.
sam3x-ek_http_server_USER_CPPAFLAGS = \
	#

# Flags included by the user.
# -Wno-cast-qual silences warnings from using lwip constants
sam3x-ek_http_server_USER_CPPFLAGS = \
	-Os \
	-fno-strict-aliasing \
	-Wno-cast-qual \
	-fwrapv \
	#

# Silence very annoying lwip warnings
lwip_CFLAGS = \
	-Wno-cast-align \
	-Wno-cast-qual \
	-Wno-sign-compare \
	-Wno-format \
	-Wno-unused-value \
	-Wno-conversion \
	-Wno-missing-noreturn \
	#
