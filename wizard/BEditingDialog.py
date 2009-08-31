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

import sys
import os

from PyQt4.QtCore import *
from PyQt4.QtGui import *

from bertos_utils import loadBertosProject, bertosVersion, getToolchainName, createBertosProject
from BToolchainPage import BToolchainPage
from BVersionPage import BVersionPage
import qvariant_converter
import BModulePage
import bertos_utils

class BEditingDialog(QDialog):

    def __init__(self, parent=None):
        QDialog.__init__(self, parent)
        self.setupUi()
        self.connectSignals()
        self.module_page.reloadData()
    
    def setupUi(self):
        layout = QVBoxLayout()
        self.module_page = BModulePage.BModulePage()
        layout.addWidget(self.module_page)
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

    def changeToolchain(self):
        dialog = QDialog()
        layout = QVBoxLayout()
        toolchain_page = BToolchainPage()
        toolchain_page.reloadData()
        layout.addWidget(toolchain_page)
        button_layout = QHBoxLayout()
        button_layout.addStretch()
        cancel_button = QPushButton(self.tr("Cancel")) 
        button_layout.addWidget(cancel_button)
        ok_button = QPushButton(self.tr("Ok"))
        button_layout.addWidget(ok_button)
        dialog.connect(cancel_button, SIGNAL("clicked()"), dialog.reject)
        layout.addLayout(button_layout)
        dialog.setLayout(layout)
        dialog.connect(ok_button, SIGNAL("clicked()"), dialog.accept)
        if dialog.exec_():
            toolchain = qvariant_converter.getStringDict(toolchain_page.currentItem().data(Qt.UserRole))
            toolchain_page.setProjectInfo("TOOLCHAIN", toolchain)
    
    def changeBertosVersion(self):
        dialog = QDialog()
        layout = QVBoxLayout()
        version_page = BVersionPage()
        version_page.reloadData()
        layout.addWidget(version_page)
        button_layout = QHBoxLayout()
        button_layout.addStretch()
        cancel_button = QPushButton(self.tr("Cancel")) 
        button_layout.addWidget(cancel_button)
        ok_button = QPushButton(self.tr("Ok"))
        button_layout.addWidget(ok_button)
        dialog.connect(cancel_button, SIGNAL("clicked()"), dialog.reject)
        layout.addLayout(button_layout)
        dialog.setLayout(layout)
        dialog.connect(ok_button, SIGNAL("clicked()"), dialog.accept)
        current_version = version_page.projectInfo("SOURCES_PATH")
        if dialog.exec_():
            version = qvariant_converter.getString(version_page.currentItem().data(Qt.UserRole))
            if version != current_version:
                if QMessageBox.question(
                    version_page,
                    self.tr("BeRTOS version update"),
                    self.tr("Changing the BeRTOS version will destroy all the modification done on the BeRTOS sources"),
                    QMessageBox.Ok | QMessageBox.Cancel
                ) == QMessageBox.Ok:
                    version_page.setProjectInfo("SOURCES_PATH", version)
                    version_page.setProjectInfo("OLD_SOURCES_PATH", current_version)
                    project = version_page.project()
                    modules, lists, configurations, files = project.info("MODULES"), project.info("LISTS"), project.info("CONFIGURATIONS"), project.info("FILES")
                    bertos_utils.loadSourceTree(version_page.project())
                    bertos_utils.loadModuleData(version_page.project(), True)
                    self.module_page.fillModuleTree()

    def apply(self):
        createBertosProject(self.module_page.project(), edit=True)
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
        return self.module_page.projectInfo("SOURCES_PATH")
    
    def setCurrentVersion(self, version):
        self.module_page.setProjectInfo("SOURCES_PATH", version)



def main():
    if len(sys.argv) > 1:
        project_file = sys.argv[1]
    else:
        print "Invalid usage: use <command> project_file"
        sys.exit()
    app = QApplication([])
    app.project = loadBertosProject(project_file)
    app.settings = QSettings("Develer", "Bertos Configurator")
    dialog = BEditingDialog()
    dialog.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()
