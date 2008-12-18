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

import sys

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import BProject

import BStartPage
import BWizard

def newProject():
    wizard = BWizard.BWizard()
    wizard.show()
    wizard.exec_()
    
def editProject():
    print "editProject"

def showStartPage():
    QApplication.instance().dialog = BStartPage.BStartPage()
    QApplication.instance().connect(QApplication.instance().dialog, SIGNAL("newProject"), newProject)
    QApplication.instance().connect(QApplication.instance().dialog, SIGNAL("editProject"), editProject)
    QApplication.instance().dialog.show()

def main():
    app = QApplication(sys.argv)
    app.settings = QSettings("Develer", "Bertos Configurator")
    app.project = BProject.BProject()
    QResource.registerResource("bertos.rcc")
    showStartPage()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()

