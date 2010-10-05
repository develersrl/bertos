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
import sys
from distutils.dep_util import newer

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import exception_handler

from BProject import BProject

from BWizard import BWizard

from BIntroPage import BIntroPage
from BFolderPage import BFolderPage
from BBoardPage import BBoardPage
from BProjectPresets import BProjectPresets
from BOpenPage import BOpenPage
from BVersionPage import BVersionPage
from BCpuPage import BCpuPage
from BToolchainPage import BToolchainPage
from BModulePage import BModulePage
from BCreationPage import BCreationPage
from BFinalPage import BFinalPage

from BEditingDialog import BEditingDialog, BVersionDialog, BToolchainDialog

from const import DATA_DIR

from LoadException import VersionException, ToolchainException

def newProject():
    QApplication.instance().project = BProject()
    page_list = [BIntroPage, BFolderPage, BVersionPage, BBoardPage, BProjectPresets, BCpuPage, BToolchainPage, BModulePage, BCreationPage, BFinalPage]
    wizard = BWizard(page_list)
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
            QProcess.startDetached(command_line, [relevant_file,])
    sys.exit()

def editProject(project_file):
    info_dict = {}
    # Progress dialog.
    p_dialog = QProgressDialog(QApplication.instance().tr("Wait project loading"), QApplication.instance().tr("Cancel"), 0, 0)
    while(True):
        p_dialog.show()
        # Empty project is the default fallback.
        QApplication.instance().project = BProject()
        try:
            QApplication.instance().project = BProject(project_file, info_dict)
        except VersionException:
            p_dialog.hide()
            QMessageBox.critical(
                None,
                QObject().tr("BeRTOS version not found!"),
                QObject().tr("The selected BeRTOS version was not found, please select another one...")
            )
            dialog = BVersionDialog()
            if dialog.exec_():
                version = dialog.version_page.currentVersion()
                info_dict["BERTOS_PATH"] = version
            continue
        except ToolchainException, exc:
            p_dialog.hide()
            QMessageBox.critical(
                None,
                QObject().tr("Toolchain not found!"),
                QObject().tr("The selected toolchain was not found, please select another one...")
            )
            QApplication.instance().project = exc.partial_project
            dialog = BToolchainDialog()
            if dialog.exec_():
                toolchain = dialog.toolchain_page.currentToolchain()
                info_dict["TOOLCHAIN"] = toolchain
            continue
        break
    dialog = BEditingDialog()
    p_dialog.hide()
    dialog.exec_()

def main():
    app = QApplication(sys.argv)
    app.settings = QSettings("Develer", "Bertos Configurator")
    # Development utility lines, to be removed for production
    datadir = DATA_DIR

    # Something seems to not work, on Windows, using pyrcc4 with BeRTOS Wizard
    # resources. So I'm restoring the old rcc-based resource generation
    # system.
    #
    # qrc, bertos_rc = os.path.join(datadir, 'bertos.qrc'), os.path.join(datadir, 'bertos_rc.py')
    # if not (hasattr(sys, "frozen") and sys.frozen) and newer(qrc, bertos_rc):
    #     os.system("pyrcc4 \"%s\" -o \"%s\"" %(qrc, bertos_rc))
    # import bertos_rc

    qrc, rcc = os.path.join(datadir, 'bertos.qrc'), os.path.join(datadir, 'bertos.rcc') 
    if not (hasattr(sys, "frozen") and sys.frozen) and newer(qrc, rcc): 
        os.system("rcc -binary \"%s\" -o \"%s\"" %(qrc, rcc)) 
    QResource.registerResource(rcc) 
    
    if len(sys.argv) == 3 and sys.argv[1] == "--edit":
        editProject(os.path.abspath(sys.argv[2]))
    else:
        newProject()


if __name__ == '__main__':
    main()

