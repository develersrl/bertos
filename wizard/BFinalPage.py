#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id:$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import os

from PyQt4.QtGui import *

from BWizardPage import *
import bertos_utils

from const import *

class BFinalPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/final_page.ui")
        self.setTitle(self.tr("Project created successfully"))
        
    def reloadData(self):
        QApplication.instance().setOverrideCursor(Qt.WaitCursor)
        bertos_utils.createBertosProject(self.wizard().project())
        QApplication.instance().restoreOverrideCursor()