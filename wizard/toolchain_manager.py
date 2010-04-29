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

import os

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import qvariant_converter
from toolchain_validation import validateToolchain


class ToolchainManager(object):
    def __init__(self):
        self._app_settings = QApplication.instance().settings

    def validateToolchain(self, toolchain):
        toolchains = self.storedToolchainDict()
        if toolchain in toolchains:
            toolchains[toolchain] = True
            self.setStoredToolchainDict(toolchains)
            return self._validateToolchain(toolchain)
        elif toolchain in self._predefined_toolchain_set:
            return self._validateToolchain(toolchain)
        else:
            return None

    def _validateToolchain(self, toolchain):
        """
        Returns information about the toolchain located in path
        "toolchain". If the toolchain is not recognized, or it doesn't
        exists anymore a empty dict is returned.

        Example of toolchain information dict:
        {
            "target": "arm",
            "version": "4.0.6",
            "build": None,
            "configured": None,
            "thread": None,
        }
        """
        return validateToolchain(toolchain)

    def addToolchain(self, toolchain, validated=False):
        if toolchain not in self.predefined_toolchains:
            toolchains = self.storedToolchainDict()
            toolchains[toolchain] = validated
            self.setStoredToolchainDict(toolchains)

    def removeToolchain(self, toolchain):
        toolchains = self.storedToolchainDict()
        if toolchain in toolchains:
            del toolchains[toolchain]
        self.setStoredToolchainDict(toolchains)

    def storedToolchainDict(self):
        toolchains = self._app_settings.value("toolchains", QVariant())
        toolchains = qvariant_converter.getBoolDict(toolchains)
        return toolchains

    def setStoredToolchainDict(self, toolchain_dict):
        toolchains = qvariant_converter.convertBoolDict(toolchain_dict)
        self._app_settings.setValue("toolchains", toolchains)

    @property
    def toolchains(self):
        toolchains = []
        toolchain_dict = self.storedToolchainDict()
        for toolchain, validated in toolchain_dict.items():
            if validated:
                information = self._validateToolchain(toolchain)
            else:
                information = None
            toolchains.append((toolchain, information))
        return toolchains

    @property
    def predefined_toolchains(self):
        toolchains = []
        stored_toolchains = self._predefined_toolchain_set
        for toolchain in stored_toolchains:
            toolchains.append((toolchain, self._validateToolchain(toolchain)))
        return toolchains

    @property
    def _predefined_toolchain_set(self):
        stored_toolchains = set()
        if os.name == "nt":
            import winreg_importer
            stored_toolchains |= set(winreg_importer.getBertosToolchains())
        return stored_toolchains

    def suitableToolchains(self, target):
        toolchains = self.toolchains
        suitable_toolchains = []
        for name, info in toolchains:
            t = info.get("target", None)
            if t and t.find(target) != -1:
                suitable_toolchains.append(name)
        return suitable_toolchains

