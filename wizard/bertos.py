#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import os
import sys
from distutils.dep_util import newer

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import BProject

import BStartPage
import BWizard

from BFolderPage import BFolderPage
from BOpenPage import BOpenPage
from BVersionPage import BVersionPage
from BCpuPage import BCpuPage
from BToolchainPage import BToolchainPage
from BModulePage import BModulePage
from BOutputPage import BOutputPage
from BCreationPage import BCreationPage
from BFinalPage import BFinalPage

import bertos_utils

def newProject():
    page_list = [BFolderPage, BVersionPage, BCpuPage, BToolchainPage, BModulePage, BOutputPage, BCreationPage, BFinalPage]
    wizard = BWizard.BWizard(page_list)
    wizard.show()
    wizard.exec_()
    project = QApplication.instance().project
    to_be_opened = project.info("TO_BE_OPENED")
    command_lines = project.info("COMMAND_LINES")
    relevant_files = project.info("RELEVANT_FILES")
    if to_be_opened:
        for ide in to_be_opened:
            command_line = command_lines[ide]
            relevant_file = relevant_files[ide]
            import subprocess
            subprocess.call(command_line + " \"" + relevant_file + "\"")
    sys.exit()
    
def editProject():
    page_list = [BOpenPage, BVersionPage, BCpuPage, BToolchainPage, BModulePage, BOutputPage, BCreationPage, BFinalPage]
    wizard = BWizard.BWizard(page_list)
    wizard.show()
    wizard.exec_()

def showStartPage():
    QApplication.instance().dialog = BStartPage.BStartPage()
    QApplication.instance().connect(QApplication.instance().dialog, SIGNAL("newProject"), newProject)
    QApplication.instance().connect(QApplication.instance().dialog, SIGNAL("editProject"), editProject)
    QApplication.instance().dialog.show()

def main():
    os.chdir(os.path.dirname(os.path.abspath(sys.argv[0])))
    app = QApplication(sys.argv)
    app.settings = QSettings("Develer", "Bertos Configurator")
    app.project = BProject.BProject()
    # Development utility lines, to be removed for production
    if not (hasattr(sys, "frozen") and sys.frozen) and newer("bertos.qrc", "bertos.rcc"):
        os.system("rcc -binary bertos.qrc -o bertos.rcc")
    QResource.registerResource("bertos.rcc")
    if "--create" in sys.argv and "--edit" not in sys.argv:
        newProject()
    elif "--edit" in sys.argv and "--create" not in sys.argv:
        editProject()
    elif "--create" in sys.argv and "--edit" in sys.argv:
        # TODO need an explaining message
        print " ".join(sys.argv)
        print "Invalid usage!"
        pass
    else:
        newProject()

if __name__ == '__main__':
    main()

