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

from bertos_utils import loadBertosProject, bertosVersion, getToolchainName
from toolchain_validation import validateToolchain
import BModulePage

class BEditingDialog(QDialog):

    def __init__(self, parent=None):
        QDialog.__init__(self, parent)
        self.setupUi()
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
        self.setupToolchainMenu()
        self.menu.addMenu(self.toolchain_menu)
        self.setupVersionMenu()
        self.menu.addMenu(self.version_menu)

    def setupToolchainMenu(self):
        self.toolchain_menu = QMenu(self.tr("select toolchain"))
        action_group = QActionGroup(self.toolchain_menu)
        for toolchain in sorted(self.toolchains()):
            info = validateToolchain(toolchain)
            if info[0]:
                name = getToolchainName(info[1])
            else:
                name = toolchain
            action = self.toolchain_menu.addAction(name)
            action_group.addAction(action)
            action.setCheckable(True)
            action.setChecked(True if toolchain == self.currentToolchain()["path"] else False)

    def setupVersionMenu(self):
        self.version_menu = QMenu(self.tr("select BeRTOS version"))
        action_group = QActionGroup(self.version_menu)
        for version in sorted([bertosVersion(v) for v in self.versions()]):
            action = self.version_menu.addAction(version)
            action_group.addAction(action)
            action.setCheckable(True)
            action.setChecked(True if unicode(action.text()) == self.currentVersion() else False)

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
