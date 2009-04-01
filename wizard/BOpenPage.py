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
import pickle

from PyQt4.QtGui import *
from PyQt4.QtCore import *

from BWizardPage import *
import bertos_utils

from const import *

class BOpenPage(BWizardPage):
    """
    Initial page of the wizard. Permit to select the project name and the directory
    where the project will be created.
    """
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/project_select.ui")
        self.setTitle(self.tr("Open an existing BeRTOS project"))
    
    ## Overloaded BWizardPage methods ##
    
    def reloadData(self):
        project = unicode(QFileDialog.getOpenFileName(self, self.tr("Open project file"), os.path.expanduser("~"), self.tr("Project file (project.bertos)")))
        if project == "":
            QApplication.instance().quit()
        else:
            QApplication.instance().project = pickle.loads(open(project, "r").read())
            self.pageContent.nameLabel.setText(os.path.basename(project.replace(os.sep + "project.bertos", "")))
            self.pageContent.dirLabel.setText(project)
    
    ####
    
