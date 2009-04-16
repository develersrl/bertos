#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#


from BWizardPage import *
import bertos_utils
import qvariant_converter

from const import *

class BCpuPage(BWizardPage):
    """
    Page of the wizard that permits to choose the cpu from the supported ones.
    """
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/cpu_select.ui")
        self.setTitle(self.tr("Select the CPU"))
    
    ## Overloaded QWizardPage methods ##

    def isComplete(self):
        """
        Overload of the QWizardPage isComplete method.
        """
        if self.pageContent.cpuList.currentRow() != -1:
            self.pageContent.frequencyLabel.setVisible(True)
            self.pageContent.frequencySpinBox.setVisible(True)
            infos = qvariant_converter.getDict(self.pageContent.cpuList.currentItem().data(Qt.UserRole))
            for key, value in infos.items():
                if type(CPU_DEF[key]) == list:
                    infos[key] = qvariant_converter.getStringList(value)
                if type(CPU_DEF[key]) == str or type(CPU_DEF) == unicode:
                    infos[key] = qvariant_converter.getString(value)
            self.setProjectInfo("CPU_INFOS", infos)
            self.setProjectInfo("CPU_NAME", unicode(self.pageContent.cpuList.currentItem().text()))
            self.setProjectInfo("SELECTED_FREQ", unicode(long(self.pageContent.frequencySpinBox.value())))
            return True
        else:
            return False
    
    ####
    
    ## Overloaded BWizardPage methods ##

    def setupUi(self):
        """
        Overload of the BWizardPage setupUi method.
        """
        self.pageContent.cpuList.setSortingEnabled(True)
        self.pageContent.descriptionLabel.setVisible(False)
        self.pageContent.descriptionLabel.setText("")
        self.pageContent.frequencyLabel.setVisible(False)
        self.pageContent.frequencySpinBox.setVisible(False)

    def connectSignals(self):
        """
        Overload of the BWizardPage connectSignals method.
        """
        self.connect(self.pageContent.cpuList, SIGNAL("itemSelectionChanged()"), self.rowChanged)
        self.connect(self.pageContent.frequencySpinBox, SIGNAL("valueChanged(double)"), self.freqChanged)

    def reloadData(self):
        """
        Overload of the BWizardPage reloadData method.
        """
        QApplication.instance().setOverrideCursor(Qt.WaitCursor)
        bertos_utils.loadSourceTree(self.project())
        self.populateCpuList()
        cpu_name = self.projectInfo("CPU_NAME")
        self.setupUi()
        if not cpu_name is None:
            self.selectItem(cpu_name)
        cpu_frequency = self.projectInfo("SELECTED_FREQ")
        if not cpu_frequency is None:
            self.pageContent.frequencySpinBox.setValue(cpu_frequency)
        QApplication.instance().restoreOverrideCursor()
        self.emit(SIGNAL("completeChanged()"))

    ####
    
    ## Slots ##

    def rowChanged(self):
        """
        Slot called when the user select an entry from the cpu list.
        """
        description = qvariant_converter.getDict(self.pageContent.cpuList.currentItem().data(Qt.UserRole))["CPU_DESC"]
        description = qvariant_converter.getStringList(description)
        frequency = qvariant_converter.getDict(self.pageContent.cpuList.currentItem().data(Qt.UserRole))["CPU_DEFAULT_FREQ"]
        frequency = qvariant_converter.getString(frequency)
        frequency = frequency.replace("U", "").replace("L", "")
        self.pageContent.descriptionLabel.setText("<br>".join(description))
        self.pageContent.descriptionLabel.setVisible(True)
        self.pageContent.frequencySpinBox.setValue(long(frequency))
        self.emit(SIGNAL("completeChanged()"))
    
    def freqChanged(self):
        self.emit(SIGNAL("completeChanged()"))

    ####
    
    def populateCpuList(self):
        """
        Fills the cpu list.
        """
        self.pageContent.cpuList.clear()
        self.pageContent.cpuList.setCurrentItem(None)
        infos = bertos_utils.loadCpuInfos(self.project())
        for cpu in infos:
            item = QListWidgetItem(cpu["CPU_NAME"])
            item.setData(Qt.UserRole, qvariant_converter.convertDict(cpu))
            self.pageContent.cpuList.addItem(item)
    
    def selectItem(self, cpu):
        """
        Selects the given cpu from the list.
        """
        elements = self.pageContent.cpuList.findItems(cpu, Qt.MatchCaseSensitive)
        if len(elements) == 1:
            self.pageContent.cpuList.setCurrentItem(elements[0])