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
    
    def _fillPropertyTable(self):
        module = unicode(self.pageContent.moduleTable.item(self.pageContent.moduleTable.currentRow(), 1).text())
        configuration = self._projectInfoRetrieve("MODULES")[module]["configuration"]
        configurations = self._projectInfoRetrieve("CONFIGURATIONS")[configuration]
        self.pageContent.propertyTable.clear()
        self.pageContent.propertyTable.setRowCount(len(configurations))
        for index, parameter in enumerate(configurations):
            self.pageContent.propertyTable.setItem(index, 0, QTableWidgetItem(parameter))
            if "type" in configurations[parameter]["informations"].keys() and configurations[parameter]["informations"]["type"] == "boolean":
                ## boolean parameter
                checkBox = QCheckBox()
                self.pageContent.propertyTable.setCellWidget(index, 1, checkBox)
                if configurations[parameter]["value"] == "1":
                    checkBox.setChecked(True)
                else:
                    checkBox.setChecked(False)
            elif "type" in configurations[parameter]["informations"].keys() and configurations[parameter]["informations"]["type"] == "enum":
                ## enum parameter
                comboBox = QComboBox()
                self.pageContent.propertyTable.setCellWidget(index, 1, comboBox)
                enum = self._projectInfoRetrieve("LISTS")[configurations[parameter]["informations"]["value_list"]]
                for element in enum:
                    comboBox.addItem(element)
            else:
                ## int, long or undefined type parameter
                spinBox = QSpinBox()
                self.pageContent.propertyTable.setCellWidget(index, 1, spinBox)
                if "min" in configurations[parameter]["informations"].keys():
                    minimum = int(configurations[parameter]["informations"]["min"])
                else:
                    minimum = -32768
                spinBox.setMinimum(minimum)
                if "max" in configurations[parameter]["informations"].keys():
                    maximum = int(configurations[parameter]["infomations"]["max"])
                else:
                    maximum = 32767
                spinBox.setMaximum(maximum)
                if "long" in configurations[parameter]["informations"].keys() and configurations[parameter]["informations"]["long"] == "True":
                    spinBox.setSuffix("L")
                spinBox.setValue(int(configurations[parameter]["value"].replace("L", "")))
    
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