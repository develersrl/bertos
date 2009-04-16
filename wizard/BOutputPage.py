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

class BOutputPage(BWizardPage):
    """
    Page of the wizard that show a little summary of the previous decisions.
    """
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/output_select.ui")
        self.setTitle(self.tr("Choose the project output"))
        self.connectSignals()
        self.setProjectInfo("OUTPUT", [])
    
    ## Overloaded BWizardPage connectSignals method. ##
    
    def connectSignals(self):
        """
        Connects the signals with the related slots.
        """
        self.connect(self.pageContent.codeliteCheckBox, SIGNAL("stateChanged(int)"), lambda checked: self.modeChecked(checked, "codelite"))
    
    ####
    
    ## Slots ##
    
    def modeChecked(self, checked, value):
        """
        Slot called when one of the mode checkbox is checked. It stores it.
        """
        output_list = self.projectInfo("OUTPUT")
        if checked == Qt.Checked:
            output_list.append(value)
        else:
            output_list.remove(value)
        self.setProjectInfo("OUTPUT", output_list)

    ####