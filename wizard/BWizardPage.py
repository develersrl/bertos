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

class BWizardPage(QWizardPage):
    
    def __init__(self, wizardGui, parent = None):
        QWizardPage.__init__(self, parent)
        self.pageContent = uic.loadUi(wizardGui, None)
        layout = QVBoxLayout()
        layout.addWidget(self.pageContent)
        self.setLayout(layout)
    
    def _settingsStore(self, key, value):
        QApplication.instance().settings.setValue(QString(key), QVariant(value))
    
    def _settingsRetrieve(self, key):
        return QApplication.instance().settings.value(QString(key), QVariant())
    
    def _projectInfoStore(self, key, value):
        QApplication.instance().project.setInfo(key, value)
    
    def _projectInfoRetrieve(self, key):
        return QApplication.instance().project.info(key)