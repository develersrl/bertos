#
# $Id$
# Copyright 2002, 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Author: Bernardo Innocenti <bernie@develer.com>
#

# Set to 1 for verbose build output, 0 for terse output
V := 0

default: all

include config.mk

# Include subtargets
include app/demo/demo.mk
#include app/triface/triface.mk
#include app/at91sam7s/at91sam7s.mk
include rules.mk
