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
        self._loadModuleData()
    
    def _loadModuleData(self):
        modules = bertos_utils.loadModuleInfosDict(self._projectInfoRetrieve("SOURCES_PATH"))
        lists = bertos_utils.loadDefineListsDict(self._projectInfoRetrieve("SOURCES_PATH"))
        configurations = {}
        for module, informations in modules.items():
            configurations.update(bertos_utils.loadConfigurationInfos(self._projectInfoRetrieve("SOURCES_PATH") + "/" + informations["configuration"]))
        print "modules", modules
        print "lists", lists
        print "configurations", configurations
    
    def _setupUi(self):
        self.pageContent.moduleList.setSortingEnabled(True)
        self.pageContent.propertyTable.horizontalHeader().setResizeMode(QHeaderView.ResizeToContents)
        self.pageContent.propertyTable.horizontalHeader().setVisible(False)
        self.pageContent.propertyTable.verticalHeader().setResizeMode(QHeaderView.ResizeToContents)
        self.pageContent.propertyTable.verticalHeader().setVisible(False)
        self.pageContent.propertyTable.setColumnCount(2)
        self.pageContent.propertyTable.setRowCount(0)
    
    def _connectSignals(self):
        pass