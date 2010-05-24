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

import os

from PyQt4 import uic

from PyQt4.QtCore import *
from PyQt4.QtGui import *

from BWizardPage import BWizardPage

import const
import qvariant_converter

class BProjectPresetsPage(QWidget):
    def __init__(self, preset_data, parent=None):
        QWidget.__init__(self, parent)
        self.pageContent = uic.loadUi(os.path.join(const.DATA_DIR, const.UI_LOCATION, "preset_page.ui"), None)
        self.project = QApplication.instance().project
        self.settings = QApplication.instance().settings
        self.preset_data = preset_data
        layout = QVBoxLayout()
        layout.addWidget(self.pageContent)
        self.setLayout(layout)
        self.setupUi()
        self.connectSignals()

    def setupUi(self):
        self.pageContent.presetList.clear()
        def _cmp(x, y):
            return cmp(x["info"].get('ord', 0), y["info"].get('ord', 0))
        for preset in sorted(self.preset_data["children"].values(), _cmp):
            item_name = preset["info"].get("name", preset["info"]["filename"])
            item_icon = os.path.join(preset["info"]["path"], const.PREDEFINED_BOARD_ICON_FILE)
            if not os.path.exists(item_icon):
                item_icon = const.PREDEFINED_BOARD_DEFAULT_ICON
            item_icon = QIcon(item_icon)
            item = QListWidgetItem(item_icon, item_name)
            item.setData(Qt.UserRole, qvariant_converter.convertString(preset["info"]["path"]))
            self.pageContent.presetList.addItem(item)
        self.pageContent.presetList.setCurrentRow(0)
        self.updateUi()

    def connectSignals(self):
        self.connect(self.pageContent.presetList, SIGNAL("itemSelectionChanged()"), self.updateUi)
        self.connect(self.pageContent.presetList, SIGNAL("itemSelectionChanged()"), self, SIGNAL("completeChanged()"))

    def updateUi(self):
        if self.selected:
            preset_path = qvariant_converter.getString(self.selected.data(Qt.UserRole))
            preset = self.preset_data["children"][preset_path]
            self.pageContent.descriptionLabel.setText(preset["info"].get("description", ""))
            image = os.path.join(preset["info"]["path"], const.PREDEFINED_BOARD_IMAGE_FILE)
            if not os.path.exists(image):
                image = const.PREDEFINED_BOARD_DEFAULT_IMAGE
            self.pageContent.imageLabel.setPixmap(QPixmap(image))
    
    @property
    def selected(self):
        return self.pageContent.presetList.currentItem()
        

class BProjectPresets(BWizardPage):
    def __init__(self):
        BWizardPage.__init__(self, const.UI_LOCATION + "/project_presets.ui")

    ## Overloaded QWizardPage methods ##

    def isComplete(self):
        current_widget = self.pageContent.boardTabWidget.currentWidget()
        preset_path = None
        if current_widget:
            current_item = current_widget.pageContent.presetList.currentItem()
            if current_item:
                preset_path = current_item.data(Qt.UserRole)
                preset_path = qvariant_converter.getString(preset_path)
        if preset_path:
            self.setProjectInfo("PROJECT_PRESET", preset_path)
            return True
        else:
            self.setProjectInfo("PROJECT_PRESET", None)
            return False
    ####
    
    ## Overloaded BWizardPage methods ##
    
    def reloadData(self):
        preset_path = self.projectInfo("PROJECT_BOARD")
        preset_tree = self.projectInfo("PRESET_TREE")
        preset_list = preset_tree["children"][preset_path]["children"]
        def _cmp(x, y):
            return cmp(x["info"].get('ord', 0), y["info"].get('ord', 0))
        preset_list = sorted(preset_list.values(), _cmp)
        self.setupTabs(preset_list)

    def connectSignals(self):
        self.connect(self.pageContent.boardTabWidget, SIGNAL("currentChanged(int)"), self, SIGNAL("completeChanged()"))

    ####
    
    ## Slots ##
    ####

    def setupTabs(self, preset_list):
        self.pageContent.boardTabWidget.clear()
        for preset in preset_list:
            icon = os.path.join(preset["info"]["path"], ".icon.png")
            preset_page = BProjectPresetsPage(preset)
            if os.path.exists(icon):
                self.pageContent.boardTabWidget.addTab(preset_page, QIcon(icon), preset["info"].get("name", preset["info"]["filename"]))
            else:
                self.pageContent.boardTabWidget.addTab(preset_page, preset["info"].get("name", preset["info"]["filename"]))
            self.connect(preset_page, SIGNAL("completeChanged()"), self, SIGNAL("completeChanged()"))