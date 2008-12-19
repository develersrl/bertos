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
    
    def _selectItem(self, cpu):
        elements = self.pageContent.cpuList.findItems(cpu, Qt.MatchCaseSensitive)
        if len(elements) == 1:
            self.pageContent.cpuList.setCurrentItem(elements[0])

    def reloadData(self):
        self._populateCpuList()
        cpuName = self._projectInfoRetrieve("CPU_NAME")
        if not cpuName is None:
            self._selectItem(cpuName)
    
    def isComplete(self):
        if self.pageContent.cpuList.currentRow() != -1:
            self._projectInfoStore("CPU_NAME", self.pageContent.cpuList.currentItem().text())
            return True
        else:
            return False
        
    def rowChanged(self):
        self.emit(SIGNAL("completeChanged()"))
