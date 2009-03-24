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

from const import *

class BFolderPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/dir_select.ui")
        self.setTitle(self.tr("Select the project name"))
        self._initializeAttributes()
        self._setupUi()
        self._connectSignals()

    def _setupUi(self):
        self.pageContent.warningLabel.setVisible(False)
    
    def _initializeAttributes(self):
        self._project_name = ""
        self._destination_folder = os.path.expanduser("~")
        self.pageContent.directoryEdit.setText(self._destination_folder)
    
    def _connectSignals(self):
        self.connect(self.pageContent.nameEdit, SIGNAL("textChanged(const QString)"), self._nameChanged)
        self.connect(self.pageContent.directoryEdit, SIGNAL("textChanged(const QString)"), self._directoryChanged)
        self.connect(self.pageContent.directoryButton, SIGNAL("clicked()"), self._selectDirectory)
    
    def _nameChanged(self, name):
        self._project_name = str(name).replace(" ", "_")
        self._setProjectPath()
    
    def _directoryChanged(self, directory):
        self._destination_folder = str(QDir.toNativeSeparators(directory))
        self._setProjectPath()
    
    def _setProjectPath(self):
        if self._destination_folder != "" and self._project_name <> "":
            if not self._destination_folder.endswith(os.sep):
                self._destination_folder += "/"
            self.pageContent.projectPath.setText(QDir.toNativeSeparators(self._destination_folder + self._project_name))
            if os.path.exists(self._destination_folder + self._project_name):
                self.pageContent.warningLabel.setVisible(True)
                self.pageContent.warningLabel.setText(self.tr("<font color='#FF0000'>Warning: the selected directory exists, \
                    it will be destroyed with all contained subdirectories and files...</font>"))
            else:
                self.pageContent.warningLabel.setVisible(False)
                self.pageContent.warningLabel.setText("")
        else:
            self.pageContent.projectPath.setText("None")
            self.pageContent.warningLabel.setVisible(False)
            self.pageContent.warningLabel.setText("")
        self.emit(SIGNAL("completeChanged()"))
    
    def _selectDirectory(self):
        directory = unicode(QFileDialog.getExistingDirectory(self, self.tr("Open Directory"), "", QFileDialog.ShowDirsOnly))
        if len(directory) > 0:
            self.pageContent.directoryEdit.setText(directory)
    
    def isComplete(self):
        if self.pageContent.projectPath.text() != "None":
            self._projectInfoStore("PROJECT_PATH", unicode(self.pageContent.projectPath.text()))
            return True
        else:
            return False