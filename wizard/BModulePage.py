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

from DefineException import *
from const import *

class BModulePage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/module_select.ui")
        self.setTitle(self.tr("Configure the BeRTOS modules"))
        self._setupUi()
        self._controlGroup = QControlGroup()
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
        try:
            modules = bertos_utils.loadModuleInfosDict(self._projectInfoRetrieve("SOURCES_PATH"))
            lists = bertos_utils.loadDefineListsDict(self._projectInfoRetrieve("SOURCES_PATH"))
            configurations = {}
            for module, informations in modules.items():
                configurations[informations["configuration"]] = bertos_utils.loadConfigurationInfos(self._projectInfoRetrieve("SOURCES_PATH") +
                                                                                                    "/" + informations["configuration"])
        except ModuleDefineException, e:
            self._exceptionOccurred(self.tr("Error parsing module information in file %1").arg(e.parameter))
        except EnumDefineException, e:
            self._exceptionOccurred(self.tr("Error parsing enum informations in file %1").arg(e.parameter))
        except ConfigurationDefineException, e:
            self._exceptionOccurred(self.tr("Error parsing configuration informations in file %1").arg(e.parameter))
        else:
            self._projectInfoStore("MODULES", modules)
            self._projectInfoStore("LISTS", lists)
            self._projectInfoStore("CONFIGURATIONS", configurations)
    
    def _fillModuleTable(self):
        modules = self._projectInfoRetrieve("MODULES")
        if modules is None:
            return
        self.pageContent.moduleTable.setRowCount(len(modules))
        for index, module in enumerate(modules):
            self.pageContent.moduleTable.setItem(index, 1, QTableWidgetItem(module))
            checkBox = QCheckBox()
            self._buttonGroup.addButton(checkBox, index)
            self.pageContent.moduleTable.setCellWidget(index, 0, checkBox)
            checkBox.setChecked(modules[module]["enabled"])
    
    def _fillPropertyTable(self):
        module = self._currentModule()
        self._controlGroup.clear()
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
                self._controlGroup.addControl(index, checkBox)
            elif "type" in configurations[property]["informations"].keys() and configurations[property]["informations"]["type"] == "enum":
                ## enum property
                comboBox = QComboBox()
                self.pageContent.propertyTable.setCellWidget(index, 1, comboBox)
                enum = self._projectInfoRetrieve("LISTS")[configurations[property]["informations"]["value_list"]]
                for i, element in enumerate(enum):
                    comboBox.addItem(element)
                    if element == configurations[property]["value"]:
                        comboBox.setCurrentIndex(i)
                self._controlGroup.addControl(index, comboBox)
            else:
                ## int, long or undefined type property
                spinBox = QSpinBox()
                self.pageContent.propertyTable.setCellWidget(index, 1, spinBox)
                if bertos_utils.isInt(configurations[property]):
                    minimum = -32768
                    maximmum = 32767
                    suff = ""
                elif bertos_utils.isLong(configurations[property]):
                    minimum = -2147483648L
                    maximum = 2147483647L
                    suff = "L"
                elif bertos_utils.isUnsigned(configurations[property]):
                    minimum = 0
                    maximum = 65535
                    suff = "U"
                elif bertos_utils.isUnsignedLong(configurations[property]):
                    minimum = 0
                    maximum = 4294967295L
                    suff = "UL"
                if "min" in configurations[property]["informations"].keys():
                    minimum = int(configurations[property]["informations"]["min"])
                if "max" in configurations[property]["informations"].keys():
                    maximum = int(configurations[property]["informations"]["max"])
                spinBox.setRange(minimum, maximum)
                spinBox.setSuffix(suff)
                spinBox.setValue(int(configurations[property]["value"].replace("L", "").replace("U", "")))
                self._controlGroup.addControl(index, spinBox)
    
    def _currentModule(self):
        return unicode(self.pageContent.moduleTable.item(self.pageContent.moduleTable.currentRow(), 1).text())
    
    def _currentModuleConfigurations(self):
        return self._configurations(self._currentModule())
    
    def _currentProperty(self):
        return qvariant_converter.getString(self.pageContent.propertyTable.item(self.pageContent.propertyTable.currentRow(), 0).data(Qt.UserRole))
    
    def _currentPropertyItem(self):
        return self.pageContent.propertyTable.item(self.pageContent.propertyTable.currentRow(), 0)
    
    def _module(self, row):
        return unicode(self.pageContent.moduleTable.item(row, 1).text())
    
    def _configurations(self, module):
        configuration = self._projectInfoRetrieve("MODULES")[module]["configuration"]
        return self._projectInfoRetrieve("CONFIGURATIONS")[configuration]
    
    def _resetPropertyDescription(self):
        for index in range(self.pageContent.propertyTable.rowCount()):
            propertyName = qvariant_converter.getString(self.pageContent.propertyTable.item(index, 0).data(Qt.UserRole))
            self.pageContent.propertyTable.item(index, 0).setText(propertyName)
    
    def _showPropertyDescription(self):
        self._resetPropertyDescription()
        configurations = self._currentModuleConfigurations()
        if self._currentProperty() in configurations.keys():
            description = configurations[self._currentProperty()]["description"]
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
        self.connect(self._controlGroup, SIGNAL("stateChanged"), self._saveValue)
    
    def _saveValue(self, index):
        property = qvariant_converter.getString(self.pageContent.propertyTable.item(index, 0).data(Qt.UserRole))
        configuration = self._projectInfoRetrieve("MODULES")[self._currentModule()]["configuration"]
        configurations = self._projectInfoRetrieve("CONFIGURATIONS")
        if "type" not in configurations[configuration][property]["informations"].keys() or configurations[configuration][property]["informations"]["type"] == "int":
            configurations[configuration][property]["value"] = str(self.pageContent.propertyTable.cellWidget(index, 1).value())
        elif configurations[configuration][property]["informations"]["type"] == "enum":
            configurations[configuration][property]["value"] = unicode(self.pageContent.propertyTable.cellWidget(index, 1).currentText())
        elif configurations[configuration][property]["informations"]["type"] == "boolean":
            if self.pageContent.propertyTable.cellWidget(index, 1).isChecked():
                configurations[configuration][property]["value"] = "1"
            else:
                configurations[configuration][property]["value"] = "0"
        self._projectInfoStore("CONFIGURATIONS", configurations)
    
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
            choice = QMessageBox.warning(self, self.tr("Dependency error"), message, QMessageBox.Yes | QMessageBox.No, QMessageBox.Yes)
            if choice == QMessageBox.Yes:
                for module in unsatisfied:
                    modules = self._projectInfoRetrieve("MODULES")
                    modules[module]["enabled"] = True
                for index in range(self.pageContent.moduleTable.rowCount()):
                    if unicode(self.pageContent.moduleTable.item(index, 1).text()) in unsatisfied:
                        self._buttonGroup.button(index).setChecked(True)
    
    def _moduleUnselected(self, unselectedModule):
        modules = self._projectInfoRetrieve("MODULES")
        modules[unselectedModule]["enabled"] = False
        self._projectInfoStore("MODULES", modules)
        unsatisfied = self.unselectDependencyCheck(unselectedModule)
        if len(unsatisfied) > 0:
            message = self.tr("The module %1 is needed by the following modules:\n%2.\n\nDo you want to resolve automatically the problem?")
            message = message.arg(unselectedModule).arg(", ".join(unsatisfied))
            choice = QMessageBox.warning(self, self.tr("Dependency error"), message, QMessageBox.Yes | QMessageBox.No, QMessageBox.Yes)
            if choice == QMessageBox.Yes:
                for module in unsatisfied:
                    modules = self._projectInfoRetrieve("MODULES")
                    modules[module]["enabled"] = False
                for index in range(self.pageContent.moduleTable.rowCount()):
                    if unicode(self.pageContent.moduleTable.item(index, 1).text()) in unsatisfied:
                        self._buttonGroup.button(index).setChecked(False)
    
    
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

class QControlGroup(QObject):
    def __init__(self):
        QObject.__init__(self)
        self._controls = {}
    
    def addControl(self, id, control):
        self._controls[id] = control
        if type(control) == QCheckBox:
            self.connect(control, SIGNAL("stateChanged(int)"), lambda: self._stateChanged(id))
        elif type(control) == QSpinBox:
            self.connect(control, SIGNAL("valueChanged(int)"), lambda: self._stateChanged(id))
        elif type(control) == QComboBox:
            self.connect(control, SIGNAL("currentIndexChanged(int)"), lambda: self._stateChanged(id))
    
    def clear(self):
        self._controls = {}
    
    def _stateChanged(self, id):
        self.emit(SIGNAL("stateChanged"), id)