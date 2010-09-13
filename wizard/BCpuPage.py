#!/usr/bin/env python
# encoding: utf-8
#
# This file is part of BeRTOS.
#
# Bertos is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this
# file does not by itself cause the resulting executable to be covered by
# the GNU General Public License.  This exception does not however
# invalidate any other reasons why the executable file might be covered by
# the GNU General Public License.
#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
#
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
        self.setTitle(self.tr("Select the CPU on your board"))
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
                if key in CPU_DEF:
                    if type(CPU_DEF[key]) == list:
                        infos[key] = qvariant_converter.getStringList(value)
                    if type(CPU_DEF[key]) == str or type(CPU_DEF) == unicode:
                        infos[key] = qvariant_converter.getString(value)
                elif key.startswith(MK_PARAM_ID):
                    infos[key] = qvariant_converter.getString(value)
                else:
                    del infos[key]
            self.setProjectInfo("CPU_INFOS", infos)
            self.setProjectInfo("CPU_NAME", unicode(self.pageContent.cpuList.currentItem().text()))
            self.setProjectInfo("SELECTED_FREQ", unicode(long(self.pageContent.frequencySpinBox.value())))
            tag_dict = self.projectInfo("ALL_CPU_TAGS")
            for tag in tag_dict:
                if tag in infos["CPU_TAGS"] + [infos["CPU_NAME"], infos["TOOLCHAIN"]]:
                    tag_dict[tag] = True
                else:
                    tag_dict[tag] = False
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
        self.pageContent.frequencyLabel.setVisible(False)
        self.pageContent.frequencySpinBox.setVisible(False)
        preset_advanced = self.projectInfo("PRESET_ADVANCED_CONFIG")
        if preset_advanced:
            self.pageContent.cpuList.setEnabled(False)

    def connectSignals(self):
        """
        Overload of the BWizardPage connectSignals method.
        """
        self.connect(self.pageContent.cpuList, SIGNAL("currentItemChanged(QListWidgetItem *, QListWidgetItem*)"), self.rowChanged)
        self.connect(self.pageContent.frequencySpinBox, SIGNAL("editingFinished()"), self.freqChanged)

    def reloadData(self, previous_id=None):
        """
        Overload of the BWizardPage reloadData method.
        """
        try:
            QApplication.instance().setOverrideCursor(Qt.WaitCursor)
            self.project.loadSourceTree()
            self.populateCpuList()
            cpu_name = self.projectInfo("CPU_NAME")
            selected_freq = self.projectInfo("SELECTED_FREQ")
            self.setupUi()
            if cpu_name:
                self.selectItem(cpu_name)
                if selected_freq:
                    self.setFrequency(selected_freq)
                    self.freq_modified = True
        finally:
            QApplication.instance().restoreOverrideCursor()
        self.emit(SIGNAL("completeChanged()"))

    ####
    
    ## Slots ##

    def rowChanged(self):
        """
        Slot called when the user select an entry from the cpu list.
        """
        if self.pageContent.cpuList.currentItem():
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
        infos = self.project.getCpuInfos()
        tag_list = bertos_utils.getTagSet(infos)
        # Create, fill and store the dict with the tags
        tag_dict = {}
        for element in tag_list:
            tag_dict[element] = False
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
