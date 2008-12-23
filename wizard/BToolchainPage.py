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
import bertos_utils

class BToolchainPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "toolchain_select.ui")
        self.setTitle(self.tr("Select toolchain"))
        self._setupUi()
        self._populateDirList()
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
    
    def _populateDirList(self):
        search_dir_list = self._settingsRetrieve("search_dir_list").toList()
        for element in search_dir_list:
            item = QListWidgetItem(element.toString())
            item.setData(Qt.UserRole, element)
            self.pageContent.customDirList.addItem(item)
        
    def _disableCustomDir(self):
        self.pageContent.customDirList.setEnabled(False)
        self.pageContent.addDirButton.setEnabled(False)
        self.pageContent.removeDirButton.setEnabled(False)
    
    def _enableCustomDir(self):
        self.pageContent.customDirList.setEnabled(True)
        self.pageContent.addDirButton.setEnabled(True)
        self.pageContent.removeDirButton.setEnabled(True)
    
    def _clearList(self):
        self.pageContent.toolchainList.clear()
    
    def _connectSignals(self):
        self.connect(self.pageContent.pathBox, SIGNAL("clicked()"), self._updateUi)
        self.connect(self.pageContent.customDirBox, SIGNAL("clicked()"), self._updateUi)
        self.connect(self.pageContent.searchButton, SIGNAL("clicked()"), self.toSearchSubpage)
        self.connect(self.pageContent.cancelButton, SIGNAL("clicked()"), self.toSelectionSubpage)
        self.connect(self.pageContent.doSearchButton, SIGNAL("clicked()"), self.doSearch)
        self.connect(self.pageContent.addDirButton, SIGNAL("clicked()"), self.addDir)
        self.connect(self.pageContent.removeDirButton, SIGNAL("clicked()"), self.removeDir)
    
    def toSearchSubpage(self):
        self.pageContent.pageSelector.setCurrentIndex(1)

    def toSelectionSubpage(self):
        self.pageContent.pageSelector.setCurrentIndex(0)    

    def doSearch(self):
        path = []
        if self.pageContent.pathBox.isChecked():
            path += bertos_utils.getSystemPath()
        if self.pageContent.customDirBox.isChecked():
            for element in range(self.pageContent.customDirList.count()):
                path += [unicode(self.pageContent.customDirList.item(element).data(Qt.UserRole).toString())]
        toolchains = bertos_utils.findToolchains(path)
        self._clearList()
        for toolchain in toolchains:
            self.pageContent.toolchainList.addItem(QListWidgetItem(toolchain))
        self.toSelectionSubpage()
    
    def addDir(self):
        directory = QFileDialog.getExistingDirectory(self, self.tr("Open Directory"), "", QFileDialog.ShowDirsOnly)
        if not directory.isEmpty():
            item = QListWidgetItem(directory)
            item.setData(Qt.UserRole, QVariant(directory))
            self.pageContent.customDirList.addItem(item)
            search_dir_list = self._settingsRetrieve("search_dir_list").toList()
            search_dir_list = set([d.toString() for d in search_dir_list] + [directory])
            self._settingsStore("search_dir_list", list(search_dir_list))
    
    def removeDir(self):
        item = self.pageContent.customDirList.takeItem(self.pageContent.customDirList.currentRow())
        search_dir_list = self._settingsRetrieve("search_dir_list").toList()
        search_dir_list = set([d.toString() for d in search_dir_list])
        search_dir_list.remove(item.data(Qt.UserRole).toString())
        self._settingsStore("search_dir_list", list(search_dir_list))