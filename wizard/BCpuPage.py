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
import qvariant_converter

from const import *

class BCpuPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/cpu_select.ui")
        self.setTitle(self.tr("Select the CPU"))
        self._connectSignals()
        self._setupUi()
    
    def _populateCpuList(self):
        self.pageContent.cpuList.clear()
        self.pageContent.cpuList.setCurrentItem(None)
        infos = bertos_utils.loadCpuInfos(self._project())
        for cpu in infos:
            item = QListWidgetItem(cpu["CPU_NAME"])
            item.setData(Qt.UserRole, qvariant_converter.convertDict(cpu))
            self.pageContent.cpuList.addItem(item)
    
    def _connectSignals(self):
        self.connect(self.pageContent.cpuList, SIGNAL("itemSelectionChanged()"), self.rowChanged)
    
    def _selectItem(self, cpu):
        elements = self.pageContent.cpuList.findItems(cpu, Qt.MatchCaseSensitive)
        if len(elements) == 1:
            self.pageContent.cpuList.setCurrentItem(elements[0])
    
    def _setupUi(self):
        self.pageContent.cpuList.setSortingEnabled(True)
        self.pageContent.descriptionLabel.setVisible(False)
        self.pageContent.descriptionLabel.setText("")
    
    def reloadData(self):
        bertos_utils.loadSourceTree(self._project())
        self._populateCpuList()
        cpuName = self._projectInfoRetrieve("CPU_NAME")
        self._setupUi()
        if not cpuName is None:
            self._selectItem(cpuName)
        self.emit(SIGNAL("completeChanged()"))
    
    def isComplete(self):
        if self.pageContent.cpuList.currentRow() != -1:
            infos = qvariant_converter.getDict(self.pageContent.cpuList.currentItem().data(Qt.UserRole))
            for key, value in infos.items():
                if type(CPU_DEF[key]) == list:
                    infos[key] = qvariant_converter.getStringList(value)
                if type(CPU_DEF[key]) == str or type(CPU_DEF) == unicode:
                    infos[key] = qvariant_converter.getString(value)
            self._projectInfoStore("CPU_INFOS", infos)
            self._projectInfoStore("CPU_NAME", unicode(self.pageContent.cpuList.currentItem().text()))
            return True
        else:
            return False
        
    def rowChanged(self):
        description = qvariant_converter.getDict(self.pageContent.cpuList.currentItem().data(Qt.UserRole))["CPU_DESC"]
        description = qvariant_converter.getStringList(description)
        self.pageContent.descriptionLabel.setText("<br>".join(description))
        self.pageContent.descriptionLabel.setVisible(True)
        self.emit(SIGNAL("completeChanged()"))
