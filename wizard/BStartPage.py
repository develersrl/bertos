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

from BWizardPage import *
import bertos_utils

class BStartPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "start.ui")
        self._connectSignals()
        self.setTitle(self.tr("Create or edit a BeRTOS project"))
        self._initializeButtons()
        self._setupUi()
        
    def _connectSignals(self):
        self.connect(self.pageContent.newButton, SIGNAL("clicked()"), self.newProject)
        self.connect(self.pageContent.editButton, SIGNAL("clicked()"), self.editProject)
    
    def _initializeButtons(self):
        self.buttonGroup = QButtonGroup()
        self.buttonGroup.addButton(self.pageContent.newButton)
        self.buttonGroup.addButton(self.pageContent.editButton)
        self.buttonGroup.setExclusive(True)

    def _setupUi(self):
        self.pageContent.newDescription.setVisible(False)
        self.pageContent.editDescription.setVisible(False)
    
    def isComplete(self):
        return self.pageContent.newButton.isChecked() or self.pageContent.editButton.isChecked()
    
    def newProject(self):
        filename = QFileDialog.getSaveFileName(self, self.tr("Destination directory"), "", "", "", QFileDialog.ShowDirsOnly)
        if not filename.isEmpty():
            self.pageContent.newDescription.setText("(\"" + filename + "\")")
            self.pageContent.newDescription.setVisible(True)
            # TODO: It's better to create it at the end of the wizard...
            bertos_utils.createBertosProject(filename)
            self.emit(SIGNAL("completeChanged()"))
        else:
            self.pageContent.newDescription.setText("")
            self.pageContent.newDescription.setVisible(False)
            self.pageContent.newButton.setChecked(False)
    
    def editProject(self):
        self.pageContent.newButton.setChecked(False)