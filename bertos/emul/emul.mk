#
# Copyright 2007 Bernie Innocenti (http://www.codewiz.org/)
# All rights reserved.
#
# Author: Bernie Innocenti <bernie@codewiz.org>
#
# Qt configuration for BeRTOS emulator.  Include it from
# your app Makefile fragment

EMUL_CFLAGS = -D'ARCH=ARCH_EMUL|ARCH_QT' \
	$(shell pkg-config QtCore QtGui --cflags) -DQT_CLEAN_NAMESPACE
EMUL_LDFLAGS = $(shell pkg-config QtCore QtGui --libs)
QT_MOC = $(shell pkg-config QtCore --variable=moc_location)
#QT_UIC = $(shell pkg-config QtCore --variable=uic_location)
