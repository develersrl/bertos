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
        for plugin in self.availablePlugins():
            module = bertos_utils.loadPlugin(plugin)
            group, check = self.createNewOutput(module.PLUGIN_NAME,
                                                module.PLUGIN_DESCRIPTION,
                                                True, True)
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