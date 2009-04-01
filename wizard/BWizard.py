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

import copy

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import BFolderPage
import BVersionPage
import BCpuPage
import BToolchainPage
import BModulePage
import BOutputPage
import BCreationPage
import BFinalPage

class BWizard(QWizard):
    """
    Main class of the wizard. It adds the pages automatically.
    """
    
    def __init__(self, page_list):
        QWizard.__init__(self)
        self.setWindowTitle(self.tr("Create a BeRTOS project"))
        self.setOption(QWizard.DisabledBackButtonOnLastPage, True)
        self.addPages(page_list)
        self.connectSignals()
    
    def addPages(self, page_list):
        """
        Adds the pages in the wizard.
        """
        for page in page_list:
            self.addPage(page())

    def connectSignals(self):
        """
        Connects the signals with the related slots.
        """
        self.connect(self, SIGNAL("currentIdChanged(int)"), self.pageChanged)
    
    def pageChanged(self, pageId):
        """
        Slot called when the user change the current page. It calls the reloadData
        method of the next page.
        """
        page = self.page(pageId)
        if page is not None:
            page.reloadData()
    
    def project(self):
        """
        Returns the BProject associated with the wizard.
        """
        return copy.deepcopy(QApplication.instance().project)
