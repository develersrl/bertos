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
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import os

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from BWizardPage import *
import bertos_utils

from bertos_utils import SupportedException
from DefineException import *
from const import *

class BModulePage(BWizardPage):
    """
    Page of the wizard that permits to select and configurate the BeRTOS modules.
    """
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/module_select.ui")
        self.setTitle(self.tr("Configure the BeRTOS modules"))
        self._control_group = QControlGroup()
        ## special connection needed for the QControlGroup
        self.connect(self._control_group, SIGNAL("stateChanged"), self.saveValue)
    
    ## Overloaded BWizardPage methods ##

    def setupUi(self):
        """
        Overload of BWizardPage setupUi method.
        """
        self.pageContent.moduleTree.clear()
        self.pageContent.moduleTree.setHeaderHidden(True)
        self.pageContent.propertyTable.horizontalHeader().setResizeMode(QHeaderView.Stretch)
        self.pageContent.propertyTable.horizontalHeader().setVisible(False)
        self.pageContent.propertyTable.verticalHeader().setResizeMode(QHeaderView.ResizeToContents)
        self.pageContent.propertyTable.verticalHeader().setVisible(False)
        self.pageContent.propertyTable.setColumnCount(2)
        self.pageContent.propertyTable.setRowCount(0)
        self.pageContent.moduleLabel.setVisible(False)
        self.pageContent.warningLabel.setVisible(False)
    
    def connectSignals(self):
        """
        Overload of the BWizardPage connectSignals method.
        """
        self.connect(self.pageContent.moduleTree, SIGNAL("itemPressed(QTreeWidgetItem*, int)"), self.fillPropertyTable)
        self.connect(self.pageContent.moduleTree, SIGNAL("itemPressed(QTreeWidgetItem*, int)"), self.moduleClicked)
        self.connect(self.pageContent.moduleTree, SIGNAL("itemChanged(QTreeWidgetItem*, int)"), self.dependencyCheck)

    def reloadData(self, previous_id=None):
        """
        Overload of the BWizardPage reloadData method.
        """
        # Check if the user are approaching this page from the previous or the
        # next one.
        if previous_id is None or previous_id < self.wizard().currentId():
            try:
                QApplication.instance().setOverrideCursor(Qt.WaitCursor)
                self.setupUi()
                self.loadModuleData()
                self.fillModuleTree()
            finally:
                QApplication.instance().restoreOverrideCursor()
    
    ####
    
    ## Slots ##

    def moduleClicked(self, item, column):
        self.setBold(item, False)

    def fillPropertyTable(self):
        """
        Slot called when the user selects a module from the module tree.
        Fills the property table using the configuration parameters defined in
        the source tree.
        """
        module = self.currentModule()
        if module:
            try:
                supported = bertos_utils.isSupported(self.project, module=module)
            except SupportedException, e:
                self.exceptionOccurred(self.tr("Error evaluating \"%1\" for module %2").arg(e.support_string).arg(module))
                supported = True
            self._control_group.clear()
            configuration = self.projectInfo("MODULES")[module]["configuration"]
            module_description = self.projectInfo("MODULES")[module]["description"]
            self.pageContent.moduleLabel.setText(module_description)
            self.pageContent.moduleLabel.setVisible(True)
            if not supported:
                self.pageContent.warningLabel.setVisible(True)
                selected_cpu = self.projectInfo("CPU_NAME")
                self.pageContent.warningLabel.setText(self.tr("<font color='#FF0000'>Warning: the selected module, \
                    is not completely supported by the %1.</font>").arg(selected_cpu))
            else:
                self.pageContent.warningLabel.setVisible(False)
            self.pageContent.propertyTable.clear()
            self.pageContent.propertyTable.setRowCount(0)
            if configuration != "":
                configurations = self.projectInfo("CONFIGURATIONS")[configuration]
                param_list = sorted(configurations["paramlist"])
                index = 0
                for i, property in param_list:
                    if "type" in configurations[property]["informations"] and configurations[property]["informations"]["type"] == "autoenabled":
                        # Doesn't show the hidden fields
                        continue
                    try:
                        param_supported = bertos_utils.isSupported(self.project, property_id=(configuration, property))
                    except SupportedException, e:
                        self.exceptionOccurred(self.tr("Error evaluating \"%1\" for parameter %2").arg(e.support_string).arg(property))
                        param_supported = True
                    if not param_supported:
                        # Doesn't show the unsupported parameters
                        continue
                    # Set the row count to the current index + 1
                    self.pageContent.propertyTable.setRowCount(index + 1)
                    item = QTableWidgetItem(configurations[property]["brief"])
                    item.setFlags(item.flags() & ~Qt.ItemIsSelectable)
                    item.setToolTip(property)
                    item.setData(Qt.UserRole, qvariant_converter.convertString(property))
                    self.pageContent.propertyTable.setItem(index, 0, item)
                    if "type" in configurations[property]["informations"] and configurations[property]["informations"]["type"] == "boolean":
                        self.insertCheckBox(index, configurations[property]["value"])
                    elif "type" in configurations[property]["informations"] and configurations[property]["informations"]["type"] == "enum":
                        self.insertComboBox(index, configurations[property]["value"], configurations[property]["informations"]["value_list"])
                    elif "type" in configurations[property]["informations"] and configurations[property]["informations"]["type"] == "int":
                        self.insertSpinBox(index, configurations[property]["value"], configurations[property]["informations"])
                    else:
                        # Not defined type, rendered as a text field
                        self.pageContent.propertyTable.setItem(index, 1, QTableWidgetItem(configurations[property]["value"]))
                    index += 1
            if self.pageContent.propertyTable.rowCount() == 0:
                module_label = self.pageContent.moduleLabel.text()
                module_label += "\n\nNo configuration needed."
                self.pageContent.moduleLabel.setText(module_label) 
        else:
            self.pageContent.moduleLabel.setText("")
            self.pageContent.moduleLabel.setVisible(False)
            self.pageContent.propertyTable.clear()
            self.pageContent.propertyTable.setRowCount(0)

    def dependencyCheck(self, item):
        """
        Checks the dependencies of the module associated with the given item.
        """
        checked = False
        module = unicode(item.text(0))
        if item.checkState(0) == Qt.Checked:
            self.moduleSelected(module)
        else:
            self.moduleUnselected(module)
            self.removeFileDependencies(module)

    def showPropertyDescription(self):
        """
        Slot called when the property selection changes. Shows the description
        of the selected property.
        """
        self.resetPropertyDescription()
        configurations = self.currentModuleConfigurations()
        if self.currentProperty() in configurations:
            description = configurations[self.currentProperty()]["brief"]
            name = self.currentProperty()
            self.currentPropertyItem().setText(description + "\n" + name)

    def saveValue(self, index):
        """
        Slot called when the user modifies one of the configuration parameters.
        It stores the new value."""
        property = qvariant_converter.getString(self.pageContent.propertyTable.item(index, 0).data(Qt.UserRole))
        configuration = self.projectInfo("MODULES")[self.currentModule()]["configuration"]
        configurations = self.projectInfo("CONFIGURATIONS")
        if "type" not in configurations[configuration][property]["informations"] or configurations[configuration][property]["informations"]["type"] == "int":
            configurations[configuration][property]["value"] = unicode(int(self.pageContent.propertyTable.cellWidget(index, 1).value()))
        elif configurations[configuration][property]["informations"]["type"] == "enum":
            configurations[configuration][property]["value"] = unicode(self.pageContent.propertyTable.cellWidget(index, 1).currentText())
        elif configurations[configuration][property]["informations"]["type"] == "boolean":
            if self.pageContent.propertyTable.cellWidget(index, 1).isChecked():
                configurations[configuration][property]["value"] = "1"
            else:
                configurations[configuration][property]["value"] = "0"
        self.setProjectInfo("CONFIGURATIONS", configurations)
        if self.moduleItem(self.currentModule()).checkState(0) == Qt.Checked:
            self.dependencyCheck(self.moduleItem(self.currentModule()))

    ####
    
    def loadModuleData(self):
        """
        Loads the module data.
        """
        # Do not load the module data again when the Wizard is in editing mode
        # or when it's working on a preset.
        if not self.project.edit and not self.project.from_preset:
            # Load the module data every time so that if the user changed the cpu
            # the right configurations are picked up.
            try:
                self.project.loadModuleData()
            except ModuleDefineException, e:
                self.exceptionOccurred(self.tr("Error parsing line '%2' in file %1").arg(e.path).arg(e.line))
            except EnumDefineException, e:
                self.exceptionOccurred(self.tr("Error parsing line '%2' in file %1").arg(e.path).arg(e.line))
            except ConfigurationDefineException, e:
                self.exceptionOccurred(self.tr("Error parsing line '%2' in file %1").arg(e.path).arg(e.line))
    
    def fillModuleTree(self):
        """
        Fills the module tree with the module entries separated in categories.
        """
        self.pageContent.moduleTree.clear()
        modules = self.projectInfo("MODULES")
        if not modules:
            return
        categories = {}
        for module, information in modules.items():
            if information["category"] not in categories:
                categories[information["category"]] = []
            categories[information["category"]].append(module)
        for category, module_list in categories.items():
            item = QTreeWidgetItem(QStringList([category]))
            for module in module_list:
                enabled = modules[module]["enabled"]
                module_item = QTreeWidgetItem(item, QStringList([module]))
                try:
                    supported = bertos_utils.isSupported(self.project, module=module)
                except SupportedException, e:
                    self.exceptionOccurred(self.tr("Error evaluating \"%1\" for module %2").arg(e.support_string).arg(module))
                    supported = True
                if not supported:
                    module_item.setForeground(0, QBrush(QColor(Qt.red)))
                if enabled:
                    module_item.setCheckState(0, Qt.Checked)
                else:
                    module_item.setCheckState(0, Qt.Unchecked)
            self.pageContent.moduleTree.addTopLevelItem(item)
        self.pageContent.moduleTree.sortItems(0, Qt.AscendingOrder)
        self.fillPropertyTable()
            
    def insertCheckBox(self, index, value):
        """
        Inserts in the table at index a checkbox for a boolean property setted
        to value.
        """
        check_box = QCheckBox()
        self.pageContent.propertyTable.setCellWidget(index, 1, check_box)
        if value == "1":
            check_box.setChecked(True)
        else:
            check_box.setChecked(False)
        self._control_group.addControl(index, check_box)
    
    def insertComboBox(self, index, value, value_list):
        """
        Inserts in the table at index a combobox for an enum property setted
        to value.
        """
        try:
            enum = self.projectInfo("LISTS")[value_list]
            combo_box = QComboBox()
            self.pageContent.propertyTable.setCellWidget(index, 1, combo_box)
            for i, element in enumerate(enum):
                combo_box.addItem(element)
                if element == value:
                    combo_box.setCurrentIndex(i)
            self._control_group.addControl(index, combo_box)
        except KeyError:
            self.exceptionOccurred(self.tr("Define list \"%1\" not found. Check definition files.").arg(value_list))
            self.pageContent.propertyTable.setItem(index, 1, QTableWidgetItem(value))
    
    def insertSpinBox(self, index, value, informations):
        """
        Inserts in the table at index a spinbox for an int, a long or an unsigned
        long property setted to value.
        """
        # int, long or undefined type property
        spin_box = None
        if bertos_utils.isLong(informations) or bertos_utils.isUnsignedLong(informations):
            spin_box = QDoubleSpinBox()
            spin_box.setDecimals(0)
        else:
            spin_box = QSpinBox()
        self.pageContent.propertyTable.setCellWidget(index, 1, spin_box)
        minimum = -32768
        maximum = 32767
        suff = ""
        if bertos_utils.isLong(informations):
            minimum = -2147483648
            maximum = 2147483647
            suff = "L"
        elif bertos_utils.isUnsigned(informations):
            minimum = 0
            maximum = 65535
            suff = "U"
        elif bertos_utils.isUnsignedLong(informations):
            minimum = 0
            maximum = 4294967295
            suff = "UL"
        if "min" in informations:
            minimum = int(informations["min"])
        if "max" in informations:
            maximum = int(informations["max"])
        spin_box.setRange(minimum, maximum)
        spin_box.setSuffix(suff)
        spin_box.setValue(int(value.replace("L", "").replace("U", "")))
        self._control_group.addControl(index, spin_box)
        
    
    def currentModule(self):
        """
        Retuns the current module name.
        """
        current_module = self.pageContent.moduleTree.currentItem()
        # return only the child items
        if current_module and current_module.parent():
            return unicode(current_module.text(0))
        else:
            return None

    def moduleItem(self, module):
        for top_level_index in range(self.pageContent.moduleTree.topLevelItemCount()):
            top_level_item = self.pageContent.moduleTree.topLevelItem(top_level_index)
            for child_index in range(top_level_item.childCount()):
                child_item = top_level_item.child(child_index)
                if unicode(child_item.text(0)) == module:
                    return child_item
        return None
    
    def currentModuleConfigurations(self):
        """
        Returns the current module configuration.
        """
        return self.configurations(self.currentModule())
    
    def currentProperty(self):
        """
        Rerturns the current property from the property table.
        """
        return qvariant_converter.getString(self.pageContent.propertyTable.item(self.pageContent.propertyTable.currentRow(), 0).data(Qt.UserRole))
    
    def currentPropertyItem(self):
        """
        Returns the QTableWidgetItem of the current property.
        """
        return self.pageContent.propertyTable.item(self.pageContent.propertyTable.currentRow(), 0)
    
    def configurations(self, module):
        """
        Returns the configuration for the selected module.
        """
        configuration = []
        if module:
            # On linux platform it seems that the behaviour of the focus
            # changing is a bit different from the mac one. So if module is
            # None then no configurations should be returned.
            configuration = self.projectInfo("MODULES")[module]["configuration"]
        if len(configuration) > 0:
            return self.projectInfo("CONFIGURATIONS")[configuration]
        else:
            return {}
    
    def resetPropertyDescription(self):
        """
        Resets the label for each property table entry.
        """
        for index in range(self.pageContent.propertyTable.rowCount()):
            property_name = qvariant_converter.getString(self.pageContent.propertyTable.item(index, 0).data(Qt.UserRole))
            # Awful solution! Needed because if the user change the module, the selection changed...
            if property_name not in self.currentModuleConfigurations():
                break
            self.pageContent.propertyTable.item(index, 0).setText(self.currentModuleConfigurations()[property_name]['brief'])
    
    def setBold(self, item, bold):
        self.pageContent.moduleTree.blockSignals(True)
        font = item.font(0)
        font.setBold(bold)
        item.setFont(0, font)
        self.pageContent.moduleTree.blockSignals(False)

    def moduleSelected(self, selectedModule):
        """
        Resolves the selection dependencies.
        """
        try:
            qApp.setOverrideCursor(Qt.WaitCursor)
            modules = self.projectInfo("MODULES")
            modules[selectedModule]["enabled"] = True
            self.setProjectInfo("MODULES", modules)
            depends = self.projectInfo("MODULES")[selectedModule]["depends"]
            unsatisfied = []
            if self.pageContent.automaticFix.isChecked():
                unsatisfied = self.selectDependencyCheck(selectedModule)
            if len(unsatisfied) > 0:
                for module in unsatisfied:
                    modules = self.projectInfo("MODULES")
                    modules[module]["enabled"] = True
                for category in range(self.pageContent.moduleTree.topLevelItemCount()):
                    item = self.pageContent.moduleTree.topLevelItem(category)
                    for child in range(item.childCount()):
                        if unicode(item.child(child).text(0)) in unsatisfied:
                            self.setBold(item.child(child), True)
                            self.setBold(item, True)
                            item.child(child).setCheckState(0, Qt.Checked)
        finally:
            qApp.restoreOverrideCursor()
    
    def moduleUnselected(self, unselectedModule):
        """
        Resolves the unselection dependencies.
        """
        try:
            qApp.setOverrideCursor(Qt.WaitCursor)
            modules = self.projectInfo("MODULES")
            modules[unselectedModule]["enabled"] = False
            self.setProjectInfo("MODULES", modules)
            unsatisfied = []
            unsatisfied_params = []
            if self.pageContent.automaticFix.isChecked():
                unsatisfied, unsatisfied_params = self.unselectDependencyCheck(unselectedModule)
            if len(unsatisfied) > 0 or len(unsatisfied_params) > 0:
                message = []
                heading = self.tr("The module %1 is needed by").arg(unselectedModule)
                message.append(heading)
                module_list = ", ".join(unsatisfied)
                param_list = ", ".join(["%s (%s)" %(param_name, module) for module, param_name in unsatisfied_params])
                if module_list:
                    message.append(QString(module_list))
                if module_list and param_list:
                    message.append(self.tr("and by"))
                if param_list:
                    message.append(QString(param_list))
                message_str = QStringList(message).join(" ")
                message_str.append(self.tr("\n\nDo you want to automatically fix these conflicts?"))
                qApp.restoreOverrideCursor()
                choice = QMessageBox.warning(self, self.tr("Dependency error"), message_str, QMessageBox.Yes | QMessageBox.No, QMessageBox.Yes)
                qApp.setOverrideCursor(Qt.WaitCursor)
                if choice == QMessageBox.Yes:
                    for module in unsatisfied:
                        modules = self.projectInfo("MODULES")
                        modules[module]["enabled"] = False
                    for category in range(self.pageContent.moduleTree.topLevelItemCount()):
                        item = self.pageContent.moduleTree.topLevelItem(category)
                        for child in range(item.childCount()):
                            if unicode(item.child(child).text(0)) in unsatisfied:
                                item.child(child).setCheckState(0, Qt.Unchecked)
                    for module, param in unsatisfied_params:
                        configuration_file = self.projectInfo("MODULES")[module]["configuration"]
                        configurations = self.projectInfo("CONFIGURATIONS")
                        configurations[configuration_file][param]["value"] = "0"
                        self.setProjectInfo("CONFIGURATIONS", configurations)
        finally:
            qApp.restoreOverrideCursor()
    
    def selectDependencyCheck(self, module):
        """
        Returns the list of unsatisfied dependencies after a selection.
        """
        unsatisfied = set()
        modules = self.projectInfo("MODULES")
        files = self.projectInfo("FILES")
        configurations = self.projectInfo("CONFIGURATIONS").get(modules[module]["configuration"], {"paramlist": ()})
        conditional_deps = ()
        for i, param_name in configurations["paramlist"]:
            information = configurations[param_name]
            if information["informations"]["type"] == "boolean" and \
                information["value"] != "0" and \
                "conditional_deps" in information["informations"]:

                conditional_deps += information["informations"]["conditional_deps"]

        for dependency in modules[module]["depends"] + conditional_deps:
            if dependency in modules and not modules[dependency]["enabled"]:
                unsatisfied |= set([dependency])
                if dependency not in unsatisfied:
                    unsatisfied |= self.selectDependencyCheck(dependency)
            if dependency not in modules:
                if dependency in files:
                    files[dependency] += 1
                else:
                    files[dependency] = 1
        self.setProjectInfo("FILES", files)
        return unsatisfied
    
    def unselectDependencyCheck(self, dependency):
        """
        Returns the list of unsatisfied dependencies after an unselection.
        """
        unsatisfied = set()
        unsatisfied_params = set()
        modules = self.projectInfo("MODULES")
        for module, informations in modules.items():
            configurations = self.projectInfo("CONFIGURATIONS").get(informations["configuration"], {"paramlist": ()})
            conditional_deps = {}
            for i, param_name in configurations["paramlist"]:
                information = configurations[param_name]
                if information["informations"]["type"] == "boolean" and information["value"] != "0" and "conditional_deps" in information["informations"]:
                    for dep in information["informations"]["conditional_deps"]:
                        if not dep in conditional_deps:
                            conditional_deps[dep] = []
                        conditional_deps[dep].append((module, param_name))
            if dependency in informations["depends"] and informations["enabled"]:
                unsatisfied |= set([module])
                if dependency not in unsatisfied:
                    tmp = self.unselectDependencyCheck(module)
                    unsatisfied |= tmp[0]
                    unsatisfied_params |= tmp[1]
            if dependency in conditional_deps:
                unsatisfied_params |= set(conditional_deps[dependency])
        return unsatisfied, unsatisfied_params
    
    def removeFileDependencies(self, module):
        """
        Removes the files dependencies of the given module.
        """
        modules = self.projectInfo("MODULES")
        files = self.projectInfo("FILES")
        dependencies = modules[module]["depends"]
        for dependency in dependencies:
            if dependency in files:
                files[dependency] -= 1
                if files[dependency] == 0:
                    del files[dependency]
        self.setProjectInfo("FILES", files)

class QControlGroup(QObject):
    """
    Simple class that permit to connect different signals of different widgets
    with a slot that emit a signal. Permits to group widget and to understand which of
    them has sent the signal.
    """
    
    def __init__(self):
        QObject.__init__(self)
        self._controls = {}
    
    def addControl(self, id, control):
        """
        Add a control.
        """
        self._controls[id] = control
        if type(control) == QCheckBox:
            self.connect(control, SIGNAL("stateChanged(int)"), lambda: self.stateChanged(id))
        elif type(control) == QSpinBox:
            self.connect(control, SIGNAL("valueChanged(int)"), lambda: self.stateChanged(id))
        elif type(control) == QComboBox:
            self.connect(control, SIGNAL("currentIndexChanged(int)"), lambda: self.stateChanged(id))
        elif type(control) == QDoubleSpinBox:
            self.connect(control, SIGNAL("valueChanged(double)"), lambda: self.stateChanged(id))
    
    def clear(self):
        """
        Remove all the controls.
        """
        self._controls = {}
    
    def stateChanged(self, id):
        """
        Slot called when the value of one of the stored widget changes. It emits
        another signal.
        """
        self.emit(SIGNAL("stateChanged"), id)
