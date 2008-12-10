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

class BStartPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "start.ui")
        self._connectSignals()
        self.setTitle(self.tr("Create or edit a beRTOS project"))
        
    def _connectSignals(self):
        self.connect(self.pageContent.newButton, SIGNAL("clicked()"), lambda: self.mutualExclusion("new"))
        self.connect(self.pageContent.editButton, SIGNAL("clicked()"), lambda: self.mutualExclusion("edit"))
        
    def mutualExclusion(self, button):
        if(button == "new"):
            self.pageContent.newButton.setChecked(True)
            self.pageContent.editButton.setChecked(False)
        elif(button == "edit"):
            self.pageContent.newButton.setChecked(False)
            self.pageContent.editButton.setChecked(True)