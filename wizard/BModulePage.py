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


class BModulePage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "module_select.ui")
        self.setTitle(self.tr("Configure the BeRTOS modules"))
        self._setupUi()
        self._connectSignals()
    
    def reloadData(self):
        self._setupButtonGroup()
        self._loadModuleData()
        self._fillModuleTable()
    
    def _setupButtonGroup(self):
        self._buttonGroup = QButtonGroup()
        self._buttonGroup.setExclusive(False)
        self.connect(self._buttonGroup, SIGNAL("buttonClicked(int)"), self._moduleSelectionChanged)
    
    def _loadModuleData(self):
        modules = bertos_utils.loadModuleInfosDict(self._projectInfoRetrieve("SOURCES_PATH"))
        lists = bertos_utils.loadDefineListsDict(self._projectInfoRetrieve("SOURCES_PATH"))
        configurations = {}
        for module, informations in modules.items():
            configurations[informations["configuration"]] = bertos_utils.loadConfigurationInfos(self._projectInfoRetrieve("SOURCES_PATH") +
                                                                                                "/" + informations["configuration"])
        self._projectInfoStore("MODULES", modules)
        self._projectInfoStore("LISTS", lists)
        self._projectInfoStore("CONFIGURATIONS", configurations)
    
    def _fillModuleTable(self):
        modules = self._projectInfoRetrieve("MODULES")
        self.pageContent.moduleTable.setRowCount(len(modules))
        for index, module in enumerate(modules):
            self.pageContent.moduleTable.setItem(index, 1, QTableWidgetItem(module))
            checkBox = QCheckBox()
            self._buttonGroup.addButton(checkBox, index)
            self.pageContent.moduleTable.setCellWidget(index, 0, checkBox)
            checkBox.setChecked(modules[module]["enabled"])
    
    def _fillPropertyTable(self):
        module = self._currentModule()
        configuration = self._projectInfoRetrieve("MODULES")[module]["configuration"]
        configurations = self._projectInfoRetrieve("CONFIGURATIONS")[configuration]
        self.pageContent.propertyTable.clear()
        self.pageContent.propertyTable.setRowCount(len(configurations))
        for index, property in enumerate(configurations):
            item = QTableWidgetItem(property)
            item.setData(Qt.UserRole, qvariant_converter.convertString(property))
            self.pageContent.propertyTable.setItem(index, 0, item)
            if "type" in configurations[property]["informations"].keys() and configurations[property]["informations"]["type"] == "boolean":
                ## boolean property
                checkBox = QCheckBox()
                self.pageContent.propertyTable.setCellWidget(index, 1, checkBox)
                if configurations[property]["value"] == "1":
                    checkBox.setChecked(True)
                else:
                    checkBox.setChecked(False)
            elif "type" in configurations[property]["informations"].keys() and configurations[property]["informations"]["type"] == "enum":
                ## enum property
                comboBox = QComboBox()
                self.pageContent.propertyTable.setCellWidget(index, 1, comboBox)
                enum = self._projectInfoRetrieve("LISTS")[configurations[property]["informations"]["value_list"]]
                for element in enum:
                    comboBox.addItem(element)
            else:
                ## int, long or undefined type property
                spinBox = QSpinBox()
                self.pageContent.propertyTable.setCellWidget(index, 1, spinBox)
                if "min" in configurations[property]["informations"].keys():
                    minimum = int(configurations[property]["informations"]["min"])
                else:
                    minimum = -32768
                spinBox.setMinimum(minimum)
                if "max" in configurations[property]["informations"].keys():
                    maximum = int(configurations[property]["informations"]["max"])
                else:
                    maximum = 32767
                spinBox.setMaximum(maximum)
                if "long" in configurations[property]["informations"].keys() and configurations[property]["informations"]["long"] == "True":
                    spinBox.setSuffix("L")
                spinBox.setValue(int(configurations[property]["value"].replace("L", "")))
    
    def _currentModule(self):
        return unicode(self.pageContent.moduleTable.item(self.pageContent.moduleTable.currentRow(), 1).text())
    
    def _currentModuleConfigurations(self):
        return self._projectInfoRetrieve("MODULES")[self._currentModule()]["configuration"]
    
    def _currentProperty(self):
        return qvariant_converter.getString(self.pageContent.propertyTable.item(self.pageContent.propertyTable.currentRow(), 0).data(Qt.UserRole))
    
    def _currentPropertyItem(self):
        return self.pageContent.propertyTable.item(self.pageContent.propertyTable.currentRow(), 0)
    
    def _resetPropertyDescription(self):
        for index in range(self.pageContent.propertyTable.rowCount()):
            propertyName = qvariant_converter.getString(self.pageContent.propertyTable.item(index, 0).data(Qt.UserRole))
            self.pageContent.propertyTable.item(index, 0).setText(propertyName)
    
    def _showPropertyDescription(self):
        self._resetPropertyDescription()
        description = self._projectInfoRetrieve("CONFIGURATIONS")[self._currentModuleConfigurations()][self._currentProperty()]["description"]
        name = self._currentProperty()
        self._currentPropertyItem().setText(name + "\n" + description)
    
    def _setupUi(self):
        self.pageContent.moduleTable.horizontalHeader().setResizeMode(QHeaderView.ResizeToContents)
        self.pageContent.moduleTable.horizontalHeader().setStretchLastSection(True)
        self.pageContent.moduleTable.horizontalHeader().setVisible(False)
        self.pageContent.moduleTable.verticalHeader().setResizeMode(QHeaderView.ResizeToContents)
        self.pageContent.moduleTable.verticalHeader().setVisible(False)
        self.pageContent.moduleTable.setColumnCount(2)
        self.pageContent.moduleTable.setRowCount(0)
        self.pageContent.propertyTable.horizontalHeader().setResizeMode(QHeaderView.Stretch)
        self.pageContent.propertyTable.horizontalHeader().setVisible(False)
        self.pageContent.propertyTable.verticalHeader().setResizeMode(QHeaderView.ResizeToContents)
        self.pageContent.propertyTable.verticalHeader().setVisible(False)
        self.pageContent.propertyTable.setColumnCount(2)
        self.pageContent.propertyTable.setRowCount(0)
    
    def _connectSignals(self):
        self.connect(self.pageContent.moduleTable, SIGNAL("itemSelectionChanged()"), self._fillPropertyTable)
        self.connect(self.pageContent.propertyTable, SIGNAL("itemSelectionChanged()"), self._showPropertyDescription)

    def _moduleSelectionChanged(self, index):
        module = unicode(self.pageContent.moduleTable.item(index, 1).text())
        if self._buttonGroup.button(index).isChecked():
            self._moduleSelected(module)
        else:
            self._moduleUnselected(module)
    
    def _moduleSelected(self, selectedModule):
        modules = self._projectInfoRetrieve("MODULES")
        modules[selectedModule]["enabled"] = True
        self._projectInfoStore("MODULES", modules)
        depends = self._projectInfoRetrieve("MODULES")[selectedModule]["depends"]
        unsatisfied = self.selectDependencyCheck(selectedModule)
        if len(unsatisfied) > 0:
            message = self.tr("The module %1 needs the following modules:\n%2.\n\nDo you want to resolve automatically the problem?")
            message = message.arg(selectedModule).arg(", ".join(unsatisfied))
            QMessageBox.warning(self, self.tr("Dependency error"), message, QMessageBox.Yes | QMessageBox.No, QMessageBox.Yes)
    
    def _moduleUnselected(self, unselectedModule):
        modules = self._projectInfoRetrieve("MODULES")
        modules[unselectedModule]["enabled"] = False
        self._projectInfoStore("MODULES", modules)
        unsatisfied = self.unselectDependencyCheck(unselectedModule)
        if len(unsatisfied) > 0:
            message = self.tr("The module %1 is needed by the following modules:\n%2.\n\nDo you want to resolve automatically the problem?")
            message = message.arg(unselectedModule).arg(", ".join(unsatisfied))
            QMessageBox.warning(self, self.tr("Dependency error"), message, QMessageBox.Yes | QMessageBox.No, QMessageBox.Yes)
    
    def selectDependencyCheck(self, module):
        unsatisfied = set()
        modules = self._projectInfoRetrieve("MODULES")
        for dependency in modules[module]["depends"]:
            if not modules[dependency]["enabled"]:
                unsatisfied |= set([dependency])
                if dependency not in unsatisfied:
                    unsatisfied |= self.selectDependencyCheck(dependency)
        return unsatisfied
    
    def unselectDependencyCheck(self, dependency):
        unsatisfied = set()
        modules = self._projectInfoRetrieve("MODULES")
        for module, informations in modules.items():
            if dependency in informations["depends"] and informations["enabled"]:
                unsatisfied |= set([module])
                if dependency not in unsatisfied:
                    unsatisfied |= self.unselectDependencyCheck(module)
        return unsatisfied