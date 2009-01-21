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
            self.pageContent.propertyTable.setItem(index, 1, QTableWidgetItem(parameters[key]["value"]))
    
    def moduleItemClicked(self):
        module = unicode(self.pageContent.moduleList.currentItem().text())
        self._fillParameterTable(self._projectInfoRetrieve("MODULE_INFO")[module])

            

