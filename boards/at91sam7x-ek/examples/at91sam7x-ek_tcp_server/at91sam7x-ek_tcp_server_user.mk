#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
at91sam7x-ek_tcp_server_PROGRAMMER_TYPE = none
at91sam7x-ek_tcp_server_PROGRAMMER_PORT = none

# Files included by the user.
at91sam7x-ek_tcp_server_USER_CSRC = \
	$(at91sam7x-ek_tcp_server_SRC_PATH)/main.c \
	#

# Files included by the user.
at91sam7x-ek_tcp_server_USER_PCSRC = \
	#

# Files included by the user.
at91sam7x-ek_tcp_server_USER_CPPASRC = \
	#

# Files included by the user.
at91sam7x-ek_tcp_server_USER_CXXSRC = \
	#

# Files included by the user.
at91sam7x-ek_tcp_server_USER_ASRC = \
	#

# Flags included by the user.
at91sam7x-ek_tcp_server_USER_LDFLAGS = \
	#

# Flags included by the user.
at91sam7x-ek_tcp_server_USER_CPPAFLAGS = \
	#

# Flags included by the user.
at91sam7x-ek_tcp_server_USER_CPPFLAGS = \
	-Os \
	-fno-strict-aliasing \
	-fwrapv \
	-Wno-cast-qual \
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
