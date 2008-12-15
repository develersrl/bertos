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
import BStartPage
import BVersionPage

def main():
    app = QApplication(sys.argv)
    app.settings = QSettings("Develer", "Bertos Configurator")
    QResource.registerResource("bertos.rcc")
    startPage = BStartPage.BStartPage()
    wizard = QWizard()
    wizard.setWindowTitle("Welcome in BeRTOS configuration wizard")
    wizard.addPage(startPage)
    wizard.addPage(BVersionPage.BVersionPage())
    wizard.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()

