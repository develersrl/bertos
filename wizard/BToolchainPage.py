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
import collections

from BWizardPage import *
import BToolchainSearch
import bertos_utils
import qvariant_converter

from const import *

class BToolchainPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/toolchain_select.ui")
        self.setTitle(self.tr("Select toolchain"))
        self._validation_process = None
        self._updateUi()
        #self._populateToolchainList()
        self._connectSignals()
    
    def _updateUi(self):
        self.pageContent.infoLabel.setVisible(False)
    
    def _populateToolchainList(self):
        toolchains = self.toolchains()
        for key, value in toolchains.items():
            item = QListWidgetItem(key)
            item.setData(Qt.UserRole, qvariant_converter.convertStringDict({"path": key}))
            self.pageContent.toolchainList.addItem(item)
            if value:
                self.validateToolchain(self.pageContent.toolchainList.row(item))

    def _clearList(self):
        self.pageContent.toolchainList.clear()
    
    def _selectionChanged(self):
        if self.pageContent.toolchainList.currentRow() != -1:
            infos = collections.defaultdict(lambda: unicode("not defined"))
            infos.update(qvariant_converter.getStringDict(self.pageContent.toolchainList.currentItem().data(Qt.UserRole)))
            self.pageContent.infoLabel.setText("GCC " + infos["version"] + " (" + infos["build"] + ")\nTarget: " + infos["target"] + "\nPath: " + os.path.normpath(infos["path"]))
            self.pageContent.infoLabel.setVisible(True)
            self.emit(SIGNAL("completeChanged()"))
    
    def _search(self):
        dir_list = self.searchDirList()
        if self.pathSearch():
            dir_list += [element for element in bertos_utils.getSystemPath()]
        toolchain_list = bertos_utils.findToolchains(dir_list)
        stored_toolchains = self.toolchains()
        for element in toolchain_list:
            if not element in stored_toolchains.keys():
                item = QListWidgetItem(element)
                item.setData(Qt.UserRole, qvariant_converter.convertStringDict({"path": element}))
                self.pageContent.toolchainList.addItem(item)
                stored_toolchains[element] = False
        self.setToolchains(stored_toolchains)
        
    def _connectSignals(self):
        self.connect(self.pageContent.toolchainList, SIGNAL("itemSelectionChanged()"), self._selectionChanged)
        self.connect(self.pageContent.addButton, SIGNAL("clicked()"), self.addToolchain)
        self.connect(self.pageContent.removeButton, SIGNAL("clicked()"), self.removeToolchain)
        self.connect(self.pageContent.searchButton, SIGNAL("clicked()"), self.searchToolchain)
        self.connect(self.pageContent.checkButton, SIGNAL("clicked()"), self.validateAllToolchains)
    
    def _validItem(self, index, infos):
        item = self.pageContent.toolchainList.item(index)
        new_data = qvariant_converter.getStringDict(self.pageContent.toolchainList.item(index).data(Qt.UserRole))
        new_data.update(infos)
        item.setData(Qt.UserRole, qvariant_converter.convertStringDict(new_data))
        needed = self._projectInfoRetrieve("CPU_INFOS")
        if "target" in infos.keys() and infos["target"].find(needed["TOOLCHAIN"]) != -1:
            item.setIcon(QIcon(":/images/ok.png"))
        else:
            item.setIcon(QIcon(":/images/warning.png"))
        if "version" in infos.keys() and "target" in infos.keys():
            item.setText("GCC " + infos["version"] + " - " + infos["target"])
    
    def _invalidItem(self, index):
        item = self.pageContent.toolchainList.item(index)
        item.setIcon(QIcon(":/images/error.png"))
    
    def addToolchain(self):
        sel_toolchain = QFileDialog.getOpenFileName(self, self.tr("Choose the toolchain"), "")
        if not sel_toolchain.isEmpty():
            item = QListWidgetItem(sel_toolchain)
            item.setData(Qt.UserRole, qvariant_converter.convertString({"path": sel_toolchain}))
            self.pageContent.toolchainList.addItem(item)
            toolchains = self.toolchains()
            toolchains[sel_toolchain] = False
            self.setToolchains(toolchains)
    
    def removeToolchain(self):
        if self.pageContent.toolchainList.currentRow() != -1:
            item = self.pageContent.toolchainList.takeItem(self.pageContent.toolchainList.currentRow())
            toolchain = qvariant_converter.getStringDict(item.data(Qt.UserRole))["path"]
            toolchains = self.toolchains()
            del toolchains[toolchain]
            self.setToolchains(toolchains)
    
    def searchToolchain(self):
        search = BToolchainSearch.BToolchainSearch()
        self.connect(search, SIGNAL("accepted()"), self._search)
        search.exec_()
    
    def validateAllToolchains(self):
        QApplication.instance().setOverrideCursor(Qt.WaitCursor)
        for i in range(self.pageContent.toolchainList.count()):
            self.validateToolchain(i)
        QApplication.instance().restoreOverrideCursor()
    
    def validateToolchain(self, i):
        filename = qvariant_converter.getStringDict(self.pageContent.toolchainList.item(i).data(Qt.UserRole))["path"]
        valid = False
        info = {}
        ## Check for the other tools of the toolchain
        for tool in TOOLCHAIN_ITEMS:
            if os.path.exists(filename.replace("gcc", tool)):
                valid = True
            else:
                valid = False
                break
        ## Try to retrieve the informations about the toolchain only for the valid toolchains
        if valid:
            self._validation_process = QProcess()
            self._validation_process.start(filename, ["-v"])
            self._validation_process.waitForStarted(1000)
            if self._validation_process.waitForFinished(200):
                description = str(self._validation_process.readAllStandardError())
                info = bertos_utils.getToolchainInfo(description)
                if len(info.keys()) >= 4:
                    valid = True
            else:
                self._validation_process.kill()
        ## Add the item in the list with the appropriate associate data.
        if valid:
            self._validItem(i, info)
        else:
            self._invalidItem(i)
        toolchains = self.toolchains()
        toolchains[filename] = True
        self.setToolchains(toolchains)
    
    def isComplete(self):
        if self.pageContent.toolchainList.currentRow() != -1:
            self._projectInfoStore("TOOLCHAIN", 
                qvariant_converter.getStringDict(self.pageContent.toolchainList.currentItem().data(Qt.UserRole)))
            return True
        else:
            return False
    
    def reloadData(self):
        self._clearList()
        self._updateUi()
        self._populateToolchainList()