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
import qvariant_converter

from const import *

class BVersionPage(BWizardPage):
    """
    Page of the wizard that permits to choose which BeRTOS version the user wants
    to use. This page show some pieces of information about the version.
    """
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/bertos_versions.ui")
        self.setTitle(self.tr("Select the BeRTOS version needed"))

    ## Overloaded QWizardPage methods ##
    
    def isComplete(self):
        """
        Overload of the QWizardPage isComplete method.
        """
        if self.pageContent.versionList.currentRow() != -1:
            sources_path = qvariant_converter.getString(self.pageContent.versionList.currentItem().data(Qt.UserRole))
            ## Remove the trailing slash
            if sources_path.endswith(os.sep):
                sources_path = sources_path[:-1]
            self.setProjectInfo("SOURCES_PATH", sources_path)
            return True
        else:
            return False
    
    ####
    
    ## Overloaded BWizardPage methods ##

    def connectSignals(self):
        """
        Overload of the BWizardPage connectSignals method.
        """
        self.connect(self.pageContent.versionList, SIGNAL("itemSelectionChanged()"), self.rowChanged)
        self.connect(self.pageContent.addButton, SIGNAL("clicked()"), self.addVersion)
        self.connect(self.pageContent.removeButton, SIGNAL("clicked()"), self.removeVersion)
        # Fake signal connection for the update button
        self.connect(self.pageContent.updateButton, SIGNAL("clicked()"), self.updateClicked)
    
    def reloadData(self):
        """
        Overload of the BWizardPage reloadData method.
        """
        self.pageContent.versionList.setCurrentRow(-1)
        self.fillVersionList()
    
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
        self.deleteVersion(qvariant_converter.getString(item.data(Qt.UserRole)))
        self.emit(SIGNAL("completeChanged()"))
    
    def rowChanged(self):
        """
        Slot called when the user select an entry from the version list.
        """
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
        versions = self.versions()
        versions.remove(directory)
        self.setVersions(versions)
        
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
        versions = self.versions()
        selected = self.projectInfo("SOURCES_PATH")
        for directory in versions:
            item = self.insertListElement(directory)
            if not selected is None and selected == directory:
                self.setCurrentItem(item)
    
    def setCurrentItem(self, item):
        """
        Select the given item in the version list.
        """
        self.pageContent.versionList.setCurrentItem(item)