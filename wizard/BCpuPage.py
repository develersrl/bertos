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
        self.freq_modified = False
    
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
            tag_dict = self.projectInfo("ALL_CPU_TAGS")
            for tag in tag_dict:
                if tag in infos["CPU_TAGS"] + [infos["CPU_NAME"], infos["CORE_CPU"]]:
                    tag_dict[tag] = "True"
                else:
                    tag_dict[tag] = "False"
            self.setProjectInfo("ALL_CPU_TAGS", tag_dict)
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
        self.connect(self.pageContent.frequencySpinBox, SIGNAL("editingFinished()"), self.freqChanged)

    def reloadData(self):
        """
        Overload of the BWizardPage reloadData method.
        """
        QApplication.instance().setOverrideCursor(Qt.WaitCursor)
        bertos_utils.loadSourceTree(self.project())
        self.populateCpuList()
        cpu_name = self.projectInfo("CPU_NAME")
        selected_freq = self.projectInfo("SELECTED_FREQ")
        self.setupUi()
        if not cpu_name is None:
            self.selectItem(cpu_name)
            if not selected_freq is None:
                self.setFrequency(selected_freq)
                self.freq_modified = True
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
        if not self.freq_modified:
            # Retrieve the default cpu frequency when the value isn't already modified
            current_freq = qvariant_converter.getDict(self.pageContent.cpuList.currentItem().data(Qt.UserRole))["CPU_DEFAULT_FREQ"]
            current_freq = qvariant_converter.getString(current_freq)
            current_freq = long(current_freq.replace("U", "").replace("L", ""))
            self.pageContent.frequencySpinBox.setValue(long(current_freq))
        self.pageContent.descriptionLabel.setText("<br>".join(description))
        self.pageContent.descriptionLabel.setVisible(True)
        self.emit(SIGNAL("completeChanged()"))
    
    def freqChanged(self):
        """
        Slot called when the user change the frequency value.
        """
        self.freq_modified = True
        self.emit(SIGNAL("completeChanged()"))

    ####
    
    def populateCpuList(self):
        """
        Fills the cpu list.
        """
        self.pageContent.cpuList.clear()
        self.pageContent.cpuList.setCurrentItem(None)
        infos = bertos_utils.loadCpuInfos(self.project())
        tag_list = bertos_utils.getTagSet(infos)
        # Create, fill and store the dict with the tags
        tag_dict = {}
        for element in tag_list:
            tag_dict[element] = "False"
        self.setProjectInfo("ALL_CPU_TAGS", tag_dict)
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
    
    def setFrequency(self, frequency):
        self.pageContent.frequencySpinBox.setValue(long(frequency))