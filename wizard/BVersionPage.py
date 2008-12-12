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
        self._setupUi()
        self.setTitle(self.tr("Select the beRTOS version needed"))
    
    def _connectSignals(self):
        self.connect(self.pageContent.versionList, SIGNAL("itemClicked(QListWidgetItem *)"), self.itemClicked)
        self.connect(self.pageContent.addButton, SIGNAL("clicked()"), self.addVersion)
        self.connect(self.pageContent.removeButton, SIGNAL("clicked()"), self.removeVersion)
        # Fake signal connection for the update button
        self.connect(self.pageContent.updateButton, SIGNAL("clicked()"), self.updateClicked)
    
    def _setupUi(self):
        pass
    
    def addVersion(self):
        directory = QFileDialog.getExistingDirectory(self, self.tr("Choose a directory"), QString(), QFileDialog.ShowDirsOnly | QFileDialog.DontResolveSymlinks)
        if bertos_utils.isBertosDir(directory):
            version = bertos_utils.bertosVersion(directory)
            self.pageContent.versionList.addItem(QListWidgetItem(QIcon(":/images/ok.png"), version + " (\"" + directory + "\")"))
        elif not directory.isEmpty():
            version = "UNCHECKED"
            self.pageContent.versionList.addItem(QListWidgetItem(QIcon(":/images/warning.png"), version + " (\"" + directory + "\")"))
    
    def removeVersion(self):
        self.pageContent.versionList.takeItem(self.pageContent.versionList.currentRow())
    
    def updateClicked(self):
        print "fake update checking"
    
    def itemClicked(self, item):
        print "clicked", repr(item)
    