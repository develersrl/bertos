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
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import os

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic

import qvariant_converter

import const

class BWizardPage(QWizardPage):
    """
    Base class for all the wizard pages. It has the utility method used in all
    the pages. A wizard page class need to extend this class.
    """
    
    def __init__(self, wizardGui, parent = None):
        QWizardPage.__init__(self, parent)
        self.pageContent = uic.loadUi(os.path.join(const.DATA_DIR, wizardGui), None)
        layout = QVBoxLayout()
        layout.addWidget(self.pageContent)
        self.setLayout(layout)
        self.setupUi()
        self.connectSignals()
    
    def exceptionOccurred(self, message):
        """
        Simple message box showing method.
        """
        QMessageBox.critical(self, self.tr("Error occurred"), message, QMessageBox.Ok, QMessageBox.NoButton)
    
    def showMessage(self, title, message):
        """
        Show an information message box with title and message.
        """
        QMessageBox.information(self, title, message)
        
    ## BProject interaction methods ##
    
    def setProjectInfo(self, key, value):
        """
        Stores the given value in the BProject class associating it with the given
        key.
        """
        QApplication.instance().project.setInfo(key, value)
    
    def projectInfo(self, key):
        """
        Retrieves the information associated with the given key.
        """
        return QApplication.instance().project.info(key)
    
    @property
    def project(self):
        """
        Returns the BProject instance.
        """
        return QApplication.instance().project
    
    ####
    
    ## QSettings interaction methods ##

    def settingsStore(self, key, value):
        """
        Stores the given value in the QSettings associated with the given key.
        """
        QApplication.instance().settings.setValue(QString(key), value)
    
    def settingsRetrieve(self, key):
        """
        Retrieves the value associated to key in the QSettings. Note that this
        Value is a QVariant and neet to be converted in a standard type.
        """
        return QApplication.instance().settings.value(QString(key), QVariant())

    def plugins(self):
        """
        Returns the list of actived plugins.
        """
        return qvariant_converter.getStringList(self.settingsRetrieve("plugins"))

    def setPlugins(self, plugins):
        """
        Stores the given list of actived plugins.
        """
        self.settingsStore("plugins", qvariant_converter.convertStringList(plugins))
    
    def versions(self):
        """
        Returns the version list from the QSettings.
        """
        return qvariant_converter.getStringList(self.settingsRetrieve("versions"))
    
    def setVersions(self, versions):
        """
        Stores the given versions in the QSettings.
        """
        self.settingsStore("versions", qvariant_converter.convertStringList(versions))
        
    def searchDirList(self):
        """
        Returns the search dir list from the QSettings.
        """
        return qvariant_converter.getStringList(self.settingsRetrieve("search_dir_list"))
    
    def setSearchDirList(self, search_dir_list):
        """
        Stores the search dir list in the QSettings.
        """
        self.settingsStore("search_dir_list", qvariant_converter.convertStringList(search_dir_list))
    
    def pathSearch(self):
        """
        Returns the value of path search from the QSettings.
        """
        return qvariant_converter.getBool(self.settingsRetrieve("path_search"))
    
    def setPathSearch(self, path_search):
        """
        Stores the path search value in the QSettings.
        """
        self.settingsStore("path_search", qvariant_converter.convertBool(path_search))
    
    def toolchains(self):
        """
        Returns the toolchains stored in the QSettings.
        """
        return qvariant_converter.getBoolDict(self.settingsRetrieve("toolchains"))

    def setToolchains(self, toolchains):
        """
        Stores the toolchains in the QSettings.
        """
        self.settingsStore("toolchains", qvariant_converter.convertBoolDict(toolchains))
    
    def defaultFolder(self):
        """
        Returns the default save folder stored in the QSettings.
        """
        return qvariant_converter.getString(self.settingsRetrieve("folder"))
    
    def setDefaultFolder(self, folder):
        """
        Stores the default save folder in the QSettings.
        """
        self.settingsStore("folder", qvariant_converter.convertString(folder))

    ####
    
    ## Methodo to be implemented in child classes when needed ##
    
    def reloadData(self, previous_id=None):
        """
        Method called before the page is loaded. The pages that need to use this
        method have to implement it.
        """
        pass
    
    def setupUi(self):
        """
        Method called automatically during the initialization of the wizard page.
        It set up the interface. Pages that need to use this method have to
        implement it.
        """
        pass
    
    def connectSignals(self):
        """
        Method called automatically during the initialization of the wizard page.
        It connects the signals and the slots. The pages that need to use this
        method have to implement it.
        """
        pass
    
    ####
