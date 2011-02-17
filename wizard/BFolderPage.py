#!/usr/bin/env python
# encoding: utf-8
#
# This file is part of BeRTOS.
#
# Bertos is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this
# file does not by itself cause the resulting executable to be covered by
# the GNU General Public License.  This exception does not however
# invalidate any other reasons why the executable file might be covered by
# the GNU General Public License.
#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
#
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import os

from PyQt4.QtGui import *
from BWizardPage import *
import bertos_utils

from BCpuPage import BCpuPage
from BBoardPage import BBoardPage

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
            self.setProjectInfo("PROJECT_NAME", os.path.basename(unicode(self.pageContent.projectPath.text())))
            self.setProjectInfo("PROJECT_SRC_PATH", os.path.join(self.projectInfo("PROJECT_PATH"), self.projectInfo("PROJECT_NAME")))
            self.setProjectInfo("PROJECT_SRC_PATH_FROM_MAKEFILE", self.projectInfo("PROJECT_NAME"))
            self.setProjectInfo("PROJECT_HW_PATH", os.path.join(self.projectInfo("PROJECT_PATH"), self.projectInfo("PROJECT_NAME")))
            self.setProjectInfo("PROJECT_HW_PATH_FROM_MAKEFILE", self.projectInfo("PROJECT_NAME"))
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
        # self.connect(self.pageContent.customButton, SIGNAL("toggled(bool)"), self.isComplete)
    
    ####

    ## Slots ##
    
    def nameChanged(self, name):
        """
        Slot called when the project name is changed manually by the user.
        """
        try:
            name = unicode(name).encode("ascii")
        except UnicodeEncodeError:
            name = self._project_name
            self.pageContent.nameEdit.setText(name)
        self._project_name = unicode(name).replace(" ", "_")
        self.setProjectPath()
    
    def directoryChanged(self, directory):
        """
        Slot called when the project folder is changed manually by the user.
        """
        try:
            directory = unicode(directory).encode("ascii")
        except UnicodeEncodeError:
            directory = self._destination_folder
            self.pageContent.directoryEdit.setText(directory)
        self._destination_folder = directory
        self.setProjectPath()

    def selectDirectory(self):
        """
        Slot called when the project folder is changed using the file dialog.
        """
        directory = unicode(QFileDialog.getExistingDirectory(self, self.tr("Open Directory"), self.pageContent.directoryEdit.text(), QFileDialog.ShowDirsOnly))
        if len(directory) > 0:
            self.pageContent.directoryEdit.setText(QDir.toNativeSeparators(directory))

    ####
    
    def initializeAttributes(self):
        """
        Initializes the page attributes to the default values.
        """
        self._project_name = ""
        stored_folder = self.defaultFolder()
        if stored_folder != "":
            self._destination_folder = stored_folder
        elif os.name == "nt":
            def _winGetSpecialFolder(csidl):
                from ctypes import windll, create_unicode_buffer
                MAX_PATH = 4096
                buf = create_unicode_buffer(MAX_PATH)
                if not windll.shell32.SHGetSpecialFolderPathW(0, buf, csidl, False):
                    raise WindowsError("cannot get special folder location")
                return buf.value
            CSIDL_PERSONAL = 5
            self._destination_folder = _winGetSpecialFolder(CSIDL_PERSONAL)
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
