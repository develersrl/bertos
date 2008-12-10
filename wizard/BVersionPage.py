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

class BVersionPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "bertos_versions.ui")
        self._connectSignals()
        self._setupUi()
        self.setTitle(self.tr("Select the beRTOS version needed"))
    
    def _connectSignals(self):
        pass
    
    def _setupUi(self):
        self.pageContent.updateProgressBar.setVisible(False)
        self.pageContent.statusLabel.setVisible(False)