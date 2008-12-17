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
from BWizardPage import *
import bertos_utils

class BFolderPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "dir_select.ui")
        self.setTitle(self.tr("Select the BeRTOS version needed"))
        self._folder = ""
        self._connectSignals()
    
    def _connectSignals(self):
        self.connect(self.pageContent.nameEdit, SIGNAL("textChanged(con QString)"), self._nameChanged)
    
    def _nameChanged(self, name):
        print "name changed"