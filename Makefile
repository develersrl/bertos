#
# $Id$
# Copyright 2002, 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Author: Bernardo Innocenti <bernie@develer.com>
#
# $Log$
# Revision 1.3  2006/09/20 14:30:14  marco
# Use demo as default app.
#
# Revision 1.2  2006/05/18 00:40:10  bernie
# Setup for AVR development.
#
# Revision 1.1  2006/03/22 09:51:53  bernie
# Add build infrastructure.
#
#

# Set to 1 for verbose build output, 0 for terse output
V := 0

default: all

include config.mk

# Include subtargets
include app/demo/demo.mk
#include app/triface/triface.mk

include rules.mk


