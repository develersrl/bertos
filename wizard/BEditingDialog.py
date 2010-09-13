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

import sys
import os

from PyQt4.QtCore import *
from PyQt4.QtGui import *

from bertos_utils import bertosVersion, getToolchainName
from BToolchainPage import BToolchainPage
from BVersionPage import BVersionPage

from BProject import BProject

import qvariant_converter
from BModulePage import BModulePage
import bertos_utils

class BEditingDialog(QDialog):

    def __init__(self, parent=None):
        QDialog.__init__(self, parent)
        self.setupUi()
        self.connectSignals()
        self.module_page.reloadData()
	self.setFrequency()
    
    def setupUi(self):
        layout = QVBoxLayout()
        self.module_page = BModulePage()
        layout.addWidget(self.module_page)
        frequency_layout = QHBoxLayout()
        frequency_layout.addWidget(QLabel(self.tr("CPU frequency")))
        self.cpu_frequency_spinbox = QDoubleSpinBox()
        self.cpu_frequency_spinbox.setSuffix("Hz")
        self.cpu_frequency_spinbox.setRange(1, 1000000000)
        self.cpu_frequency_spinbox.setSingleStep(1000)
        self.cpu_frequency_spinbox.setDecimals(0)
        frequency_layout.addWidget(self.cpu_frequency_spinbox)
        frequency_layout.addStretch()
        layout.addLayout(frequency_layout)
        button_layout = QHBoxLayout()
        self.advanced_button = QToolButton()
        self.setupMenu()
        self.advanced_button.setMenu(self.menu)
        self.advanced_button.setPopupMode(QToolButton.InstantPopup)
        self.advanced_button.setText(self.tr("Advanced"))
        button_layout.addWidget(self.advanced_button)
        button_layout.addStretch()
        self.cancel_button = QPushButton(self.tr("Cancel"))
        button_layout.addWidget(self.cancel_button)
        self.apply_button = QPushButton(self.tr("Apply"))
        button_layout.addWidget(self.apply_button)
        layout.addLayout(button_layout)
        self.setLayout(layout)
        self.setWindowTitle(self.tr("Edit \"%1\" project").arg(os.path.basename(self.module_page.projectInfo("PROJECT_PATH"))))
	self.setWindowIcon(QIcon(":/images/appicon.png"))

    def setupMenu(self):
        self.menu = QMenu(self.tr("Advanced options"))
        self.change_toolchain = QAction(self.tr("Change toolchain"), self)
        self.change_bertos_version = QAction(self.tr("Change BeRTOS version"), self)
        self.menu.addAction(self.change_toolchain)
        self.menu.addAction(self.change_bertos_version)

    def connectSignals(self):
        self.connect(self.change_toolchain, SIGNAL("triggered(bool)"), self.changeToolchain)
        self.connect(self.change_bertos_version, SIGNAL("triggered(bool)"), self.changeBertosVersion)
        self.connect(self.apply_button, SIGNAL("clicked()"), self.apply)
        self.connect(self.cancel_button, SIGNAL("clicked()"), self.reject)
	self.connect(self.cpu_frequency_spinbox, SIGNAL("valueChanged(double)"), self.frequencyChanged)
    
    def setFrequency(self):
	frequency = long(self.module_page.projectInfo("SELECTED_FREQ"))
	self.cpu_frequency_spinbox.setValue(frequency)

    def frequencyChanged(self, frequency):
	frequency = unicode(long(frequency))
	self.module_page.setProjectInfo("SELECTED_FREQ", frequency)

    def changeToolchain(self):
        dialog = BToolchainDialog()
        if dialog.exec_():
            toolchain = qvariant_converter.getStringDict(dialog.toolchain_page.currentItem().data(Qt.UserRole))
            dialog.toolchain_page.setProjectInfo("TOOLCHAIN", toolchain)
    
    def changeBertosVersion(self):
	current_version = self.module_page.projectInfo("BERTOS_PATH")
        dialog = BVersionDialog()
        if dialog.exec_():
            version = qvariant_converter.getString(dialog.version_page.currentItem().data(Qt.UserRole))
            if QMessageBox.question(
                dialog.version_page,
                self.tr("BeRTOS version update"),
                self.tr("Changing the BeRTOS version will destroy all the modification done on the BeRTOS sources"),
                QMessageBox.Ok | QMessageBox.Cancel
            ) == QMessageBox.Ok:
                try:
                    qApp.setOverrideCursor(QCursor(Qt.WaitCursor))
                    dialog.version_page.setProjectInfo("BERTOS_PATH", version)
                    dialog.version_page.setProjectInfo("OLD_BERTOS_PATH", current_version)
                    enabled_modules = bertos_utils.enabledModules(dialog.version_page.project)
                    old_configuration = dialog.version_page.projectInfo("CONFIGURATIONS")
                    dialog.version_page.project.loadSourceTree()
                    QApplication.instance().project.reloadCpuInfo()
                    QApplication.instance().project.loadModuleData()
                    new_configuration = dialog.version_page.projectInfo("CONFIGURATIONS")
                    merged_configuration = {}
                    for conf in new_configuration:
                        if conf in old_configuration:
                            configuration = bertos_utils.updateConfigurationValues(new_configuration[conf], old_configuration[conf])
                        else:
                            configuration = new_configuration[conf]
                        merged_configuration[conf] = configuration
                    dialog.version_page.setProjectInfo("CONFIGURATIONS", merged_configuration)
                    dialog.version_page.project.setEnabledModules(enabled_modules)
                    self.module_page.fillModuleTree()
                finally:
                    qApp.restoreOverrideCursor()
            else:
                # Rollback version to the previous selected one.
                dialog.version_page.setProjectInfo("BERTOS_PATH", current_version)

    def apply(self):
        try:
            qApp.setOverrideCursor(QCursor(Qt.WaitCursor))
            QApplication.instance().project.createBertosProject()
        finally:
            qApp.restoreOverrideCursor()
        self.accept()

    def toolchains(self):
        return self.module_page.toolchains()
    
    def currentToolchain(self):
        return self.module_page.projectInfo("TOOLCHAIN")
    
    def setCurrentToolchain(self, toolchain):
        self.module_page.setProjectInfo("TOOLCHAIN", toolchain)

    def versions(self):
        return self.module_page.versions()

    def currentVersion(self):
        return self.module_page.projectInfo("BERTOS_PATH")
    
    def setCurrentVersion(self, version):
        self.module_page.setProjectInfo("BERTOS_PATH", version)

class BToolchainDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)
	self.setWindowIcon(QIcon(":/images/appicon.png"))
        layout = QVBoxLayout()
        toolchain_page = BToolchainPage()
	current_toolchain = toolchain_page.projectInfo("TOOLCHAIN")
        toolchain_page.reloadData()
	# TODO: to be moved in BToolchainPage
	for toolchain_row in range(toolchain_page.pageContent.toolchainList.count()):
            toolchain = qvariant_converter.getStringDict(toolchain_page.pageContent.toolchainList.item(toolchain_row).data(Qt.UserRole))
	    if current_toolchain and toolchain["path"] == current_toolchain["path"]:
                toolchain_page.pageContent.toolchainList.setCurrentRow(toolchain_row)
		toolchain_page.selectionChanged()
		break
	self.toolchain_page = toolchain_page
        layout.addWidget(toolchain_page)
        button_layout = QHBoxLayout()
        button_layout.addStretch()
        cancel_button = QPushButton(self.tr("Cancel")) 
        button_layout.addWidget(cancel_button)
        ok_button = QPushButton(self.tr("Ok"))
        button_layout.addWidget(ok_button)
        self.connect(cancel_button, SIGNAL("clicked()"), self.reject)
        layout.addLayout(button_layout)
        self.setLayout(layout)
        self.connect(ok_button, SIGNAL("clicked()"), self.accept)
        self.setWindowTitle(self.tr("Change toolchain"))

class BVersionDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)
	self.setWindowIcon(QIcon(":/images/appicon.png"))
        layout = QVBoxLayout()
        version_page = BVersionPage(edit=True)
        version_page.reloadData()
	self.version_page = version_page
        layout.addWidget(version_page)
        button_layout = QHBoxLayout()
        button_layout.addStretch()
        cancel_button = QPushButton(self.tr("Cancel")) 
        button_layout.addWidget(cancel_button)
        ok_button = QPushButton(self.tr("Ok"))
        button_layout.addWidget(ok_button)
        self.connect(cancel_button, SIGNAL("clicked()"), self.reject)
        layout.addLayout(button_layout)
        self.setLayout(layout)
        self.connect(ok_button, SIGNAL("clicked()"), self.accept)
        current_version = version_page.projectInfo("BERTOS_PATH")
        self.setWindowTitle(self.tr("Change BeRTOS version"))


def main():
    if len(sys.argv) > 1:
        project_file = sys.argv[1]
    else:
        print "Invalid usage: use <command> project_file"
        sys.exit()
    app = QApplication([])
    app.project = BProject(project_file)
    app.settings = QSettings("Develer", "Bertos Configurator")
    dialog = BEditingDialog()
    dialog.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()
