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
if PYQT_VERSION_STR > "4.4.3":
    import qvariant_converter_new as qvariant_converter
else:
    import qvariant_converter_old as qvariant_converter

class BCpuPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "cpu_select.ui")
        self.setTitle(self.tr("Select the CPU"))
        self._connectSignals()
        self._setupUi()
    
    def _populateCpuList(self):
        self.pageContent.cpuList.clear()
        self.pageContent.cpuList.setCurrentItem(None)
        infos = bertos_utils.loadCpuInfos(self._projectInfoRetrieve("SOURCES_PATH"))
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
        self.pageContent.descriptionLabel.setVisible(False)
        self.pageContent.descriptionLabel.setText("")
    
    def reloadData(self):
        self._populateCpuList()
        cpuName = self._projectInfoRetrieve("CPU_NAME")
        self._setupUi()
        if not cpuName is None:
            self._selectItem(cpuName)
        self.emit(SIGNAL("completeChanged()"))
    
    def isComplete(self):
        if self.pageContent.cpuList.currentRow() != -1:
            self._projectInfoStore("CPU_INFOS", qvariant_converter.getDict(self.pageContent.cpuList.currentItem().data(Qt.UserRole)))
            return True
        else:
            return False
        
    def rowChanged(self):
        description = qvariant_converter.getDict(self.pageContent.cpuList.currentItem().data(Qt.UserRole))["CPU_DESC"]
        description = qvariant_converter.getStringList(description)
        self.pageContent.descriptionLabel.setText("<br>".join(description))
        self.pageContent.descriptionLabel.setVisible(True)
        self.emit(SIGNAL("completeChanged()"))
