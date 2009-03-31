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

from const import *

class BOutputPage(BWizardPage):
    """
    Page of the wizard that show a little summary of the previous decisions.
    """
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/output_select.ui")
        self.setTitle(self.tr("Choose the project output"))
        self._connectSignals()
        self._projectInfoStore("OUTPUT", [])
    
    def _connectSignals(self):
        """
        Connects the signals with the related slots.
        """
        self.connect(self.pageContent.eclipseCheckBox, SIGNAL("stateChanged(int)"), lambda checked: self._modeChecked(checked, "eclipse"))
        self.connect(self.pageContent.xcodeCheckBox, SIGNAL("stateChanged(int)"), lambda checked: self._modeChecked(checked, "xcode"))
        self.connect(self.pageContent.codeliteCheckBox, SIGNAL("stateChanged(int)"), lambda checked: self._modeChecked(checked, "codelite"))
    
    def _modeChecked(self, checked, value):
        """
        Slot called when one of the mode checkbox is checked. It stores it.
        """
        output_list = self._projectInfoRetrieve("OUTPUT")
        if checked == Qt.Checked:
            output_list.append(value)
        else:
            output_list.remove(value)
        self._projectInfoStore("OUTPUT", output_list)
    