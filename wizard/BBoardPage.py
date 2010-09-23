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
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import os

from PyQt4.QtCore import *
from PyQt4.QtGui import *

from BWizardPage import BWizardPage

from BCpuPage import BCpuPage

import const
import qvariant_converter
from bertos_utils import presetList, _cmp

class BBoardPage(BWizardPage):
    """
    Initial page of the alternative route. Permit to select one of the
    predefined projects for supported board.
    """
    
    def __init__(self):
        BWizardPage.__init__(self, const.UI_LOCATION + "/board_select.ui")
        self.setTitle(self.tr("Select your development board"))

    ## Overloaded QWizardPage methods ##

    def isComplete(self):
        """
        Overload of the QWizardPage isComplete method.
        """
        if self.selected:
            preset_path = qvariant_converter.getDict(self.selected.data(Qt.UserRole))
            preset_path = qvariant_converter.getStringDict(preset_path["info"])
            preset_path = preset_path["path"]
            self.setProjectInfo("PROJECT_BOARD", preset_path)
            self.setProjectInfo("PROJECT_FROM_PRESET", True)
            self.setProjectInfo("PRESET_LOADED", False)
            return True
        else:
            return False

    def nextId(self):
        wizard = self.wizard()
        if not self.projectInfo("PROJECT_FROM_PRESET"):
            return wizard.pageIndex(BCpuPage)
        else:
            return QWizardPage.nextId(self)

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
        self.connect(self.pageContent.boardList, SIGNAL("currentItemChanged(QListWidgetItem*,QListWidgetItem*)"), self.updateUi)
        self.connect(self.pageContent.boardList, SIGNAL("currentItemChanged(QListWidgetItem*,QListWidgetItem*)"), self, SIGNAL("completeChanged()"))
        self.connect(self.pageContent.customButton, SIGNAL("clicked()"), self.customButtonClicked)

    def reloadData(self, previous_id=None):
        """
        Overload of the BWizardPage reloadData method.
        """
        self.project.loadProjectPresets()
        preset_list = self.projectInfo("PRESET_TREE")
        preset_list = preset_list["children"]
        preset_list = sorted(preset_list.values(), _cmp)
        self.setItems(preset_list)
        project_from_preset = self.projectInfo("PROJECT_FROM_PRESET")
        project_board = self.projectInfo("PROJECT_BOARD")
        if not (project_from_preset and project_board):
            self.pageContent.boardList.setCurrentRow(0)

    ####

    ## Slots ##

    def updateUi(self):
        if self.selected:
            info_dict = qvariant_converter.getDict(self.selected.data(Qt.UserRole))
            info_dict = qvariant_converter.getStringDict(info_dict["info"])
            description = info_dict.get("description", "")
            path = unicode(QUrl.fromLocalFile(info_dict["path"]).toString())
            description = description.replace("$path", path)
            self.pageContent.descriptionArea.setHtml(description)

    def customButtonClicked(self):
        self.setProjectInfo("PROJECT_FROM_PRESET", False)
        self.wizard().next()

    ####

    def setItems(self, preset_list):
        self.pageContent.boardList.clear()
        selected_board = self.projectInfo("PROJECT_BOARD")
        for item_data in preset_list:
            item_name = item_data["info"].get("name", item_data["info"]["filename"])
            item_icon = os.path.join(item_data["info"]["path"], const.PREDEFINED_BOARD_ICON_FILE)
            if not os.path.exists(item_icon):
                item_icon = const.PREDEFINED_BOARD_DEFAULT_ICON
            item = QListWidgetItem(QIcon(item_icon), item_name)
            item.setData(Qt.UserRole, qvariant_converter.convertDict(item_data))
            self.pageContent.boardList.addItem(item)
            if selected_board and selected_board == item_data["info"]["path"]:
                self.pageContent.boardList.setCurrentItem(item)

    @property
    def selected(self):
        return self.pageContent.boardList.currentItem()
