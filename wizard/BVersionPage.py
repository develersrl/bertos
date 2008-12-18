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

from PyQt4.QtGui import *
from BWizardPage import *
import bertos_utils

class BVersionPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "bertos_versions.ui")
        self._connectSignals()
        self._fillVersionList()
        self._setupUi()
        self.setTitle(self.tr("Select the BeRTOS version needed"))
    
    def _connectSignals(self):
        self.connect(self.pageContent.versionList, SIGNAL("itemClicked(QListWidgetItem *)"), self.itemClicked)
        self.connect(self.pageContent.addButton, SIGNAL("clicked()"), self.addVersion)
        self.connect(self.pageContent.removeButton, SIGNAL("clicked()"), self.removeVersion)
        # Fake signal connection for the update button
        self.connect(self.pageContent.updateButton, SIGNAL("clicked()"), self.updateClicked)
    
    def _setupUi(self):
        pass
    
    def _storeVersion(self, directory):
        versions = self._settingsRetrieve("versions").toList()
        versions = set([x.toString() for x in versions] + [directory])
        self._settingsStore("versions", list(versions))
    
    def _deleteVersion(self, directory):
        versions = self._settingsRetrieve(QString("versions")).toList()
        versions = [x.toString() for x in versions]
        versions.remove(directory)
        self._settingsStore("versions", versions)
        
    def _insertListElement(self, directory):
        if bertos_utils.isBertosDir(directory):
            item = QListWidgetItem(QIcon(":/images/ok.png"), bertos_utils.bertosVersion(directory) + " (\"" + directory + "\")")
            item.setData(Qt.UserRole, QVariant(directory))
            self.pageContent.versionList.addItem(item)
        elif not directory.isEmpty():
            item = QListWidgetItem(QIcon(":/images/warning.png"), "UNKNOWN" + " (\"" + directory + "\")")
            item.setData(Qt.UserRole, QVariant(directory))
            self.pageContent.versionList.addItem(item)
    
    def _fillVersionList(self):
        versions = self._settingsRetrieve("versions").toList()
        for directory in versions:
            self._insertListElement(directory.toString())

    def isComplete(self):
        if self.pageContent.versionList.currentRow() != -1:
            self._projectInfoStore("SOURCES_PATH", self.pageContent.versionList.currentItem().data(Qt.UserRole).toString())
            return True
        else:
            return False
    
    def addVersion(self):
        directory = QFileDialog.getExistingDirectory(self, self.tr("Choose a directory"), "", QFileDialog.ShowDirsOnly | QFileDialog.DontResolveSymlinks)
        if not directory.isEmpty():
            self._storeVersion(directory)
            self.pageContent.versionList.clear()
            self._fillVersionList()
            self.emit(SIGNAL("completeChanged()"))
    
    def removeVersion(self):
        item = self.pageContent.versionList.takeItem(self.pageContent.versionList.currentRow())
        self._deleteVersion(item.data(Qt.UserRole).toString())
        self.emit(SIGNAL("completeChanged()"))
    
    def updateClicked(self):
        print "fake update checking"
    
    def itemClicked(self, item):
        self.emit(SIGNAL("completeChanged()"))
    