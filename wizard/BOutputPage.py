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

import plugins

from const import *

class BOutputPage(BWizardPage):
    """
    Page of the wizard that show a little summary of the previous decisions.
    """
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/output_select.ui")
        self.setTitle(self.tr("Choose the project output"))
    
    ## Overloaded BWizardPage methods. ##
    
    def connectSignals(self):
        """
        Overload of the BWizardPage connectSignals method.
        """
        for plugin in self._plugin_dict:
            self.connect(plugin, SIGNAL("stateChanged(int)"), self.modeChecked)
    
    def setupUi(self):
        """
        Overload of the BWizardPage setupUi method.
        """
        self._plugin_dict = {}
        scrollLayout = QVBoxLayout()
        group, check = self.createNewOutput("BeRTOS Build System",
                                            "Classic BeRTOS makefile based project",
                                            True, False)
        scrollLayout.addWidget(group)
        plugin_list = self.plugins()
        for plugin in self.availablePlugins():
            module = bertos_utils.loadPlugin(plugin)
            selected = False
            if plugin in plugin_list: selected = True
            group, check = self.createNewOutput(module.PLUGIN_NAME,
                                                module.PLUGIN_DESCRIPTION,
                                                selected, True)
            scrollLayout.addWidget(group)
            self._plugin_dict[check] = plugin
        scrollLayout.addStretch()
        widget = QWidget()
        widget.setLayout(scrollLayout)
        self.pageContent.scrollArea.setWidget(widget)
    
    def reloadData(self):
        """
        Overload of the BWizardPage reloadData method.
        """
        self.modeChecked()
        
    ####
    
    ## Slots ##
    
    def modeChecked(self):
        """
        Slot called when one of the mode checkbox is checked. It stores it.
        """
        plugins = []
        for checkBox, plugin in self._plugin_dict.items():
            if checkBox.checkState() == Qt.Checked:
                plugins.append(plugin)
        self.setPlugins(plugins)
        self.setProjectInfo("OUTPUT", plugins)

    ####
    
    def availablePlugins(self):
        """
        Returns the list of the available plugins.
        """
        return plugins.plugin_list
    
    def createNewOutput(self, name, description, checked=True, enabled=True):
        """
        Create a groupBox for the given pieces of information. Returns the
        groupBox and the checkBox
        """
        check = QCheckBox(description)
        if checked:
            check.setCheckState(Qt.Checked)
        else:
            check.setCheckState(Qt.Unchecked)
        groupLayout = QVBoxLayout()
        groupLayout.addWidget(check)
        group = QGroupBox(name)
        group.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Maximum)
        group.setLayout(groupLayout)
        group.setEnabled(enabled)
        return group, check