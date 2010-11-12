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

import copy

from PyQt4.QtCore import *
from PyQt4.QtGui import *

try:
    from version import wizard_version
except ImportError:
    wizard_version = "sandbox"

class BWizard(QWizard):
    """
    Main class of the wizard. It adds the pages automatically.
    """

    def __init__(self, page_list):
        self._current = None
        QWizard.__init__(self)
        # Hardcoded default position (to avoid (0,0) origin)
        geometry = QApplication.instance().settings.value("geometry", QVariant(QRect(QPoint(200, 200), QSize(800, 600))))
        if geometry:
            geometry = geometry.toRect()
            self.setGeometry(geometry)
        self.setWindowTitle(self.tr("Create a BeRTOS project - rev.%1").arg(wizard_version))
        self.setWindowIcon(QIcon(":/images/appicon.png"))
        self.setOption(QWizard.DisabledBackButtonOnLastPage, True)
        self.addPages(page_list)
        self.connectSignals()

    def addPages(self, page_list):
        """
        Adds the pages in the wizard.
        """
        self._page_dict = {}
        for i, page in enumerate(page_list):
            self._page_dict[page] = i
            self.setPage(i, page())

    def pageIndex(self, page_class):
        return self._page_dict[page_class]

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
        if page:
            page.reloadData(previous_id= self._current)
        self._current = pageId

    def project(self):
        """
        Returns the BProject associated with the wizard.
        """
        return copy.deepcopy(QApplication.instance().project)

    def done(self, result):
        geometry = self.geometry()
        QApplication.instance().settings.setValue("geometry", QVariant(geometry))
        QWizard.done(self, result)
