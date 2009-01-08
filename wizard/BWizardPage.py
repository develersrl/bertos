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

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic

import qvariant_converter

class BWizardPage(QWizardPage):
    
    def __init__(self, wizardGui, parent = None):
        QWizardPage.__init__(self, parent)
        self.pageContent = uic.loadUi(wizardGui, None)
        layout = QVBoxLayout()
        layout.addWidget(self.pageContent)
        self.setLayout(layout)
    
    def _settingsStore(self, key, value):
        QApplication.instance().settings.setValue(QString(key), value)
    
    def _settingsRetrieve(self, key):
        return QApplication.instance().settings.value(QString(key), QVariant())
    
    def _projectInfoStore(self, key, value):
        QApplication.instance().project.setInfo(key, value)
    
    def _projectInfoRetrieve(self, key):
        return QApplication.instance().project.info(key)

    def versions(self):
        return qvariant_converter.getStringList(self._settingsRetrieve("versions"))
    
    def setVersions(self, versions):
        self._settingsStore("versions", qvariant_converter.convertStringList(versions))
        
    def searchDirList(self):
        return qvariant_converter.getStringList(self._settingsRetrieve("search_dir_list"))
    
    def setSearchDirList(self, search_dir_list):
        self._settingsStore("search_dir_list", qvariant_converter.convertStringList(search_dir_list))
    
    def pathSearch(self):
        return qvariant_converter.getBool(self._settingsRetrieve("path_search"))
    
    def setPathSearch(self, path_search):
        self._settingsStore("path_search", qvariant_converter.convertBool(path_search))
    
    def toolchains(self):
        return qvariant_converter.getStringList(self._settingsRetrieve("toolchains"))

    def setToolchains(self, toolchains):
        self._settingsStore("toolchains", qvariant_converter.convertStringList(toolchains))
    
    def reloadData(self):
        pass