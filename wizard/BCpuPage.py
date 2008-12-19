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
        self._setupUi()
    
    def _populateCpuList(self):
        self.pageContent.cpuList.clear()
        infos = bertos_utils.loadCpuInfos(self._projectInfoRetrieve("SOURCES_PATH"))
        for cpu in infos:
            item = QListWidgetItem(cpu["CPU_NAME"])
            # The CPU_DESC field in the cpu definition is a list of string, so we need to 
            # store it as a QStringList in a QVariant
            item.setData(Qt.UserRole, QVariant(QStringList(cpu["CPU_DESC"])))
            self.pageContent.cpuList.addItem(item)
    
    def _connectSignals(self):
        self.connect(self.pageContent.cpuList, SIGNAL("itemSelectionChanged()"), self.rowChanged)
    
    def _selectItem(self, cpu):
        elements = self.pageContent.cpuList.findItems(cpu, Qt.MatchCaseSensitive)
        if len(elements) == 1:
            self.pageContent.cpuList.setCurrentItem(elements[0])
    
    def _setupUi(self):
        self.pageContent.descriptionLabel.setVisible(False)
    
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
        description = self.pageContent.cpuList.currentItem().data(Qt.UserRole).toStringList()
        # We need to convert the list of QString in a list of unicode
        description =  [unicode(line) for line in description]
        self.pageContent.descriptionLabel.setText("<br>".join(description))
        self.pageContent.descriptionLabel.setVisible(True)
        self.emit(SIGNAL("completeChanged()"))
