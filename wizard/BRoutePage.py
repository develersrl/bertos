#!/usr/bin/env python
# encoding: utf-8
#
# This file is part of bertos.
#
# Bertos is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this
# file does not by itself cause the resulting executable to be covered by
# the GNU General Public License.  This exception does not however
# invalidate any other reasons why the executable file might be covered by
# the GNU General Public License.
#
# Copyright 2010 Develer S.r.l. (http://www.develer.com/)
#
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

from PyQt4.QtCore import *
from PyQt4.QtGui import *

from BWizardPage import BWizardPage

from BOutputPage import BOutputPage
from BToolchainPage import BToolchainPage

import const
import qvariant_converter
from bertos_utils import presetList

class BRoutePage(BWizardPage):
    """
    Let the user choice Advanced or base route.
    """
    
    def __init__(self):
        BWizardPage.__init__(self, const.UI_LOCATION + "/route_select.ui")
        self.setTitle(self.tr("Select Advanced or Base setup"))
        self._last_selected = None

    ## Overloaded QWizardPage methods ##

    def isComplete(self):
        """
        Overload of the QWizardPage isComplete method.
        """
        return False

    def nextId(self):
        """
        Overload of the QWizardPage nextId method.
        """
        # Route to Toolchain page if the user select advanced
        # or to Output page if the user select base
        if self.advanced:
            return self.wizard().pageIndex(BToolchainPage)
        else:
            return self.wizard().pageIndex(BOutputPage)

    ####

    ## Overloaded BWizardPage methods ##

    def setupUi(self):
        """
        Overload of the BWizardPage setupUi method.
        """
        pass

    def connectSignals(self):
        """
        Overload of the BWizardPage connectSignals method.
        """
        pass

    def reloadData(self):
        """
        Overload of the BWizardPage reloadData method.
        """
        pass

    ####

    ## Slots ##


    ####

    @property
    def advanced(self):
        return self.pageContent.advancedButton.isChecked()
