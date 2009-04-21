#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id$
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
    """
    Page of the wizard that permits to choose the toolchain to use for the
    project.
    """

    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/toolchain_select.ui")
        self.setTitle(self.tr("Select toolchain"))
        self._validation_process = None

    ## Overloaded QWizardPage methods. ##

    def isComplete(self):
        """
        Overload of the QWizard isComplete method.
        """
        if self.pageContent.toolchainList.currentRow() != -1:
            self.setProjectInfo("TOOLCHAIN",
                qvariant_converter.getStringDict(self.pageContent.toolchainList.currentItem().data(Qt.UserRole)))
            return True
        else:
            return False

    ####

    ## Overloaded BWizardPage methods. ##

    def setupUi(self):
        """
        Sets up the user interface.
        """
        self.pageContent.infoLabel.setVisible(False)

    def connectSignals(self):
        """
        Connects the signals with the related slots.
        """
        self.connect(self.pageContent.toolchainList, SIGNAL("itemSelectionChanged()"), self.selectionChanged)
        self.connect(self.pageContent.addButton, SIGNAL("clicked()"), self.addToolchain)
        self.connect(self.pageContent.removeButton, SIGNAL("clicked()"), self.removeToolchain)
        self.connect(self.pageContent.searchButton, SIGNAL("clicked()"), self.searchToolchain)
        self.connect(self.pageContent.checkButton, SIGNAL("clicked()"), self.validateAllToolchains)

    def reloadData(self):
        """
        Overload of the BWizard reloadData method.
        """
        self._clearList()
        self.setupUi()
        self._populateToolchainList()

    ####

    ## Slots ##

    def selectionChanged(self):
        """
        Slot called when the user click on an entry of the toolchain list.
        """
        if self.pageContent.toolchainList.currentRow() != -1:
            infos = collections.defaultdict(lambda: unicode("not defined"))
            infos.update(qvariant_converter.getStringDict(self.pageContent.toolchainList.currentItem().data(Qt.UserRole)))
            self.pageContent.infoLabel.setText("GCC " + infos["version"] + " (" + infos["build"] + ")\nTarget: " + infos["target"] + "\nPath: " + os.path.normpath(infos["path"]))
            self.pageContent.infoLabel.setVisible(True)
            self.emit(SIGNAL("completeChanged()"))

    def addToolchain(self):
        """
        Slot called when the user adds manually a toolchain.
        """
        sel_toolchain = unicode(QFileDialog.getOpenFileName(self, self.tr("Choose the toolchain"), ""))
        if sel_toolchain != "":
            item = QListWidgetItem(sel_toolchain)
            item.setData(Qt.UserRole, qvariant_converter.convertStringDict({"path": sel_toolchain}))
            self.pageContent.toolchainList.addItem(item)
            toolchains = self.toolchains()
            toolchains[sel_toolchain] = False
            self.setToolchains(toolchains)

    def removeToolchain(self):
        """
        Slot called when the user removes manually a toolchain.
        """
        if self.pageContent.toolchainList.currentRow() != -1:
            item = self.pageContent.toolchainList.takeItem(self.pageContent.toolchainList.currentRow())
            toolchain = qvariant_converter.getStringDict(item.data(Qt.UserRole))["path"]
            toolchains = self.toolchains()
            del toolchains[toolchain]
            self.setToolchains(toolchains)

    def searchToolchain(self):
        """
        Slot called when the user clicks on the 'search' button. It opens the
        toolchain search dialog.
        """
        search = BToolchainSearch.BToolchainSearch()
        self.connect(search, SIGNAL("accepted()"), self._search)
        search.exec_()

    def validateAllToolchains(self):
        """
        Slot called when the user clicks on the validate button. It starts the
        toolchain validation procedure for all the toolchains.
        """
        QApplication.instance().setOverrideCursor(Qt.WaitCursor)
        for i in range(self.pageContent.toolchainList.count()):
            self.validateToolchain(i)
        QApplication.instance().restoreOverrideCursor()

    ####

    def _populateToolchainList(self):
        """
        Fills the toolchain list with the toolchains stored in the QSettings.
        """
        toolchains = self.toolchains()
        if os.name == "nt":
            import winreg_importer
            stored_toolchains = winreg_importer.getBertosToolchains()
            for toolchain in stored_toolchains:
                toolchains[toolchain] = True
        sel_toolchain = self.projectInfo("TOOLCHAIN")
        for key, value in toolchains.items():
            item = QListWidgetItem(key)
            item.setData(Qt.UserRole, qvariant_converter.convertStringDict({"path": key}))
            self.pageContent.toolchainList.addItem(item)
            if sel_toolchain and sel_toolchain["path"] == key:
                self.pageContent.toolchainList.setCurrentItem(item)
            if value:
                self.validateToolchain(self.pageContent.toolchainList.row(item))

    def _clearList(self):
        """
        Removes all the toolchain from the list.
        """
        self.pageContent.toolchainList.clear()

    def _search(self):
        """
        Searches for toolchains in the stored directories, and stores them in the
        QSettings.
        """
        dir_list = self.searchDirList()
        if self.pathSearch():
            dir_list += [element for element in bertos_utils.getSystemPath()]
        toolchain_list = bertos_utils.findToolchains(dir_list)
        stored_toolchains = self.toolchains()
        for element in toolchain_list:
            if not element in stored_toolchains:
                item = QListWidgetItem(element)
                item.setData(Qt.UserRole, qvariant_converter.convertStringDict({"path": element}))
                self.pageContent.toolchainList.addItem(item)
                stored_toolchains[element] = False
        self.setToolchains(stored_toolchains)
        self.showMessage(self.tr("Toolchain search result."), self.tr("%1 toolchains founded").arg(len(stored_toolchains)))

    def _validItem(self, index, infos):
        """
        Sets the item at index as a valid item and associates the given info to it.
        """
        item = self.pageContent.toolchainList.item(index)
        new_data = qvariant_converter.getStringDict(self.pageContent.toolchainList.item(index).data(Qt.UserRole))
        new_data.update(infos)
        item.setData(Qt.UserRole, qvariant_converter.convertStringDict(new_data))
        needed = self.projectInfo("CPU_INFOS")
        if "target" in infos and infos["target"].find(needed["TOOLCHAIN"]) != -1:
            item.setIcon(QIcon(":/images/ok.png"))
        else:
            item.setIcon(QIcon(":/images/warning.png"))
        if "version" in infos and "target" in infos:
            item.setText("GCC " + infos["version"] + " - " + infos["target"].strip())

    def _invalidItem(self, index):
        """
        Sets the item at index as an invalid item.
        """
        item = self.pageContent.toolchainList.item(index)
        item.setIcon(QIcon(":/images/error.png"))

    def validateToolchain(self, i):
        """
        Toolchain validation procedure.
        """
        filename = qvariant_converter.getStringDict(self.pageContent.toolchainList.item(i).data(Qt.UserRole))["path"]
        valid = False
        info = {}
        # Check for the other tools of the toolchain
        for tool in TOOLCHAIN_ITEMS:
            if os.path.exists(filename.replace("gcc", tool)):
                valid = True
            else:
                valid = False
                break
        # Try to retrieve the informations about the toolchain only for the valid toolchains
        if valid:
            self._validation_process = QProcess()
            self._validation_process.start(filename, ["-v"])
            self._validation_process.waitForStarted(1000)
            if self._validation_process.waitForFinished(200):
                description = str(self._validation_process.readAllStandardError())
                info = bertos_utils.getToolchainInfo(description)
                if len(info) >= 4:
                    valid = True
            else:
                self._validation_process.kill()
        # Add the item in the list with the appropriate associate data.
        if valid:
            self._validItem(i, info)
        else:
            self._invalidItem(i)
        toolchains = self.toolchains()
        toolchains[filename] = True
        self.setToolchains(toolchains)