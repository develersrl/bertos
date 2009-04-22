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
        layout = QVBoxLayout()
        for plugin in self.availablePlugins():
            check = QCheckBox(plugin)
            layout.addWidget(check)
            check.setCheckState(Qt.Checked)
            self._plugin_dict[check] = plugin
        widget = QWidget()
        widget.setLayout(layout)
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
        return plugins.__all__