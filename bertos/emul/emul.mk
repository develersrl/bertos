#
# Copyright 2007 Bernie Innocenti (http://www.codewiz.org/)
# All rights reserved.
#
# Author: Bernie Innocenti <bernie@codewiz.org>
#
# Qt configuration for devlib emulator demo.  Include it from
# your app Makefile fragment

EMUL_CFLAGS = $(shell pkg-config QtCore QtGui --cflags) -DQT_CLEAN_NAMESPACE -DQT3_SUPPORT
EMUL_LDFLAGS = $(shell pkg-config QtCore QtGui --libs)
QT_MOC = $(shell pkg-config QtCore --variable=moc_location)
#QT_UIC = $(shell pkg-config QtCore --variable=uic_location)
