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
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import sys
import os

from PyQt4.QtCore import *
from PyQt4.QtGui import *

from bertos_utils import loadBertosProject
import BModulePage

class BEditingDialog(QDialog):

    def __init__(self, parent=None):
        QDialog.__init__(self, parent)
        self.setupUi()
        self.module_page.reloadData()
    
    def setupUi(self):
        layout = QVBoxLayout()
        self.module_page = BModulePage.BModulePage()
        layout.addWidget(self.module_page)
        self.setLayout(layout)



def main():
    if len(sys.argv) > 1:
        project_file = sys.argv[1]
    else:
        print "Invalid usage: use <command> project_file"
        sys.exit()
    app = QApplication([])
    app.project = loadBertosProject(project_file)
    dialog = BEditingDialog()
    dialog.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()
