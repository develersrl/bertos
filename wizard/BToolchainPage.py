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

from BWizardPage import *

class BToolchainPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "toolchain_select.ui")
        self.setTitle(self.tr("Select toolchain"))
        self._setupUi()
        self._connectSignals()
    
    def _setupUi(self):
        path = self._settingsRetrieve("path_search").toBool()
        if not path is None:
            self.pageContent.pathBox.setChecked(path)
        else:
            self.pageContent.pathBox.setChecked(False)
        customPath = self._settingsRetrieve("custom_dir").toBool()
        if not customPath is None:
            self.pageContent.customDirBox.setChecked(customPath)
        else:
            self.pageContent.customDirBox.setChecked(False)
        self._updateUi()
    
    def _updateUi(self):
        if self.pageContent.customDirBox.isChecked():
            self._enableCustomDir()
        else:
            self._disableCustomDir()
        if self.pageContent.pathBox.isChecked() or (self.pageContent.customDirBox.isChecked() and self.pageContent.customDirList.count() != 0):
            self.pageContent.doSearchButton.setEnabled(True)
        else:
            self.pageContent.doSearchButton.setEnabled(False)
        
    def _disableCustomDir(self):
        self.pageContent.customDirList.setEnabled(False)
        self.pageContent.addDirButton.setEnabled(False)
        self.pageContent.removeDirButton.setEnabled(False)
    
    def _enableCustomDir(self):
        self.pageContent.customDirList.setEnabled(True)
        self.pageContent.addDirButton.setEnabled(True)
        self.pageContent.removeDirButton.setEnabled(True)
    
    def _toSearchSubpage(self):
        self.pageContent.pageSelector.setCurrentIndex(1)
    
    def _toSelectionSubpage(self):
        self.pageContent.pageSelector.setCurrentIndex(0)
    
    def _connectSignals(self):
        self.connect(self.pageContent.pathBox, SIGNAL("clicked()"), self._updateUi)
        self.connect(self.pageContent.customDirBox, SIGNAL("clicked()"), self._updateUi)
        self.connect(self.pageContent.searchButton, SIGNAL("clicked()"), self._toSearchSubpage)
        self.connect(self.pageContent.cancelButton, SIGNAL("clicked()"), self._toSelectionSubpage)