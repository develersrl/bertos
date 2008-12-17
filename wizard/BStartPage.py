#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id:$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

from PyQt4.QtCore import *
from PyQt4.QtGui import *
import PyQt4.uic as uic

class BStartPage(QWidget):
    
    def __init__(self):
        QDialog.__init__(self)
        self._setupUi()
        self._connectSignals()
        self.setWindowTitle(self.tr("Create or edit a BeRTOS project"))
        self._initializeButtons()
    
    def _setupUi(self):
        self.content = uic.loadUi("start.ui", None)
        layout = QVBoxLayout()
        layout.addWidget(self.content)
        self.setLayout(layout)
    
    def _connectSignals(self):
        self.connect(self.content.newButton, SIGNAL("clicked()"), self.newProject)
        self.connect(self.content.editButton, SIGNAL("clicked()"), self.editProject)
    
    def _initializeButtons(self):
        self.buttonGroup = QButtonGroup()
        self.buttonGroup.addButton(self.content.newButton)
        self.buttonGroup.addButton(self.content.editButton)
        self.buttonGroup.setExclusive(True)
        
    def newProject(self):
        self.close()
        self.emit(SIGNAL("newProject"))
    
    def editProject(self):
        self.close()
        self.emit(SIGNAL("editProject"))
