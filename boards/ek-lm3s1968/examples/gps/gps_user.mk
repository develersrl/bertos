#
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile template for BeRTOS wizard.
#
# Author: Lorenzo Berni <duplo@develer.com>
#
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
gps_PROGRAMMER_TYPE = none
gps_PROGRAMMER_PORT = none

# Files included by the user.
gps_USER_CSRC = \
	bertos/cpu/cortex-m3/drv/ssi_lm3s.c \
	bertos/fonts/gohu.c \
	bertos/fonts/luBS14.c \
	bertos/gui/menu.c \
	$(gps_SRC_PATH)/compass.c \
	$(gps_SRC_PATH)/main.c \
	#

# Files included by the user.
gps_USER_PCSRC = \
	#

# Files included by the user.
gps_USER_CPPASRC = \
	#

# Files included by the user.
gps_USER_CXXSRC = \
	#

# Files included by the user.
gps_USER_ASRC = \
	#

# Flags included by the user.
gps_USER_LDFLAGS = \
	#

# Flags included by the user.
gps_USER_CPPAFLAGS = \
	#

# Flags included by the user.
gps_USER_CPPFLAGS = \
	-O2 -fomit-frame-pointer \
	-fno-strict-aliasing \
	-fwrapv \
	#
