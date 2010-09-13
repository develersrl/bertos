#!/usr/bin/env python
# encoding: utf-8
#
# This file is part of BeRTOS.
#
# Bertos is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this
# file does not by itself cause the resulting executable to be covered by
# the GNU General Public License.  This exception does not however
# invalidate any other reasons why the executable file might be covered by
# the GNU General Public License.
#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
#
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
    
    def reloadData(self, previous_id=None):
        """
        Overload of the BWizardPage reloadData method.
        """
        project = unicode(QFileDialog.getOpenFileName(self, self.tr("Open project file"), os.path.expanduser("~"), self.tr("Project file (project.bertos)")))
        if project == "":
            QApplication.instance().quit()
        else:
            QApplication.instance().project = pickle.loads(open(project, "r").read())
            self.pageContent.nameLabel.setText(os.path.basename(project.replace(os.sep + "project.bertos", "")))
            self.pageContent.dirLabel.setText(project)
    
    ####
    
