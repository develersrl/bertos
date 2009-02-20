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

import sys
import os

from PyQt4.QtGui import *

# Add the wizard class path in the path for the testing module
sys.path.append("../")

import BProject
import BModulePage
import bertos_utils

# Add a custom UI_LOCATION constant
BModulePage.UI_LOCATION = "../ui"

def main():
    app = QApplication([])
    app.project = BProject.BProject()
    page = BModulePage.BModulePage()
    page._projectInfoStore("SOURCES_PATH", "../../")
    page._projectInfoStore("CPU_INFOS", {"TOOLCHAIN": "avr", "CPU_TAGS": []})
    bertos_utils.loadSourceTree(page._project())
    page.reloadData()
    page.show()
    app.exec_()

if __name__ == '__main__':
    main()

