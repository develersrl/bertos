#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
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

class BFolderPage(BWizardPage):
    """
    Initial page of the wizard. Permit to select the project name and the directory
    where the project will be created.
    """
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/dir_select.ui")
        self.setTitle(self.tr("Select the project name"))
        self.initializeAttributes()
    
    ## Overloaded QWizardPage methods ##

    def isComplete(self):
        """
        Overload of the QWizardPage isComplete method.
        """
        self.setDefaultFolder(self._destination_folder)
        if self.pageContent.projectPath.text() != "None":
            self.setProjectInfo("PROJECT_PATH", unicode(self.pageContent.projectPath.text()))
            return True
        else:
            return False
    
    ####

    ## Overloaded BWizardPage methods ##

    def setupUi(self):
        """
        Overload of the BWizardPage setupUi method.
        """
        self.pageContent.warningLabel.setVisible(False)
    
    def connectSignals(self):
        """
        Overload of the BWizardPage connectSignals method.
        """
        self.connect(self.pageContent.nameEdit, SIGNAL("textChanged(const QString)"), self.nameChanged)
        self.connect(self.pageContent.directoryEdit, SIGNAL("textChanged(const QString)"), self.directoryChanged)
        self.connect(self.pageContent.directoryButton, SIGNAL("clicked()"), self.selectDirectory)
    
    ####

    ## Slots ##
    
    def nameChanged(self, name):
        """
        Slot called when the project name is changed manually by the user.
        """
        self._project_name = str(name).replace(" ", "_")
        self.setProjectPath()
    
    def directoryChanged(self, directory):
        """
        Slot called when the project folder is changed manually by the user.
        """
        self._destination_folder = str(QDir.toNativeSeparators(directory))
        self.setProjectPath()

    def selectDirectory(self):
        """
        Slot called when the project folder is changed using the file dialog.
        """
        directory = unicode(QFileDialog.getExistingDirectory(self, self.tr("Open Directory"), "", QFileDialog.ShowDirsOnly))
        if len(directory) > 0:
            self.pageContent.directoryEdit.setText(directory)

    ####
    
    def initializeAttributes(self):
        """
        Initializes the page attributes to the default values.
        """
        self._project_name = ""
        stored_folder = self.defaultFolder()
        if stored_folder != "":
            self._destination_folder = stored_folder
        else:
            self._destination_folder = os.path.expanduser("~")
        self.pageContent.directoryEdit.setText(self._destination_folder)
    
    def setProjectPath(self):
        """
        Analyzes the page attributes and generates the path string.
        """
        if self._destination_folder != "" and self._project_name <> "":
            if not self._destination_folder.endswith(os.sep):
                self._destination_folder += os.sep
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
