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

import os

from PyQt4.QtGui import *

from BWizardPage import *
import bertos_utils

from const import *

class BCreationPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/project_creation.ui")
        self.setTitle(self.tr("Create the BeRTOS project"))
        self._setupUi()
        self._connectSignals()
        self._completed = False
    
    def _setupUi(self):
        self._confirm_group = QWidgetGroup(self.pageContent.summaryTree,
                                            self.pageContent.createButton)
        self._final_group = QWidgetGroup(self.pageContent.iconLabel,
                                            self.pageContent.textLabel)
        self._final_group.setVisible(False)
        summary = self.pageContent.summaryTree
        summary.setHeaderHidden(True)
        summary.setColumnCount(1)
    
    def reloadData(self):
        self._completed = False
        self._setupUi()
        self.pageContent.summaryTree.clear()
        top_level = []
        folder_title = QTreeWidgetItem(QStringList([self.tr("Project folder")]))
        folder_item = QTreeWidgetItem(folder_title, QStringList([os.path.normpath(self._projectInfoRetrieve("PROJECT_PATH"))]))
        top_level.append(folder_title)
        version_title = QTreeWidgetItem(QStringList([self.tr("BeRTOS version")]))
        sources_path = self._projectInfoRetrieve("SOURCES_PATH")
        version = QTreeWidgetItem(version_title, QStringList([self.tr("version: ") + bertos_utils.bertosVersion(sources_path)]))
        source_path = QTreeWidgetItem(version_title, QStringList([self.tr("path: ") + os.path.normpath(sources_path)]))
        top_level.append(version_title)
        cpu_title = QTreeWidgetItem(QStringList([self.tr("CPU")]))
        cpu_name = QTreeWidgetItem(cpu_title, QStringList([self.tr("cpu name: ") + self._projectInfoRetrieve("CPU_NAME")]))
        top_level.append(cpu_title)
        toolchain_title = QTreeWidgetItem(QStringList([self.tr("Toolchain")]))
        toolchain_info = self._projectInfoRetrieve("TOOLCHAIN")
        if "target" in toolchain_info.keys():
            toolchain_target = QTreeWidgetItem(toolchain_title, QStringList([self.tr("target: " + toolchain_info["target"])]))
        if "version" in toolchain_info.keys():
            toolchain_target = QTreeWidgetItem(toolchain_title, QStringList([self.tr("version: " + "GCC " + toolchain_info["version"] + " (" + toolchain_info["build"] + ")")]))
        toolchain_path = QTreeWidgetItem(toolchain_title, QStringList([self.tr("path: " + os.path.normpath(toolchain_info["path"]))]))
        top_level.append(toolchain_title)
        module_title = QTreeWidgetItem(QStringList([self.tr("Modules")]))
        configurations = self._projectInfoRetrieve("CONFIGURATIONS")
        module_categories = {}
        for module, information in self._projectInfoRetrieve("MODULES").items():
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
    
    def _connectSignals(self):
        self.connect(self.pageContent.createButton, SIGNAL("clicked(bool)"), self._createProject)
    
    def _createProject(self):
        QApplication.setOverrideCursor(QCursor(Qt.WaitCursor))
        self._confirm_group.setVisible(False)
        bertos_utils.createBertosProject(self.wizard().project())
        self._final_group.setVisible(True)
        self._completed = True
        QApplication.restoreOverrideCursor()
        self.emit(SIGNAL("completeChanged()"))
    
    def isComplete(self):
        return self._completed

class QWidgetGroup(QObject):
    """
    Container class, this class contains widgets and permit to set some
    properties of the contained widgets at the same time.
    """
    def __init__(self, *elements):
        self._widgets = []
        for element in elements:
            self._widgets.append(element)
    
    def addWidget(self, widget):
        if widget not in self._widgets:
            self._widgets.append(widget)
    
    def setVisible(self, visible):
        for widget in self._widgets:
            widget.setVisible(visible)
    
    def isVisible(self):
        for widget in self._widgets:
            if not widget.isVisible():
                return False
        return True

