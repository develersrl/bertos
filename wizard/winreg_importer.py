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
# $Id: qvariant_converter.py 2506 2009-04-15 08:29:07Z duplo $
#
# Author: Lorenzo Berni <duplo@develer.com>
#

from _winreg import *

# Open the registry keys. When the keys don't exist it do nothing
try:
    DIR_KEY = OpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\Develer\BeRTOS SDK\BeRTOS Dirs")
except WindowsError:
    DIR_KEY = None

try:
    TOOLCHAIN_KEY = OpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\Develer\BeRTOS SDK\Toolchain Executables")
except WindowsError:
    TOOLCHAIN_KEY = None

try:
    CLI_KEY = OpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\Develer\BeRTOS SDK\Ide Command Lines")
except WindowsError:
    CLI_KEY = None

def getBertosDirs():
    """
    Returns the path of the BeRTOS versions installed by the BeRTOS SDK installer.
    """
    return getFromRegistry(DIR_KEY).values()

def getBertosToolchains():
    """
    Returns the path of the executables of the toolchains installed by the BeRTOS
    SDK installer.
    """
    return getFromRegistry(TOOLCHAIN_KEY).values()

def getCommandLines():
    """
    Returns the command lines to launch in order to open the selected IDE.
    """
    return getFromRegistry(CLI_KEY)

def getFromRegistry(key):
    """
    Returns the value of all the named values of the given key.
    """
    index = 0
    items = {}
    if key:
        while True:
            try:
                item = EnumValue(key, index)
                items[item[0]] = item[1]
                index += 1
            except WindowsError:
                break
    return items