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

from PyQt4.QtGui import *

from BWizardPage import *
import bertos_utils

from const import *

class BFinalPage(BWizardPage):
    """
    Last page of the wizard. It creates the project and show a success message.
    """
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/final_page.ui")
        self.setTitle(self.tr("Project created successfully!"))
    
    ## Overloaded BWizardPage methods ##
        
    def reloadData(self, previous_id=None):
        self.setVisible(False)
        """
        Overload of the BWizardPage reloadData method.
        """
        try:
            QApplication.instance().setOverrideCursor(Qt.WaitCursor)
            try:
                # This operation can throw WindowsError, if the directory is
                # locked.
                self.project.createBertosProject()
            except OSError, e:
                QMessageBox.critical(
                    self,
                    self.tr("Error removing destination directory"),
                    self.tr("Error removing the destination directory. This directory or a file in it is in use by another user or application.\nClose the application which is using the directory and retry."))
                self.wizard().back()
                return
        finally:
            QApplication.instance().restoreOverrideCursor()
        self.setVisible(True)
        self._plugin_dict = {}
        if os.name == "nt":
            output = self.projectInfo("OUTPUT")
            import winreg_importer
            command_lines = winreg_importer.getCommandLines()
            self.setProjectInfo("COMMAND_LINES", command_lines)
            layout = QVBoxLayout()
            for plugin in output:
                if plugin in command_lines:
                    module = bertos_utils.loadPlugin(plugin)
                    check = QCheckBox(self.tr("Open project in %s" %module.PLUGIN_NAME))
                    if len(output) == 1:
                        check.setCheckState(Qt.Checked)
                    else:
                        check.setCheckState(Qt.Unchecked)
                    layout.addWidget(check)
                    self._plugin_dict[check] = plugin
            widget = QWidget()
            widget.setLayout(layout)
            if len(self._plugin_dict) > 0:
                self.pageContent.scrollArea.setVisible(True)
            self.pageContent.scrollArea.setWidget(widget)
            for plugin in self._plugin_dict:
                self.connect(plugin, SIGNAL("stateChanged(int)"), self.modeChecked)
        self.modeChecked()
    
    def setupUi(self):
        """
        Overload of the BWizardPage setupUi method.
        """
        self.pageContent.scrollArea.setVisible(False)
    
    ####

    ## Slots ##

    def modeChecked(self):
        to_be_opened = []
        for check, plugin in self._plugin_dict.items():
            if check.checkState() == Qt.Checked:
                to_be_opened.append(plugin)
        self.setProjectInfo("TO_BE_OPENED", to_be_opened)
    
    ####
