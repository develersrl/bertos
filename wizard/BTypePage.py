#!/usr/bin/env python
# encoding: utf-8
#
# This file is part of slimqc.
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
# Copyright 2010 Develer S.r.l. (http://www.develer.com/)
#
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

from PyQt4.QtCore import *
from PyQt4.QtGui import *

from BWizardPage import BWizardPage

from BBoardPage import BBoardPage
from BCpuPage import BCpuPage

from const import UI_LOCATION

class BTypePage(BWizardPage):
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/project_type_select.ui")
        self.setTitle(self.tr("Select the project type"))

    ## Overloaded QWizardPage methods ##

    def isComplete(self):
        self.setProjectInfo("PROJECT_FROM_PRESET", self.from_preset)
        return True

    def nextId(self):
        if self.from_preset:
            return self.wizard().pageIndex(BBoardPage)
        else:
            return self.wizard().pageIndex(BCpuPage)

    ####

    ## Overloaded BWizardPage methods ##

    def connectSignals(self):
        self.connect(self.pageContent.predefinedButton, SIGNAL("toggled(bool)"), self, SIGNAL("completeChanged()"))

    def reloadData(self):
        self.project.loadProjectPresets()
        self.pageContent.predefinedButton.setEnabled(len(self.has_presets) > 0)
        self.pageContent.predefinedButton.setChecked(len(self.has_presets) > 0)
        self.pageContent.customButton.setChecked(len(self.has_presets) == 0)

    ####

    @property
    def from_preset(self):
        return self.pageContent.predefinedButton.isChecked()

    @property
    def has_presets(self):
        preset_tree = self.project.info("PRESET_TREE")
        return isinstance(preset_tree, dict) and preset_tree.get("children", [])