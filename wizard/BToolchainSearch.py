#!/usr/bin/env python
# encoding: utf-8
#
# This file is part of BeRTOS.
#
# Bertos is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this
# file does not by itself cause the resulting executable to be covered by
# the GNU General Public License.  This exception does not however
# invalidate any other reasons why the executable file might be covered by
# the GNU General Public License.
#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
#
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import os

from PyQt4.QtCore import *
from PyQt4.QtGui import *
import PyQt4.uic as uic

import qvariant_converter

import const

class BToolchainSearch(QDialog):
    """
    Dialog that permits to choice the settings for the toolchain search procedure.
    """
    
    def __init__(self):
        QDialog.__init__(self)
        self._setupUi()
        self._connectSignals()
        self.setWindowTitle(self.tr("Toolchain search page"))
    
    def _setupUi(self):
        """
        Sets up the user interface.
        """
        self.content = uic.loadUi(os.path.join(const.DATA_DIR, const.UI_LOCATION, "toolchain_search.ui"), None)
        layout = QVBoxLayout()
        layout.addWidget(self.content)
        self.setLayout(layout)
        self._populateDirList()
        self._setPathSearch()
        self._setSearchButton()
    
    def _connectSignals(self):
        """
        Connects the signals with the related slots.
        """
        self.connect(self.content.pathBox, SIGNAL("stateChanged(int)"), self._stateChanged)
        self.connect(self.content.addButton, SIGNAL("clicked()"), self._addDir)
        self.connect(self.content.removeButton, SIGNAL("clicked()"), self._removeDir)
        self.connect(self.content.cancelButton, SIGNAL("clicked()"), self.reject)
        self.connect(self.content.searchButton, SIGNAL("clicked()"), self.accept)
        
    def _setSearchButton(self):
        """
        Toggles the 'searchButton' evaluating the settings.
        """
        self.content.searchButton.setDefault(True)
        self.content.searchButton.setEnabled(self.content.pathBox.isChecked() or self.content.customDirList.count() != 0)
    
    def _populateDirList(self):
        """
        Fills the dir list with the directories stored in the QSettings.
        """
        search_dir_list = qvariant_converter.getStringList(QApplication.instance().settings.value("search_dir_list"))
        for element in search_dir_list:
            item = QListWidgetItem(element)
            self.content.customDirList.addItem(item)
    
    def _setPathSearch(self):
        """
        Sets the path search checkbox to the stored value.
        """
        pathSearch = qvariant_converter.getBool(QApplication.instance().settings.value(QString("path_search")))
        self.content.pathBox.setChecked(pathSearch)
    
    def _stateChanged(self, state):
        """
        Slot called when the path search checkbox state changes. Stores the value in the QSettings.
        """
        QApplication.instance().settings.setValue(QString("path_search"), QVariant(state != 0))
        self._setSearchButton()
    
    def _addDir(self):
        """
        Slot called when the user adds a dir.
        """
        directory = QFileDialog.getExistingDirectory(self, self.tr("Open Directory"), "", QFileDialog.ShowDirsOnly)
        if not directory.isEmpty():
            directory = unicode(directory)
            item = QListWidgetItem(directory)
            self.content.customDirList.addItem(item)
            search_dir_list = qvariant_converter.getStringList(QApplication.instance().settings.value("search_dir_list"))
            search_dir_list = set(search_dir_list + [directory])
            QApplication.instance().settings.setValue(QString("search_dir_list"), qvariant_converter.convertStringList(list(search_dir_list)))
            self._setSearchButton()
    
    def _removeDir(self):
        """
        Slot called when the user removes a dir.
        """
        if self.content.customDirList.currentRow() != -1:
            item = self.content.customDirList.takeItem(self.content.customDirList.currentRow())
            search_dir_list = qvariant_converter.getStringList(QApplication.instance().settings.value(QString("search_dir_list")))
            search_dir_list = set(search_dir_list)
            search_dir_list.remove(unicode(item.text()))
            QApplication.instance().settings.setValue(QString("search_dir_list"), qvariant_converter.convertStringList(list(search_dir_list)))
            self._setSearchButton()
    
