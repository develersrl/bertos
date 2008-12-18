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

from PyQt4.QtGui import *

import BFolderPage
import BVersionPage
import BCpuPage

class BWizard(QWizard):
    
    def __init__(self):
        QWizard.__init__(self)
        self.setWindowTitle("Create a BeRTOS project")
        self.addPage(BFolderPage.BFolderPage())
        self.addPage(BVersionPage.BVersionPage())
        self.addPage(BCpuPage.BCpuPage())