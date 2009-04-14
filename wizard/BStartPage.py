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

from const import *

class BStartPage(QDialog):
    
    def __init__(self):
        QDialog.__init__(self)
        self.setupUi()
        self.connectSignals()
        self.setWindowTitle(self.tr("Create or edit a BeRTOS project"))
        self.initializeButtons()
    
    def setupUi(self):
        self.content = uic.loadUi(UI_LOCATION + "/start.ui", None)
        self.setWindowIcon(QIcon(":/images/appicon.png"))
        layout = QVBoxLayout()
        layout.addWidget(self.content)
        self.setLayout(layout)
    
    def connectSignals(self):
        self.connect(self.content.newButton, SIGNAL("clicked()"), self.newProject)
        self.connect(self.content.editButton, SIGNAL("clicked()"), self.editProject)
    
    def initializeButtons(self):
        self.button_group = QButtonGroup()
        self.button_group.addButton(self.content.newButton)
        self.button_group.addButton(self.content.editButton)
        self.button_group.setExclusive(True)
        
    def newProject(self):
        self.close()
        self.emit(SIGNAL("newProject"))
    
    def editProject(self):
        self.close()
        self.emit(SIGNAL("editProject"))
