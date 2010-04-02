#!/usr/bin/env python
# encoding: utf-8
#
# This file is part of slimqc.
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

from BCpuPage import BCpuPage
from BOutputPage import BOutputPage

import const
import qvariant_converter
from bertos_utils import presetList

class BBoardPage(BWizardPage):
    """
    Initial page of the alternative route. Permit to select one of the
    predefined projects for supported board.
    """
    
    def __init__(self):
        BWizardPage.__init__(self, const.UI_LOCATION + "/board_select.ui")
        self.setTitle(self.tr("Select the board from the predefined ones"))
        self._last_selected = None

    ## Overloaded QWizardPage methods ##

    def isComplete(self):
        """
        Overload of the QWizardPage isComplete method.
        """
        return self.pageContent.boardList.currentItem() is not None

    def nextId(self):
        """
        Overload of the QWizardPage nextId method.
        """
        # Stub of nextId logic
        if self.advanced:
            self.setProjectInfo("PRESET_ADVANCED_CONFIG", True)
            return self.wizard().pageIndex(BCpuPage)
        # Search for suitable toolchains. If there isn't one return 
        # BToolchainPage id (BToolchainPage should then route to BOutputPage
        # instead of BModulePage).

        # If a suitable toolchain is found the user has to be prompted
        # directly to the BOutputPage
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
        self.connect(self.pageContent.boardList, SIGNAL('itemSelectionChanged()'), self.itemSelectionChanged)

    def reloadData(self):
        """
        Overload of the BWizardPage reloadData method.
        """
        presets = presetList("/Users/duplo/Development/bertos")
        self.setProjectInfo("PRESETS", presets)
        self.populatePresetList()

    def populatePresetList(self):
        self.pageContent.boardList.clear()
        presets = self.projectInfo("PRESETS")
        for preset, info in presets.items():
            board_list = self.pageContent.boardList
            item = QListWidgetItem(info["PRESET_NAME"], board_list)
            item.setData(Qt.UserRole, qvariant_converter.convertString(preset))
            if self._last_selected == preset:
                self.pageContent.boardList.setCurrentItem(item)
        if not self._last_selected and self.pageContent.boardList.count():
            self.pageContent.boardList.setCurrentRow(0)

    ####

    ## Slots ##

    def itemSelectionChanged(self):
        preset_path = qvariant_converter.getString(self.pageContent.boardList.currentItem().data(Qt.UserRole))
        presets = self.projectInfo("PRESETS")
        selected_preset = presets[preset_path]
        text_components = [
            "Board: %s" %selected_preset["PRESET_NAME"],
            "CPU: %s" %selected_preset["CPU_NAME"],
        ]
        if selected_preset["PRESET_DESCRIPTION"]:
            text_components.append("Description: %s" %selected_preset["PRESET_DESCRIPTION"])
        text = "\n".join(text_components)
        self.pageContent.descriptionLabel.setText(text)
        self._last_selected = preset_path
        self.emit(SIGNAL("completeChanged()"))

    ####

    @property
    def advanced(self):
        return self.pageContent.advancedCheckBox.isChecked()