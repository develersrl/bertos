#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id:$
#
# Author: Lorenzo Berni <duplo@develer.com>
#


from BWizardPage import *
import bertos_utils

class BCpuPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "cpu_select.ui")
        self.setTitle(self.tr("Select the CPU"))
        self._connectSignals()
    
    def _populateCpuList(self):
        self.pageContent.cpuList.clear()
        infos = bertos_utils.loadCpuInfos(self._projectInfoRetrieve("SOURCES_PATH"))
        for cpu in infos:
            self.pageContent.cpuList.addItem(QListWidgetItem(cpu["CPU_NAME"]))
    
    def _connectSignals(self):
        self.connect(self.pageContent.cpuList, SIGNAL("itemSelectionChanged()"), self.rowChanged)

    def reloadData(self):
        self._populateCpuList()
    
    def isComplete(self):
        if self.pageContent.cpuList.currentRow() != -1:
            self._projectInfoStore("CPU_NAME", self.pageContent.cpuList.currentItem())
            return True
        else:
            return False
        
    def rowChanged(self):
        self.emit(SIGNAL("completeChanged()"))
