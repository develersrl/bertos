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
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
#
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import os

from PyQt4.QtGui import *

from BWizardPage import *
import bertos_utils

from const import *

class BCreationPage(BWizardPage):

    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/project_creation.ui")
	self.setTitle(self.tr("Project summary"))
	self.setSubTitle(self.tr("Your project is ready to be created. Review your selections and press the \"Create\" button"))
        self._completed = False

    ## Overloaded BWizardPage methods ##

    def connectSignals(self):
        self.connect(self.pageContent.codeliteCheckBox, SIGNAL("stateChanged(int)"), self.codelitePluginChanged)

    def setupUi(self):
        summary = self.pageContent.summaryTree
        summary.setHeaderHidden(True)
        summary.setColumnCount(1)
        self.pageContent.codeliteCheckBox.setChecked(False)
        self.codelitePluginChanged()
        self.setButtonText(QWizard.NextButton, self.tr("Create"))

    def reloadData(self, previous_id=None):
        self.setupUi()
        self.pageContent.summaryTree.clear()
        top_level = []
        folder_title = QTreeWidgetItem(QStringList([self.tr("Project folder")]))
        folder_item = QTreeWidgetItem(folder_title, QStringList([os.path.normpath(self.projectInfo("PROJECT_PATH"))]))
        top_level.append(folder_title)
        version_title = QTreeWidgetItem(QStringList([self.tr("BeRTOS version")]))
        sources_path = self.projectInfo("BERTOS_PATH")
        version = QTreeWidgetItem(version_title, QStringList([self.tr("version: ") + bertos_utils.bertosVersion(sources_path)]))
        source_path = QTreeWidgetItem(version_title, QStringList([self.tr("path: ") + os.path.normpath(sources_path)]))
        top_level.append(version_title)
        cpu_title = QTreeWidgetItem(QStringList([self.tr("CPU")]))
        cpu_name = QTreeWidgetItem(cpu_title, QStringList([self.tr("cpu name: ") + self.projectInfo("CPU_NAME")]))
        cpu_freq = QTreeWidgetItem(cpu_title, QStringList([self.tr("frequency: ") + self.projectInfo("SELECTED_FREQ") + "Hz"]))
        top_level.append(cpu_title)
        toolchain_title = QTreeWidgetItem(QStringList([self.tr("Toolchain")]))
        toolchain_info = self.projectInfo("TOOLCHAIN")
        if "target" in toolchain_info:
            toolchain_target = QTreeWidgetItem(toolchain_title, QStringList([self.tr("target: " + toolchain_info["target"].strip())]))
        version = ""
        if "version" in toolchain_info:
            version += "version: " + "GCC " + toolchain_info["version"].strip() + " "
        if "build" in toolchain_info:
            version += "(" + toolchain_info["build"].strip() + ")"
        if "version" in toolchain_info:
            toolchain_target = QTreeWidgetItem(toolchain_title, QStringList([version]))
        toolchain_path = QTreeWidgetItem(toolchain_title, QStringList([self.tr("path: " + os.path.normpath(toolchain_info["path"]))]))
        top_level.append(toolchain_title)
        module_title = QTreeWidgetItem(QStringList([self.tr("Selected modules")]))
        configurations = self.projectInfo("CONFIGURATIONS")
        module_categories = {}
        for module, information in self.projectInfo("MODULES").items():
            if information["enabled"]:
                if information["category"] not in module_categories:
                    module_categories[information["category"]] = []
                moduleItem = QTreeWidgetItem(QStringList([module + " - " + information["description"]]))
                module_categories[information["category"]].append(moduleItem)
                if len(information["configuration"]) > 0:
                    for start, property in configurations[information["configuration"]]["paramlist"]:
                        # If the final char of the brief is a dot (".") removes it.
                        brief = configurations[information["configuration"]][property]["brief"]
                        if brief[-1] == ".":
                            brief = brief[:-1]
                        configuration_item = QTreeWidgetItem(moduleItem, QStringList([brief + ": " + configurations[information["configuration"]][property]["value"]]))
        for key, value in module_categories.items():
            category_item = QTreeWidgetItem(module_title, QStringList([key]))
            category_item.addChildren(value)
        top_level.append(module_title)
        self.pageContent.summaryTree.insertTopLevelItems(0, top_level)
        for item in top_level:
            self.pageContent.summaryTree.expandItem(item)

    ####

    ## Slots ##

    def codelitePluginChanged(self):
        if not self.pageContent.codeliteCheckBox.isChecked():
            output = ["codelite"]
        else:
            output= []
        self.setProjectInfo("OUTPUT", output)
        self.setPlugins(output)

    ####
