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
        return False

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
        presets = self.projectInfo("PRESETS")
        for preset, info in presets.items():
            board_list = self.pageContent.boardList
            item = QListWidgetItem(info["name"], board_list)
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
        self.pageContent.descriptionLabel.setText(selected_preset['description'])
        self._last_selected = preset_path

    ####
