#!/usr/bin/env python
# encoding: utf-8
#
# This file is part of BeRTOS.
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

from PyQt4 import uic

from PyQt4.QtCore import *
from PyQt4.QtGui import *

from BWizardPage import BWizardPage

from BCreationPage import BCreationPage
from BToolchainPage import BToolchainPage

from DefineException import ModuleDefineException

from bertos_utils import _cmp
from toolchain_manager import ToolchainManager

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
        self.pageContent.categoryDescription.setText(self.preset_data["info"].get("description", ""))
        for preset in sorted(self.preset_data["children"].values(), _cmp):
            item_name = preset["info"].get("name", preset["info"]["filename"])
            item_icon = os.path.join(preset["info"]["path"], const.PREDEFINED_BOARD_ICON_FILE)
            if not os.path.exists(item_icon):
                item_icon = const.PREDEFINED_BOARD_DEFAULT_PROJECT_ICON
            item_icon = QIcon(item_icon)
            item = QListWidgetItem(item_icon, item_name)
            item.setData(Qt.UserRole, qvariant_converter.convertString(preset["info"]["path"]))
            self.pageContent.presetList.addItem(item)
        self.pageContent.presetList.setCurrentRow(0)
        self.updateUi()

    def connectSignals(self):
        self.connect(self.pageContent.presetList, SIGNAL("currentItemChanged(QListWidgetItem *, QListWidgetItem*)"), self.updateUi)
        self.connect(self.pageContent.presetList, SIGNAL("currentItemChanged(QListWidgetItem *, QListWidgetItem*)"), self, SIGNAL("completeChanged()"))

    def updateUi(self):
        if self.selected:
            preset_path = qvariant_converter.getString(self.selected.data(Qt.UserRole))
            preset = self.preset_data["children"][preset_path]
            description = preset["info"].get("description", "")
            path = unicode(QUrl.fromLocalFile(preset_path).toString())
            description = description.replace("$path", path)
            self.pageContent.descriptionArea.setHtml(description)
    
    @property
    def selected(self):
        return self.pageContent.presetList.currentItem()
        

class BProjectPresets(BWizardPage):
    def __init__(self):
        BWizardPage.__init__(self, const.UI_LOCATION + "/project_presets.ui")

    ## Overloaded QWizardPage methods ##

    def isComplete(self):
        preset_path = self.selected_path
        if preset_path:
            self.setProjectInfo("PROJECT_PRESET", preset_path)
            self.setProjectInfo("BASE_MODE", not self.advanced)
            return True
        else:
            self.setProjectInfo("PROJECT_PRESET", None)
            return False

    def validatePage(self):
        """
        This hack permits to load the preset once, when the user go press the
        Next button.
        """
        preset_path = self.selected_path
        try:
            QApplication.instance().setOverrideCursor(Qt.WaitCursor)
            try:
                self.project.loadProjectFromPreset(preset_path)
            except ModuleDefineException, e:
                self.exceptionOccurred(self.tr("Error parsing line '%2' in file %1").arg(e.path).arg(e.line))
            self.setProjectInfo("PRESET_LOADED", True)
        finally:
            QApplication.instance().restoreOverrideCursor()
        # Return always True, this is a fake validation.
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
                    return self.wizard().pageIndex(BCreationPage)
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
    
    def reloadData(self, previous_id=None):
        if not self.projectInfo("PRESET_LOADED"):
            preset_path = self.projectInfo("PROJECT_BOARD")
            preset_tree = self.projectInfo("PRESET_TREE")
            preset_list = preset_tree["children"][preset_path]["children"]
            preset_list = sorted(preset_list.values(), _cmp)
            self.setTitle(self.tr("Select the project template for %1").arg(preset_tree["children"][preset_path]["info"].get("name", preset_tree["children"][preset_path]["info"]["filename"])))
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

    @property
    def advanced(self):
        if self.selected_data:
            return self.selected_data["info"].get("advanced", False)
        else:
            return None

    @property
    def selected_path(self):
        current_widget = self.pageContent.boardTabWidget.currentWidget()
        preset_path = None
        if current_widget:
            current_item = current_widget.pageContent.presetList.currentItem()
            if current_item:
                preset_path = current_item.data(Qt.UserRole)
                preset_path = qvariant_converter.getString(preset_path)
        return preset_path

    @property
    def selected_data(self):
        if self.selected_path:
            current_widget = self.pageContent.boardTabWidget.currentWidget()
            return current_widget.preset_data["children"][self.selected_path]
        else:
            return None
