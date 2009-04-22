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

from const import *

class BFinalPage(BWizardPage):
    """
    Last page of the wizard. It creates the project and show a success message.
    """
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/final_page.ui")
        self.setTitle(self.tr("Project created successfully"))
    
    ## Overloaded BWizardPage methods ##
        
    def reloadData(self):
        """
        Overload of the BWizardPage reloadData method.
        """
        QApplication.instance().setOverrideCursor(Qt.WaitCursor)
        bertos_utils.createBertosProject(self.project())
        QApplication.instance().restoreOverrideCursor()
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