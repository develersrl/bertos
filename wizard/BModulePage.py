#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import os

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
        self.connect(self.pageContent.moduleTree, SIGNAL("itemChanged(QTreeWidgetItem*, int)"), self.dependencyCheck)
        self.connect(self.pageContent.propertyTable, SIGNAL("itemSelectionChanged()"), self.showPropertyDescription)

    def reloadData(self):
        """
        Overload of the BWizardPage reloadData method.
        """
        QApplication.instance().setOverrideCursor(Qt.WaitCursor)
        self.setupUi()
        self.loadModuleData()
        self.fillModuleTree()
        QApplication.instance().restoreOverrideCursor()
    
    ####
    
    ## Slots ##

    def fillPropertyTable(self):
        """
        Slot called when the user selects a module from the module tree.
        Fills the property table using the configuration parameters defined in
        the source tree.
        """
        module = self.currentModule()
        if module is not None:
            try:
                supported = bertos_utils.isSupported(self.project(), module=module)
            except SupportedException, e:
                self.exceptionOccurred(self.tr("Error evaluating \"%1\" for module %2").arg(e.support_string).arg(selectedModule))
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
                        param_supported = bertos_utils.isSupported(self.project(), property_id=(configuration, property))
                    except SupportedException, e:
                        self.exceptionOccurred(self.tr("Error evaluating \"%1\" for module %2").arg(e.support_string).arg(selectedModule))
                        param_supported = True
                    if not param_supported:
                        # Doesn't show the unsupported parameters
                        continue
                    # Set the row count to the current index + 1
                    self.pageContent.propertyTable.setRowCount(index + 1)
                    item = QTableWidgetItem(configurations[property]["brief"])
                    item.setData(Qt.UserRole, qvariant_converter.convertString(property))
                    self.pageContent.propertyTable.setItem(index, 0, item)
                    if "type" in configurations[property]["informations"].keys() and configurations[property]["informations"]["type"] == "boolean":
                        self.insertCheckBox(index, configurations[property]["value"])
                    elif "type" in configurations[property]["informations"].keys() and configurations[property]["informations"]["type"] == "enum":
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
        if self.currentProperty() in configurations.keys():
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
        if "type" not in configurations[configuration][property]["informations"].keys() or configurations[configuration][property]["informations"]["type"] == "int":
            configurations[configuration][property]["value"] = str(int(self.pageContent.propertyTable.cellWidget(index, 1).value()))
        elif configurations[configuration][property]["informations"]["type"] == "enum":
            configurations[configuration][property]["value"] = unicode(self.pageContent.propertyTable.cellWidget(index, 1).currentText())
        elif configurations[configuration][property]["informations"]["type"] == "boolean":
            if self.pageContent.propertyTable.cellWidget(index, 1).isChecked():
                configurations[configuration][property]["value"] = "1"
            else:
                configurations[configuration][property]["value"] = "0"
        self.setProjectInfo("CONFIGURATIONS", configurations)

    ####
    
    def loadModuleData(self):
        """
        Loads the module data.
        """
        # Load the module data only if it isn't already loaded
        if self.projectInfo("MODULES") == None \
                and self.projectInfo("LISTS") == None \
                and self.projectInfo("CONFIGURATIONS") == None:
            try:
                bertos_utils.loadModuleData(self.project())
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
        modules = self.projectInfo("MODULES")
        if modules is None:
            return
        categories = {}
        for module, information in modules.items():
            if information["category"] not in categories.keys():
                categories[information["category"]] = []
            categories[information["category"]].append(module)
        for category, module_list in categories.items():
            item = QTreeWidgetItem(QStringList([category]))
            for module in module_list:
                enabled = modules[module]["enabled"]
                module_item = QTreeWidgetItem(item, QStringList([module]))
                try:
                    supported = bertos_utils.isSupported(self.project(), module=module)
                except SupportedException, e:
                    self.exceptionOccurred(self.tr("Error evaluating \"%1\" for module %2").arg(e.support_string).arg(selectedModule))
                    supported = True
                if not supported:
                    module_item.setForeground(0, QBrush(QColor(Qt.red)))
                if enabled:
                    module_item.setCheckState(0, Qt.Checked)
                else:
                    module_item.setCheckState(0, Qt.Unchecked)
            self.pageContent.moduleTree.addTopLevelItem(item)
        self.pageContent.moduleTree.sortItems(0, Qt.AscendingOrder)
            
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
        if "min" in informations.keys():
            minimum = int(informations["min"])
        if "max" in informations.keys():
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
        if current_module is not None and current_module.parent() is not None:
            return unicode(current_module.text(0))
        else:
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
            if property_name not in self.currentModuleConfigurations().keys():
                break
            self.pageContent.propertyTable.item(index, 0).setText(self.currentModuleConfigurations()[property_name]['brief'])
    
    def moduleSelected(self, selectedModule):
        """
        Resolves the selection dependencies.
        """
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
                        item.child(child).setCheckState(0, Qt.Checked)
    
    def moduleUnselected(self, unselectedModule):
        """
        Resolves the unselection dependencies.
        """
        modules = self.projectInfo("MODULES")
        modules[unselectedModule]["enabled"] = False
        self.setProjectInfo("MODULES", modules)
        unsatisfied = []
        if self.pageContent.automaticFix.isChecked():
            unsatisfied = self.unselectDependencyCheck(unselectedModule)
        if len(unsatisfied) > 0:
            message = self.tr("The module %1 is needed by the following modules:\n%2.\n\nDo you want to remove these modules too?")
            message = message.arg(unselectedModule).arg(", ".join(unsatisfied))
            choice = QMessageBox.warning(self, self.tr("Dependency error"), message, QMessageBox.Yes | QMessageBox.No, QMessageBox.Yes)
            if choice == QMessageBox.Yes:
                for module in unsatisfied:
                    modules = self.projectInfo("MODULES")
                    modules[module]["enabled"] = False
                for category in range(self.pageContent.moduleTree.topLevelItemCount()):
                    item = self.pageContent.moduleTree.topLevelItem(category)
                    for child in range(item.childCount()):
                        if unicode(item.child(child).text(0)) in unsatisfied:
                            item.child(child).setCheckState(0, Qt.Unchecked)
    
    def selectDependencyCheck(self, module):
        """
        Returns the list of unsatisfied dependencies after a selection.
        """
        unsatisfied = set()
        modules = self.projectInfo("MODULES")
        files = self.projectInfo("FILES")
        for dependency in modules[module]["depends"]:
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
        modules = self.projectInfo("MODULES")
        for module, informations in modules.items():
            if dependency in informations["depends"] and informations["enabled"]:
                unsatisfied |= set([module])
                if dependency not in unsatisfied:
                    unsatisfied |= self.unselectDependencyCheck(module)
        return unsatisfied
    
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