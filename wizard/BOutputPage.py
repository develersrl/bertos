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
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/output_select.ui")
        self.setTitle(self.tr("Choose the project output"))
        self._connectSignals()
        self._projectInfoStore("OUTPUT", [])
    
    def _connectSignals(self):
        self.connect(self.pageContent.eclipseCheckBox, SIGNAL("stateChanged(int)"), lambda checked: self._modeChecked(checked, "eclipse"))
        self.connect(self.pageContent.xcodeCheckBox, SIGNAL("stateChanged(int)"), lambda checked: self._modeChecked(checked, "xcode"))
        self.connect(self.pageContent.codeliteCheckBox, SIGNAL("stateChanged(int)"), lambda checked: self._modeChecked(checked, "codelite"))
    
    def _modeChecked(self, checked, value):
        outputList = self._projectInfoRetrieve("OUTPUT")
        if checked == Qt.Checked:
            outputList.append(value)
        else:
            outputList.remove(value)
        self._projectInfoStore("OUTPUT", outputList)
    