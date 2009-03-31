#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id:$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import os

from PyQt4.QtGui import *
from BWizardPage import *
import bertos_utils

from DefineException import *
from const import *

class BModulePage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/module_select.ui")
        self.setTitle(self.tr("Configure the BeRTOS modules"))
        self._control_group = QControlGroup()
        self._connectSignals()
    
    def reloadData(self):
        QApplication.instance().setOverrideCursor(Qt.WaitCursor)
        self._setupUi()
        self._loadModuleData()
        self._fillModuleTree()
        QApplication.instance().restoreOverrideCursor()
    
    def _setupButtonGroup(self):
        self._button_group = QButtonGroup()
        self._button_group.setExclusive(False)
        self.connect(self._button_group, SIGNAL("buttonClicked(int)"), self._moduleSelectionChanged)
    
    def _loadModuleData(self):
        ## Load the module data only if it isn't already loaded
        if self._projectInfoRetrieve("MODULES") == None \
                and self._projectInfoRetrieve("LISTS") == None \
                and self._projectInfoRetrieve("CONFIGURATIONS") == None:
            try:
                bertos_utils.loadModuleData(self._project())
            except ModuleDefineException, e:
                self._exceptionOccurred(self.tr("Error parsing line '%2' in file %1").arg(e.path).arg(e.line))
            except EnumDefineException, e:
                self._exceptionOccurred(self.tr("Error parsing line '%2' in file %1").arg(e.path).arg(e.line))
            except ConfigurationDefineException, e:
                self._exceptionOccurred(self.tr("Error parsing line '%2' in file %1").arg(e.path).arg(e.line))
    
    def _fillModuleTree(self):
        modules = self._projectInfoRetrieve("MODULES")
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
                if enabled:
                    module_item.setCheckState(0, Qt.Checked)
                else:
                    module_item.setCheckState(0, Qt.Unchecked)
            self.pageContent.moduleTree.addTopLevelItem(item)
        self.pageContent.moduleTree.sortItems(0, Qt.AscendingOrder)
        
    
    def _fillPropertyTable(self):
        module = self._currentModule()
        if module is not None:
            self._control_group.clear()
            configuration = self._projectInfoRetrieve("MODULES")[module]["configuration"]
            module_description = self._projectInfoRetrieve("MODULES")[module]["description"]
            self.pageContent.moduleLabel.setText(module_description)
            self.pageContent.moduleLabel.setVisible(True)
            self.pageContent.propertyTable.clear()
            self.pageContent.propertyTable.setRowCount(0)
            if configuration != "":
                configurations = self._projectInfoRetrieve("CONFIGURATIONS")[configuration]
                param_list = sorted(configurations["paramlist"])
                index = 0
                for i, property in param_list:
                    if "type" in configurations[property]["informations"] and configurations[property]["informations"]["type"] == "autoenabled":
                        ## Doesn't show the hidden fields
                        pass
                    else:
                        ## Set the row count to the current index + 1
                        self.pageContent.propertyTable.setRowCount(index + 1)
                        item = QTableWidgetItem(configurations[property]["brief"])
                        item.setData(Qt.UserRole, qvariant_converter.convertString(property))
                        self.pageContent.propertyTable.setItem(index, 0, item)
                        if "type" in configurations[property]["informations"].keys() and configurations[property]["informations"]["type"] == "boolean":
                            self._insertCheckBox(index, configurations[property]["value"])
                        elif "type" in configurations[property]["informations"].keys() and configurations[property]["informations"]["type"] == "enum":
                            self._insertComboBox(index, configurations[property]["value"], configurations[property]["informations"]["value_list"])
                        elif "type" in configurations[property]["informations"] and configurations[property]["informations"]["type"] == "int":
                            self._insertSpinBox(index, configurations[property]["value"], configurations[property]["informations"])
                        else:
                            # Not defined type, rendered as a text field
                            self.pageContent.propertyTable.setItem(index, 1, QTableWidgetItem(configurations[property]["value"]))
                        index += 1
            if self.pageContent.propertyTable.rowCount() == 0:
                module_label = self.pageContent.moduleLabel.text()
                module_label += "\n\nNo configuration needed."
                self.pageContent.moduleLabel.setText(module_label)
    
    def _insertCheckBox(self, index, value):
        ## boolean property
        check_box = QCheckBox()
        self.pageContent.propertyTable.setCellWidget(index, 1, check_box)
        if value == "1":
            check_box.setChecked(True)
        else:
            check_box.setChecked(False)
        self._control_group.addControl(index, check_box)
    
    def _insertComboBox(self, index, value, value_list):
        ## enum property
        try:
            enum = self._projectInfoRetrieve("LISTS")[value_list]
            combo_box = QComboBox()
            self.pageContent.propertyTable.setCellWidget(index, 1, combo_box)
            for i, element in enumerate(enum):
                combo_box.addItem(element)
                if element == value:
                    combo_box.setCurrentIndex(i)
            self._control_group.addControl(index, combo_box)
        except KeyError:
            self._exceptionOccurred(self.tr("Define list \"%1\" not found. Check definition files.").arg(value_list))
            self.pageContent.propertyTable.setItem(index, 1, QTableWidgetItem(value))
    
    def _insertSpinBox(self, index, value, informations):
        ## int, long or undefined type property
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
        
    
    def _currentModule(self):
        current_module = self.pageContent.moduleTree.currentItem()
        # return only the child items
        if current_module is not None and current_module.parent() is not None:
            return unicode(current_module.text(0))
        else:
            return None
    
    def _currentModuleConfigurations(self):
        return self._configurations(self._currentModule())
    
    def _currentProperty(self):
        return qvariant_converter.getString(self.pageContent.propertyTable.item(self.pageContent.propertyTable.currentRow(), 0).data(Qt.UserRole))
    
    def _currentPropertyItem(self):
        return self.pageContent.propertyTable.item(self.pageContent.propertyTable.currentRow(), 0)
    
    def _configurations(self, module):
        configuration = self._projectInfoRetrieve("MODULES")[module]["configuration"]
        if len(configuration) > 0:
            return self._projectInfoRetrieve("CONFIGURATIONS")[configuration]
        else:
            return {}
    
    def _resetPropertyDescription(self):
        for index in range(self.pageContent.propertyTable.rowCount()):
            property_name = qvariant_converter.getString(self.pageContent.propertyTable.item(index, 0).data(Qt.UserRole))
            # Awful solution! Needed because if the user change the module, the selection changed...
            if property_name not in self._currentModuleConfigurations().keys():
                break
            self.pageContent.propertyTable.item(index, 0).setText(self._currentModuleConfigurations()[property_name]['brief'])
    
    def _showPropertyDescription(self):
        self._resetPropertyDescription()
        configurations = self._currentModuleConfigurations()
        if self._currentProperty() in configurations.keys():
            description = configurations[self._currentProperty()]["brief"]
            name = self._currentProperty()
            self._currentPropertyItem().setText(description + "\n" + name)
    
    def _setupUi(self):
        self.pageContent.moduleTree.clear()
        self.pageContent.moduleTree.setHeaderHidden(True)
        self.pageContent.propertyTable.horizontalHeader().setResizeMode(QHeaderView.Stretch)
        self.pageContent.propertyTable.horizontalHeader().setVisible(False)
        self.pageContent.propertyTable.verticalHeader().setResizeMode(QHeaderView.ResizeToContents)
        self.pageContent.propertyTable.verticalHeader().setVisible(False)
        self.pageContent.propertyTable.setColumnCount(2)
        self.pageContent.propertyTable.setRowCount(0)
        self.pageContent.moduleLabel.setVisible(False)
    
    def _connectSignals(self):
        self.connect(self.pageContent.moduleTree, SIGNAL("itemPressed(QTreeWidgetItem*, int)"), self._fillPropertyTable)
        self.connect(self.pageContent.moduleTree, SIGNAL("itemChanged(QTreeWidgetItem*, int)"), self._dependencyCheck)
        self.connect(self.pageContent.propertyTable, SIGNAL("itemSelectionChanged()"), self._showPropertyDescription)
        self.connect(self._control_group, SIGNAL("stateChanged"), self._saveValue)
    
    def _saveValue(self, index):
        property = qvariant_converter.getString(self.pageContent.propertyTable.item(index, 0).data(Qt.UserRole))
        configuration = self._projectInfoRetrieve("MODULES")[self._currentModule()]["configuration"]
        configurations = self._projectInfoRetrieve("CONFIGURATIONS")
        if "type" not in configurations[configuration][property]["informations"].keys() or configurations[configuration][property]["informations"]["type"] == "int":
            configurations[configuration][property]["value"] = str(int(self.pageContent.propertyTable.cellWidget(index, 1).value()))
        elif configurations[configuration][property]["informations"]["type"] == "enum":
            configurations[configuration][property]["value"] = unicode(self.pageContent.propertyTable.cellWidget(index, 1).currentText())
        elif configurations[configuration][property]["informations"]["type"] == "boolean":
            if self.pageContent.propertyTable.cellWidget(index, 1).isChecked():
                configurations[configuration][property]["value"] = "1"
            else:
                configurations[configuration][property]["value"] = "0"
        self._projectInfoStore("CONFIGURATIONS", configurations)
    
    def _moduleSelectionChanged(self, index):
        module = unicode(self.pageContent.moduleTable.item(index, 1).text())
        if self._button_group.button(index).isChecked():
            self._moduleSelected(module)
        else:
            self._moduleUnselected(module)
    
    def _dependencyCheck(self, item):
        checked = False
        module = unicode(item.text(0))
        if item.checkState(0) == Qt.Checked:
            self._moduleSelected(module)
        else:
            self._moduleUnselected(module)
            self.removeFileDependencies(module)
    
    def _moduleSelected(self, selectedModule):
        modules = self._projectInfoRetrieve("MODULES")
        modules[selectedModule]["enabled"] = True
        self._projectInfoStore("MODULES", modules)
        depends = self._projectInfoRetrieve("MODULES")[selectedModule]["depends"]
        unsatisfied = []
        if self.pageContent.automaticFix.isChecked():
            unsatisfied = self.selectDependencyCheck(selectedModule)
        if len(unsatisfied) > 0:
            for module in unsatisfied:
                modules = self._projectInfoRetrieve("MODULES")
                modules[module]["enabled"] = True
            for category in range(self.pageContent.moduleTree.topLevelItemCount()):
                item = self.pageContent.moduleTree.topLevelItem(category)
                for child in range(item.childCount()):
                    if unicode(item.child(child).text(0)) in unsatisfied:
                        item.child(child).setCheckState(0, Qt.Checked)
    
    def _moduleUnselected(self, unselectedModule):
        modules = self._projectInfoRetrieve("MODULES")
        modules[unselectedModule]["enabled"] = False
        self._projectInfoStore("MODULES", modules)
        unsatisfied = []
        if self.pageContent.automaticFix.isChecked():
            unsatisfied = self.unselectDependencyCheck(unselectedModule)
        if len(unsatisfied) > 0:
            message = self.tr("The module %1 is needed by the following modules:\n%2.\n\nDo you want to remove these modules too?")
            message = message.arg(unselectedModule).arg(", ".join(unsatisfied))
            choice = QMessageBox.warning(self, self.tr("Dependency error"), message, QMessageBox.Yes | QMessageBox.No, QMessageBox.Yes)
            if choice == QMessageBox.Yes:
                for module in unsatisfied:
                    modules = self._projectInfoRetrieve("MODULES")
                    modules[module]["enabled"] = False
                for category in range(self.pageContent.moduleTree.topLevelItemCount()):
                    item = self.pageContent.moduleTree.topLevelItem(category)
                    for child in range(item.childCount()):
                        if unicode(item.child(child).text(0)) in unsatisfied:
                            item.child(child).setCheckState(0, Qt.Unchecked)
    
    def selectDependencyCheck(self, module):
        unsatisfied = set()
        modules = self._projectInfoRetrieve("MODULES")
        files = self._projectInfoRetrieve("FILES")
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
        self._projectInfoStore("FILES", files)
        return unsatisfied
    
    def unselectDependencyCheck(self, dependency):
        unsatisfied = set()
        modules = self._projectInfoRetrieve("MODULES")
        for module, informations in modules.items():
            if dependency in informations["depends"] and informations["enabled"]:
                unsatisfied |= set([module])
                if dependency not in unsatisfied:
                    unsatisfied |= self.unselectDependencyCheck(module)
        return unsatisfied
    
    def removeFileDependencies(self, module):
        modules = self._projectInfoRetrieve("MODULES")
        files = self._projectInfoRetrieve("FILES")
        dependencies = modules[module]["depends"]
        for dependency in dependencies:
            if dependency in files:
                files[dependency] -= 1
                if files[dependency] == 0:
                    del files[dependency]
        self._projectInfoStore("FILES", files)

class QControlGroup(QObject):
    def __init__(self):
        QObject.__init__(self)
        self._controls = {}
    
    def addControl(self, id, control):
        self._controls[id] = control
        if type(control) == QCheckBox:
            self.connect(control, SIGNAL("stateChanged(int)"), lambda: self._stateChanged(id))
        elif type(control) == QSpinBox:
            self.connect(control, SIGNAL("valueChanged(int)"), lambda: self._stateChanged(id))
        elif type(control) == QComboBox:
            self.connect(control, SIGNAL("currentIndexChanged(int)"), lambda: self._stateChanged(id))
        elif type(control) == QDoubleSpinBox:
            self.connect(control, SIGNAL("valueChanged(double)"), lambda: self._stateChanged(id))
    
    def clear(self):
        self._controls = {}
    
    def _stateChanged(self, id):
        self.emit(SIGNAL("stateChanged"), id)