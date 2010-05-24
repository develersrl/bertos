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
from toolchain_manager import ToolchainManager

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
        self.setProjectInfo("BASE_MODE", not self.advanced)
        return True

    def nextId(self):
        """
        Overload of the QWizardPage nextId method.
        """
        # Route to Toolchain page if the user select advanced
        # or to Output page if the user select base
        if self.advanced:
            return self.wizard().pageIndex(BToolchainPage)
        else:
            cpu_info = self.projectInfo("CPU_INFOS")
            if cpu_info:
                target = cpu_info["TOOLCHAIN"]
                # Try to find a suitable toolchain automatically
                tm = ToolchainManager()
                suitable_toolchains = tm.suitableToolchains(target)
                if len(suitable_toolchains) == 1:
                    toolchain = suitable_toolchains.pop()
                    toolchain_info = tm._validateToolchain(toolchain)
                    toolchain_info["path"] = toolchain
                    self.setProjectInfo("TOOLCHAIN", toolchain_info)
                    return self.wizard().pageIndex(BOutputPage)
                else:
                    return self.wizard().pageIndex(BToolchainPage)
            else:
                # It seems that the nextId method is called before the
                # reloadData one (that is called after the page changing.
                #
                # TODO: fix this awful code lines
                target = None
                return self.wizard().pageIndex(BToolchainPage)

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
        self.connect(self.pageContent.baseButton, SIGNAL("toggled(bool)"), self, SIGNAL("completeChanged()"))

    def reloadData(self):
        """
        Overload of the BWizardPage reloadData method.
        """
        preset = self.projectInfo('PROJECT_PRESET')
        try:
            QApplication.instance().setOverrideCursor(Qt.WaitCursor)
            self.project.loadProjectFromPreset(preset)
        finally:
            QApplication.instance().restoreOverrideCursor()

    ####

    ## Slots ##


    ####

    @property
    def advanced(self):
        return self.pageContent.advancedButton.isChecked()

    @property
    def empty_main(self):
        if self.advanced:
            return self.pageContent.emptyCheckBox.isChecked()
        else:
            return False
