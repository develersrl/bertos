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


class BOutputPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "output_select.ui")
        self.setTitle(self.tr("Choose the project output"))
        self._setupButtonGroup()
        self._connectSignals()
    
    def _connectSignals(self):
        self.connect(self._buttonGroup, SIGNAL("buttonClicked(int)"), self._buttonClicked)
    
    def _setupButtonGroup(self):
        self._buttonGroup = QButtonGroup()
        self._buttonGroup.addButton(self.pageContent.bbsButton)
        self._buttonGroup.addButton(self.pageContent.eclipseButton)
        self._buttonGroup.addButton(self.pageContent.codeliteButton)
        self._buttonGroup.addButton(self.pageContent.xcodeButton)
    
    def _buttonClicked(self):
        self.emit(SIGNAL("completeChanged()"))
    
    def isComplete(self):
        for button in self._buttonGroup.buttons():
            if button.isChecked():
                if button is self.pageContent.bbsButton:
                    self._projectInfoStore("OUTPUT", "makefile")
                elif button is self.pageContent.eclipseButton:
                    self._projectInfoStore("OUTPUT", "eclipse")
                elif button is self.pageContent.codeliteButton:
                    self._projectInfoStore("OUTPUT", "codelite")
                elif button is self.pageContent.xcodeButton:
                    self._projectInfoStore("OUTPUT", "xcode")
                return True
        return False