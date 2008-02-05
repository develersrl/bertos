#
# $Id: demo.mk 18234 2007-10-08 13:39:48Z rasky $
# Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile fragment for DevLib demo application.
#
# Author: Bernardo Innocenti <bernie@develer.com>
#

# Set to 1 for debug builds
battfs_test_DEBUG = 1

# Our target application
TRG += battfs_test


battfs_test_CSRC = \
	fs/battfs.c \
	app/battfs/battfs_test.c

battfs_test_CFLAGS = -O0 -D'ARCH=ARCH_EMUL' -D_EMUL -Iapp/battfs -Ihw -g3


# Debug stuff
ifeq ($(battfs_test_DEBUG),1)
	battfs_test_CFLAGS += -D_DEBUG
endif

