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
        # TODO: see why this function call is needed for display the
        # application icons...
        uic.loadUiType(wizardGui)
        self.pageContent = uic.loadUi(wizardGui, None)
        layout = QVBoxLayout()
        layout.addWidget(self.pageContent)
        self.setLayout(layout)