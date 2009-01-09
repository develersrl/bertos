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

from BWizardPage import *
import BToolchainSearch
import bertos_utils
import qvariant_converter

class BToolchainPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "toolchain_select.ui")
        self.setTitle(self.tr("Select toolchain"))
        self._validationProcess = None
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
        toolchains = self.toolchains()
        for element in toolchains:
            item = QListWidgetItem(element)
            item.setData(Qt.UserRole, qvariant_converter.convertString(element))
            self.pageContent.toolchainList.addItem(item)
            
    def _clearList(self):
        self.pageContent.toolchainList.clear()
    
    def _selectionChanged(self):
        self.emit(SIGNAL("completeChanged()"))
    
    def _search(self):
        dirList = self.searchDirList()
        if(self.pathSearch()):
            dirList += [element for element in bertos_utils.getSystemPath()]
        toolchainList = bertos_utils.findToolchains(dirList)
        storedToolchainList = self.toolchains()
        toolchainList = set(toolchainList) - set(storedToolchainList)
        for element in toolchainList:
            item = QListWidgetItem(element)
            item.setData(Qt.UserRole, qvariant_converter.convertString(element))
            self.pageContent.toolchainList.addItem(item)
        self.setToolchains(list(toolchainList.union(storedToolchainList)))
        
    def _connectSignals(self):
        self.connect(self.pageContent.toolchainList, SIGNAL("itemSelectionChanged()"), self._selectionChanged)
        self.connect(self.pageContent.addButton, SIGNAL("clicked()"), self.addToolchain)
        self.connect(self.pageContent.removeButton, SIGNAL("clicked()"), self.removeToolchain)
        self.connect(self.pageContent.searchButton, SIGNAL("clicked()"), self.searchToolchain)
        self.connect(self.pageContent.checkButton, SIGNAL("clicked()"), self.validateToolchains)
    
    def _validItem(self, index, infos):
        item = self.pageContent.toolchainList.item(index)
        needed = self._projectInfoRetrieve("CPU_INFOS")
        if infos["target"].find(needed["TOOLCHAIN"]) != -1:
            item.setIcon(QIcon(":/images/ok.png"))
        else:
            item.setIcon(QIcon(":/images/warning.png"))
        item.setText(infos["version"] + " " + infos["target"])
    
    def _invalidItem(self, index):
        item = self.pageContent.toolchainList.item(index)
        item.setIcon(QIcon(":/images/error.png"))
    
    def addToolchain(self):
        sel_toolchain = QFileDialog.getOpenFileName(self, self.tr("Choose the toolchain"), "")
        if not sel_toolchain.isEmpty():
            item = QListWidgetItem(sel_toolchain)
            item.setData(Qt.UserRole, qvariant_converter.convertString(sel_toolchain))
            self.pageContent.toolchainList.addItem(item)
            toolchains = self.toolchains()
            toolchains = set(toolchains + [sel_toolchain])
            self.setToolchains(list(toolchains))
    
    def removeToolchain(self):
        if self.pageContent.toolchainList.currentRow() != -1:
            item = self.pageContent.toolchainList.takeItem(self.pageContent.toolchainList.currentRow())
            toolchain = qvariant_converter.getString(item.data(Qt.UserRole))
            toolchains = self.toolchains()
            toolchains.remove(toolchain)
            self.setToolchains(toolchains)
    
    def searchToolchain(self):
        search = BToolchainSearch.BToolchainSearch()
        self.connect(search, SIGNAL("accepted()"), self._search)
        search.exec_()
    
    def validateToolchains(self):
        for i in range(self.pageContent.toolchainList.count()):
            filename = qvariant_converter.getString(self.pageContent.toolchainList.item(i).data(Qt.UserRole))
            self._validationProcess = QProcess()
            self._validationProcess.start(filename, ["-v"])
            self._validationProcess.waitForStarted(1000)
            if self._validationProcess.waitForFinished(200):
                description = str(self._validationProcess.readAllStandardError())
                infos = bertos_utils.getToolchainInfo(description)
                if len(infos.keys()) == 4:
                    self._validItem(i, infos)
                else:
                    self._invalidItem(i)
            else:
                self._validationProcess.kill()
                self._invalidItem(i)
    
    def isComplete(self):
        if self.pageContent.toolchainList.currentRow() != -1:
            self._projectInfoStore("TOOLCHAIN", 
                qvariant_converter.getString(self.pageContent.toolchainList.item(self.pageContent.toolchainList.currentRow()).data(Qt.UserRole)))
            return True
        else:
            return False