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
        self.setTitle(self.tr("Configure the modules"))
        self._setupUi()
        self._connectSignals()
    
    def reloadData(self):
        self._fillModuleList()
    
    def _setupUi(self):
        self.pageContent.moduleList.setSortingEnabled(True)
        self.pageContent.propertyTable.horizontalHeader().setResizeMode(QHeaderView.ResizeToContents)
        self.pageContent.propertyTable.horizontalHeader().setVisible(False)
        self.pageContent.propertyTable.verticalHeader().setVisible(False)
        self.pageContent.propertyTable.setColumnCount(2)
        self.pageContent.propertyTable.setRowCount(0)
    
    def _connectSignals(self):
        self.connect(self.pageContent.moduleList, SIGNAL("currentRowChanged(int)"), self.moduleItemClicked)
    
    def _fillModuleList(self):
        moduleInfos = bertos_utils.loadModuleInfos(self._projectInfoRetrieve("SOURCES_PATH"))
        self._projectInfoStore("MODULE_INFO", moduleInfos)
        for key, value in moduleInfos.items():
            self.pageContent.moduleList.addItem(QListWidgetItem(key))
    
    def _fillParameterTable(self, parameters):
        self.pageContent.propertyTable.clear()
        self.pageContent.propertyTable.setRowCount(len(parameters))
        for index, key in enumerate(parameters):
            self.pageContent.propertyTable.setItem(index, 0, QTableWidgetItem(key))
            if "type" in parameters[key]["informations"].keys():
                if parameters[key]["informations"]["type"] == "boolean":
                    checkBox = QCheckBox()
                    self.pageContent.propertyTable.setCellWidget(index, 1, checkBox)
                    checkBox.setChecked(parameters[key]["value"] == "1")
                if parameters[key]["informations"]["type"] == "int":
                    spinBox = QSpinBox()
                    if "max" in parameters[key]["informations"].keys():
                        spinBox.setMaximum(int(parameters[key]["informations"]["max"]))
                    if "min" in parameters[key]["informations"].keys():
                        spinBox.setMinimum(int(parameters[key]["informations"]["min"]))
                    if "long" in parameters[key]["informations"].keys():
                        if bool(parameters[key]["informations"]["long"]):
                            spinBox.setSuffix("L")
                    self.pageContent.propertyTable.setCellWidget(index, 1, spinBox)
                    spinBox.setValue(int(parameters[key]["value"]))
            else:
                self.pageContent.propertyTable.setItem(index, 1, QTableWidgetItem(parameters[key]["value"]))
    
    def moduleItemClicked(self):
        module = unicode(self.pageContent.moduleList.currentItem().text())
        self._fillParameterTable(self._projectInfoRetrieve("MODULE_INFO")[module])

            

