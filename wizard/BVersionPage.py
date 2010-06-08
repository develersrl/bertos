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

from PyQt4.QtGui import *
from BWizardPage import *
import bertos_utils
import qvariant_converter

from const import *

class BVersionPage(BWizardPage):
    """
    Page of the wizard that permits to choose which BeRTOS version the user wants
    to use. This page show some pieces of information about the version.
    """
    
    def __init__(self, edit=False):
        self._edit = edit
        BWizardPage.__init__(self, UI_LOCATION + "/bertos_versions.ui")
        self.setTitle(self.tr("Select BeRTOS version"))
        self.setSubTitle(self.tr("Your project will be created with the specified BeRTOS version"))

    ## Overloaded QWizardPage methods ##
    
    def isComplete(self):
        """
        Overload of the QWizardPage isComplete method.
        """
        if self.pageContent.versionList.currentRow() != -1:
            sources_path = qvariant_converter.getString(self.pageContent.versionList.currentItem().data(Qt.UserRole))
            # Remove the trailing slash
            if sources_path.endswith(os.sep):
                sources_path = sources_path[:-1]
            self.setProjectInfo("BERTOS_PATH", sources_path)
            return True
        else:
            return False
    
    ####
    
    ## Overloaded BWizardPage methods ##

    def connectSignals(self):
        """
        Overload of the BWizardPage connectSignals method.
        """
        self.connect(self.pageContent.versionList, SIGNAL("currentItemChanged(QListWidgetItem *, QListWidgetItem*)"), self.rowChanged)
        self.connect(self.pageContent.addButton, SIGNAL("clicked()"), self.addVersion)
        self.connect(self.pageContent.removeButton, SIGNAL("clicked()"), self.removeVersion)
        # Fake signal connection for the update button
        self.connect(self.pageContent.updateButton, SIGNAL("clicked()"), self.updateClicked)
    
    def reloadData(self, previous_id=None):
        """
        Overload of the BWizardPage reloadData method.
        """
        self.resetVersionList()
        self.pageContent.versionList.setCurrentRow(-1)
        self.fillVersionList()
    
    def setupUi(self):
        """
        Overload of the BWizardPage setupUi method.
        """
        self.pageContent.updateButton.setVisible(False)
    
    ####
    
    ## Slots ##
    
    def addVersion(self):
        """
        Slot called when the user add a BeRTOS version.
        """
        directory = QFileDialog.getExistingDirectory(self, self.tr("Choose a directory"), "", QFileDialog.ShowDirsOnly | QFileDialog.DontResolveSymlinks)
        if not directory.isEmpty():
            self.storeVersion(unicode(directory))
            self.pageContent.versionList.clear()
            self.fillVersionList()
            self.emit(SIGNAL("completeChanged()"))
    
    def removeVersion(self):
        """
        Slot called when the user remove a BeRTOS version.
        """
        item = self.pageContent.versionList.takeItem(self.pageContent.versionList.currentRow())
	if item:
		self.deleteVersion(qvariant_converter.getString(item.data(Qt.UserRole)))
        self.emit(SIGNAL("completeChanged()"))
    
    def rowChanged(self):
        """
        Slot called when the user select an entry from the version list.
        """
        if self.isDefaultVersion(self.currentVersion()):
            self.disableRemoveButton()
        else:
            self.enableRemoveButton()
        self.emit(SIGNAL("completeChanged()"))

    def updateClicked(self):
        """
        Slot called when the user clicks on the 'update' button. It checks for
        update (TO BE IMPLEMENTED).
        """
        pass    

    ####
    
    def storeVersion(self, directory):
        """
        Stores the directory selected by the user in the QSettings.
        """
        versions = self.versions()
        versions = set(versions + [directory])
        self.setVersions(list(versions))
    
    def deleteVersion(self, directory):
        """
        Removes the given directory from the QSettings.
        """
        versions = [os.path.normpath(path) for path in self.versions()]
        versions.remove(os.path.normpath(directory))
        self.setVersions(versions)
    
    def resetVersionList(self):
        """
        Remove all the version entries from the list.
        """
        self.pageContent.versionList.clear()
    
    def insertListElement(self, directory):
        """
        Inserts the given directory in the version list and returns the
        inserted item.
        """
        if bertos_utils.isBertosDir(directory):
            item = QListWidgetItem(QIcon(":/images/ok.png"), bertos_utils.bertosVersion(directory) + " (\"" + os.path.normpath(directory) + "\")")
            item.setData(Qt.UserRole, qvariant_converter.convertString(directory))
            self.pageContent.versionList.addItem(item)
            return item
        elif len(directory) > 0:
            item = QListWidgetItem(QIcon(":/images/warning.png"), "UNKNOWN" + " (\"" + os.path.normpath(directory) + "\")")
            item.setData(Qt.UserRole, qvariant_converter.convertString(directory))
            self.pageContent.versionList.addItem(item)
            return item
    
    def fillVersionList(self):
        """
        Fills the version list with all the BeRTOS versions founded in the QSettings.
        """
        versions = set([])
        if os.name == "nt":
            import winreg_importer
            versions |= set([os.path.normpath(dir) for dir in winreg_importer.getBertosDirs()])
        versions |= set([os.path.normpath(dir) for dir in self.versions()])
        selected = self.projectInfo("BERTOS_PATH")
        for directory in versions:
            item = self.insertListElement(directory)
            if selected and selected == directory:
                self.setCurrentItem(item)
        if not selected:
            latest_version_item = self.latestVersionItem()
            if latest_version_item:
                self.setCurrentItem(latest_version_item)
    
    def disableRemoveButton(self):
        """
        Disable the Remove button.
        """
        self.pageContent.removeButton.setEnabled(False)

    def enableRemoveButton(self):
        """
        Enable the Remove button.
        """
        self.pageContent.removeButton.setEnabled(True)
    
    def latestVersionItem(self):
        """
        Returns the latest BeRTOS version founded.
        """
        latest_version_item = None
        for index in range(self.pageContent.versionList.count()):
            item = self.pageContent.versionList.item(index)
            if not latest_version_item:
                latest_version_item = item
            version = item.text().split(" (")[0]
            latest = latest_version_item.text().split(" (")[0]
            if version != "UNKNOWN" and version > latest:
                latest_version_item = item
        return latest_version_item
    
    def setCurrentItem(self, item):
        """
        Select the given item in the version list.
        """
        self.pageContent.versionList.setCurrentItem(item)
    
    def currentItem(self):
        """
        Returns the current selected item.
        """
        return self.pageContent.versionList.currentItem()
    
    def currentVersion(self):
        """
        Return the path of the selected version.
        """
        current = self.currentItem()
        if current:
            return qvariant_converter.getString(current.data(Qt.UserRole))
        else:
            return None
    
    def isDefaultVersion(self, version):
        """
        Returns True if the given version is one of the default versions.
        """
        if os.name == "nt":
            import winreg_importer
            if version in winreg_importer.getBertosDirs():
                return True
        return False

