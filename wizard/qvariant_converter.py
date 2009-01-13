#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id:$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

from PyQt4.QtCore import PYQT_VERSION_STR

if PYQT_VERSION_STR > "4.4.3":
    from qvariant_converter_new import *
else:
    from qvariant_converter_old import *
