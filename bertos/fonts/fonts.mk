#
# $Id$
# Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile fragment for DevLib demo application.
#
# Author: Bernardo Innocenti <bernie@develer.com>
#

%.c: %.bdf
	bertos/fonts/convbdf $< > $@
