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
        self._confirmGroup = QWidgetGroup(self.pageContent.summaryTree,
                                            self.pageContent.createButton)
        self._finalGroup = QWidgetGroup(self.pageContent.iconLabel,
                                            self.pageContent.textLabel)
        self._finalGroup.setVisible(False)
        summary = self.pageContent.summaryTree
        summary.setHeaderHidden(True)
        summary.setColumnCount(1)
    
    def reloadData(self):
        self._completed = False
        self._setupUi()
        self.pageContent.summaryTree.clear()
        topLevel = []
        folderTitle = QTreeWidgetItem(QStringList([self.tr("Project folder")]))
        folderItem = QTreeWidgetItem(folderTitle, QStringList([self._projectInfoRetrieve("PROJECT_PATH")]))
        topLevel.append(folderTitle)
        versionTitle = QTreeWidgetItem(QStringList([self.tr("BeRTOS version")]))
        sources_path = self._projectInfoRetrieve("SOURCES_PATH")
        version = QTreeWidgetItem(versionTitle, QStringList([self.tr("version: ") + bertos_utils.bertosVersion(sources_path)]))
        sourcePath = QTreeWidgetItem(versionTitle, QStringList([self.tr("path: ") + sources_path]))
        topLevel.append(versionTitle)
        cpuTitle = QTreeWidgetItem(QStringList([self.tr("CPU")]))
        cpuName = QTreeWidgetItem(cpuTitle, QStringList([self.tr("cpu name: ") + self._projectInfoRetrieve("CPU_NAME")]))
        topLevel.append(cpuTitle)
        toolchainTitle = QTreeWidgetItem(QStringList([self.tr("Toolchain")]))
        toolchainInfo = self._projectInfoRetrieve("TOOLCHAIN")
        if "target" in toolchainInfo.keys():
            toolchainTarget = QTreeWidgetItem(toolchainTitle, QStringList([self.tr("target: " + toolchainInfo["target"])]))
        if "version" in toolchainInfo.keys():
            toolchainTarget = QTreeWidgetItem(toolchainTitle, QStringList([self.tr("version: " + "GCC " + toolchainInfo["version"] + " (" + toolchainInfo["build"] + ")")]))
        toolchainPath = QTreeWidgetItem(toolchainTitle, QStringList([self.tr("path: " + toolchainInfo["path"])]))
        topLevel.append(toolchainTitle)
        moduleTitle = QTreeWidgetItem(QStringList([self.tr("Modules")]))
        configurations = self._projectInfoRetrieve("CONFIGURATIONS")
        for module, information in self._projectInfoRetrieve("MODULES").items():
            if information["enabled"]:
                moduleItem = QTreeWidgetItem(moduleTitle, QStringList([module + " - " + information["description"]]))
                if len(information["configuration"]) > 0:
                    for property, data in configurations[information["configuration"]].items():
                        configurationItem = QTreeWidgetItem(moduleItem, QStringList([data["description"] + " - " + data["value"]]))
        topLevel.append(moduleTitle)
        self.pageContent.summaryTree.insertTopLevelItems(0, topLevel)
    
    def _connectSignals(self):
        self.connect(self.pageContent.createButton, SIGNAL("clicked(bool)"), self._createProject)
    
    def _createProject(self):
        QApplication.setOverrideCursor(QCursor(Qt.WaitCursor))
        self._confirmGroup.setVisible(False)
        bertos_utils.createBertosProject(self.wizard().project())
        self._finalGroup.setVisible(True)
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

