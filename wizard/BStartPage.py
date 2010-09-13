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

from PyQt4.QtCore import *
from PyQt4.QtGui import *
import PyQt4.uic as uic

from const import *

class BStartPage(QDialog):

    def __init__(self):
        QDialog.__init__(self)
        self.setupUi()
        self.connectSignals()
        self.setWindowTitle(self.tr("Create or edit a BeRTOS project"))
        self.initializeButtons()

    def setupUi(self):
        self.content = uic.loadUi(UI_LOCATION + "/start.ui", None)
        self.setWindowIcon(QIcon(":/images/appicon.png"))
        layout = QVBoxLayout()
        layout.addWidget(self.content)
        self.setLayout(layout)

    def connectSignals(self):
        self.connect(self.content.newButton, SIGNAL("clicked()"), self.newProject)
        self.connect(self.content.editButton, SIGNAL("clicked()"), self.editProject)

    def initializeButtons(self):
        self.button_group = QButtonGroup()
        self.button_group.addButton(self.content.newButton)
        self.button_group.addButton(self.content.editButton)
        self.button_group.setExclusive(True)

    def newProject(self):
        self.close()
        self.emit(SIGNAL("newProject"))

    def editProject(self):
        self.close()
        self.emit(SIGNAL("editProject"))
