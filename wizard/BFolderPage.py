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

import os

from PyQt4.QtGui import *
from BWizardPage import *
import bertos_utils

class BFolderPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "dir_select.ui")
        self.setTitle(self.tr("Select the project name"))
        self._initializeAttributes()
        self._connectSignals()

    def _initializeAttributes(self):
        self._projectName = ""
        self._destinationFolder = os.path.expanduser("~")
        self.pageContent.directoryEdit.setText(self._destinationFolder)
    
    def _connectSignals(self):
        self.connect(self.pageContent.nameEdit, SIGNAL("textChanged(const QString)"), self._nameChanged)
        self.connect(self.pageContent.directoryEdit, SIGNAL("textChanged(const QString)"), self._directoryChanged)
        self.connect(self.pageContent.directoryButton, SIGNAL("clicked()"), self._selectDirectory)
    
    def _nameChanged(self, name):
        self._projectName = str(name).replace(" ", "_")
        self._setProjectPath()
    
    def _directoryChanged(self, directory):
        self._destinationFolder = str(directory)
        self._setProjectPath()
    
    def _setProjectPath(self):
        if self._destinationFolder != "" and self._projectName <> "":
            if not self._destinationFolder.endswith(os.sep):
                self._destinationFolder += os.sep
            self.pageContent.projectPath.setText(self._destinationFolder + self._projectName)
        else:
            self.pageContent.projectPath.setText("None")
        self.emit(SIGNAL("completeChanged()"))
    
    def _selectDirectory(self):
        directory = unicode(QFileDialog.getExistingDirectory(self, self.tr("Open Directory"), "", QFileDialog.ShowDirsOnly))
        if len(directory) == "":
            self.pageContent.directoryEdit.setText(directory)
    
    def isComplete(self):
        if self.pageContent.projectPath.text() != "None":
            self._projectInfoStore("PROJECT_PATH", unicode(self.pageContent.projectPath.text()))
            return True
        else:
            return False