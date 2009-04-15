#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id$
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
        self.setTitle(self.tr("Create the BeRTOS project"))
        self._completed = False

    ## Overloaded BWizardPage methods ##
    
    def setupUi(self):
        summary = self.pageContent.summaryTree
        summary.setHeaderHidden(True)
        summary.setColumnCount(1)
        self.setButtonText(QWizard.NextButton, self.tr("Create"))
    
    def reloadData(self):
        self.setupUi()
        self.pageContent.summaryTree.clear()
        top_level = []
        folder_title = QTreeWidgetItem(QStringList([self.tr("Project folder")]))
        folder_item = QTreeWidgetItem(folder_title, QStringList([os.path.normpath(self.projectInfo("PROJECT_PATH"))]))
        top_level.append(folder_title)
        version_title = QTreeWidgetItem(QStringList([self.tr("BeRTOS version")]))
        sources_path = self.projectInfo("SOURCES_PATH")
        version = QTreeWidgetItem(version_title, QStringList([self.tr("version: ") + bertos_utils.bertosVersion(sources_path)]))
        source_path = QTreeWidgetItem(version_title, QStringList([self.tr("path: ") + os.path.normpath(sources_path)]))
        top_level.append(version_title)
        cpu_title = QTreeWidgetItem(QStringList([self.tr("CPU")]))
        cpu_name = QTreeWidgetItem(cpu_title, QStringList([self.tr("cpu name: ") + self.projectInfo("CPU_NAME")]))
        top_level.append(cpu_title)
        toolchain_title = QTreeWidgetItem(QStringList([self.tr("Toolchain")]))
        toolchain_info = self.projectInfo("TOOLCHAIN")
        if "target" in toolchain_info.keys():
            toolchain_target = QTreeWidgetItem(toolchain_title, QStringList([self.tr("target: " + toolchain_info["target"])]))
        version = ""
        if "version" in toolchain_info.keys():
            version += "version: " + "GCC " + toolchain_info["version"] + " "
        if "build" in toolchain_info.keys():
            version += "(" + toolchain_info["build"] + ")"
        if "version" in toolchain_info.keys():
            toolchain_target = QTreeWidgetItem(toolchain_title, QStringList([version]))
        toolchain_path = QTreeWidgetItem(toolchain_title, QStringList([self.tr("path: " + os.path.normpath(toolchain_info["path"]))]))
        top_level.append(toolchain_title)
        module_title = QTreeWidgetItem(QStringList([self.tr("Modules")]))
        configurations = self.projectInfo("CONFIGURATIONS")
        module_categories = {}
        for module, information in self.projectInfo("MODULES").items():
            if information["enabled"]:
                if information["category"] not in module_categories.keys():
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
    
    ####