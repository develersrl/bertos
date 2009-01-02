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
        dirList = [unicode(element.toString()) for element in self._settingsRetrieve("search_dir_list").toList()]
        dirList += [element for element in bertos_utils.getSystemPath()]
        toolchainList = bertos_utils.findToolchains(dirList)
        for element in toolchainList:
            self.pageContent.toolchainList.addItem(QListWidgetItem(element))
        self._settingsStore("toolchains", toolchainList)
        
    def _connectSignals(self):
        self.connect(self.pageContent.toolchainList, SIGNAL("itemSelectionChanged()"), self._selectionChanged)
        self.connect(self.pageContent.addButton, SIGNAL("clicked()"), self.addToolchain)
        self.connect(self.pageContent.removeButton, SIGNAL("clicked()"), self.removeToolchain)
        self.connect(self.pageContent.searchButton, SIGNAL("clicked()"), self.searchToolchain)
        self.connect(self.pageContent.checkButton, SIGNAL("clicked()"), self.validateToolchains)
    
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
    
    def validateToolchains(self):
        print "validating toolchains"
        print self.pageContent.toolchainList.count()
        for i in range(self.pageContent.toolchainList.count()):
            filename = self.pageContent.toolchainList.item(i).text()
            print i, filename
            self._validationProcess = QProcess()
            self._validationProcess.start(filename, ["-v"])
            self._validationProcess.waitForStarted(5)
            if not self._validationProcess.waitForFinished(5):
                self._validationProcess.kill()
                print "process killed"
            else:
                print self._validationProcess.readAllStandardError()
    
    def _getToolchainInfo():
        print self._validationProcess.readAllStandardOutput()
    
    def isComplete(self):
        if self.pageContent.toolchainList.currentRow() != -1:
            self._projectInfoStore("TOOLCHAIN", self.pageContent.toolchainList.item(self.pageContent.toolchainList.currentRow()).data(Qt.UserRole).toString())
            return True
        else:
            return False