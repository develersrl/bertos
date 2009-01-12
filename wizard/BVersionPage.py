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
if PYQT_VERSION_STR > "4.4.3":
    import qvariant_converter_new as qvariant_converter
else:
    import qvariant_converter_old as qvariant_converter

class BVersionPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "bertos_versions.ui")
        self._connectSignals()
        self._fillVersionList()
        self._setupUi()
        self.setTitle(self.tr("Select the BeRTOS version needed"))
    
    def _connectSignals(self):
        self.connect(self.pageContent.versionList, SIGNAL("itemSelectionChanged()"), self.rowChanged)
        self.connect(self.pageContent.addButton, SIGNAL("clicked()"), self.addVersion)
        self.connect(self.pageContent.removeButton, SIGNAL("clicked()"), self.removeVersion)
        # Fake signal connection for the update button
        self.connect(self.pageContent.updateButton, SIGNAL("clicked()"), self.updateClicked)
    
    def _setupUi(self):
        self.pageContent.versionList.setCurrentRow(-1)
    
    def _storeVersion(self, directory):
        versions = self.versions()
        versions = set(versions + [directory])
        self.setVersions(list(versions))
    
    def _deleteVersion(self, directory):
        versions = self.versions()
        versions.remove(directory)
        self.setVersions(versions)
        
    def _insertListElement(self, directory):
        if bertos_utils.isBertosDir(directory):
            item = QListWidgetItem(QIcon(":/images/ok.png"), bertos_utils.bertosVersion(directory) + " (\"" + directory + "\")")
            item.setData(Qt.UserRole, qvariant_converter.convertString(directory))
            self.pageContent.versionList.addItem(item)
        elif len(directory) > 0:
            item = QListWidgetItem(QIcon(":/images/warning.png"), "UNKNOWN" + " (\"" + directory + "\")")
            item.setData(Qt.UserRole, qvariant_converter.convertString(directory))
            self.pageContent.versionList.addItem(item)
    
    def _fillVersionList(self):
        versions = self.versions()
        for directory in versions:
            self._insertListElement(directory)

    def isComplete(self):
        if self.pageContent.versionList.currentRow() != -1:
            self._projectInfoStore("SOURCES_PATH", qvariant_converter.getString(self.pageContent.versionList.currentItem().data(Qt.UserRole)))
            return True
        else:
            return False
    
    def addVersion(self):
        directory = QFileDialog.getExistingDirectory(self, self.tr("Choose a directory"), "", QFileDialog.ShowDirsOnly | QFileDialog.DontResolveSymlinks)
        if not directory.isEmpty():
            self._storeVersion(unicode(directory))
            self.pageContent.versionList.clear()
            self._fillVersionList()
            self.emit(SIGNAL("completeChanged()"))
    
    def removeVersion(self):
        item = self.pageContent.versionList.takeItem(self.pageContent.versionList.currentRow())
        self._deleteVersion(qvariant_converter.getString(item.data(Qt.UserRole)))
        self.emit(SIGNAL("completeChanged()"))
    
    def updateClicked(self):
        print "fake update checking"
    
    def rowChanged(self):
        self.emit(SIGNAL("completeChanged()"))
    