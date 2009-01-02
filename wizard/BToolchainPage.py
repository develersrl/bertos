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
import BToolchainSearch
import bertos_utils

class BToolchainPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "toolchain_select.ui")
        self.setTitle(self.tr("Select toolchain"))
        self._populateToolchainList()
        self._connectSignals()
    
    def _updateUi(self):
        if self.pageContent.customDirBox.isChecked():
            self._enableCustomDir()
        else:
            self._disableCustomDir()
        if self.pageContent.pathBox.isChecked() or (self.pageContent.customDirBox.isChecked() and self.pageContent.customDirList.count() != 0):
            self.pageContent.doSearchButton.setEnabled(True)
        else:
            self.pageContent.doSearchButton.setEnabled(False)
    
    def _populateToolchainList(self):
        toolchains = self._settingsRetrieve("toolchains").toList()
        for element in toolchains:
            item = QListWidgetItem(element.toString())
            item.setData(Qt.UserRole, element)
            self.pageContent.toolchainList.addItem(item)
            
    def _clearList(self):
        self.pageContent.toolchainList.clear()
    
    def _selectionChanged(self):
        self.emit(SIGNAL("completeChanged()"))
    
    def _search(self):
        pass
        
    def _connectSignals(self):
        self.connect(self.pageContent.toolchainList, SIGNAL("itemSelectionChanged()"), self._selectionChanged)
        self.connect(self.pageContent.addButton, SIGNAL("clicked()"), self.addToolchain)
        self.connect(self.pageContent.removeButton, SIGNAL("clicked()"), self.removeToolchain)
        self.connect(self.pageContent.searchButton, SIGNAL("clicked()"), self.searchToolchain)
    
    def addToolchain(self):
        sel_toolchain = QFileDialog.getOpenFileName(self, self.tr("Choose the toolchain"), "")
        if not sel_toolchain.isEmpty():
            item = QListWidgetItem(sel_toolchain)
            item.setData(Qt.UserRole, QVariant(sel_toolchain))
            self.pageContent.toolchainList.addItem(item)
            toolchains = self._settingsRetrieve("toolchains").toList()
            toolchains = set([toolchain.toString() for toolchain in toolchains] + [sel_toolchain])
            self._settingsStore("toolchains", list(toolchains))
    
    def removeToolchain(self):
        if self.pageContent.toolchainList.currentRow() != -1:
            item = self.pageContent.toolchainList.takeItem(self.pageContent.toolchainList.currentRow())
            item = item.data(Qt.UserRole).toString()
            toolchains = self._settingsRetrieve("toolchains").toList()
            toolchains = [toolchain.toString() for toolchain in toolchains]
            toolchains.remove(unicode(item))
            self._settingsStore("toolchains", toolchains)
    
    def searchToolchain(self):
        search = BToolchainSearch.BToolchainSearch()
        self.connect(search, SIGNAL("accepted()"), self._search)
        search.exec_()
    
    def isComplete(self):
        if self.pageContent.toolchainList.currentRow() != -1:
            self._projectInfoStore("TOOLCHAIN", self.pageContent.toolchainList.item(self.pageContent.toolchainList.currentRow()).data(Qt.UserRole).toString())
            return True
        else:
            return False