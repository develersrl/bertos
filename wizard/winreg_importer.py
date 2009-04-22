#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
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
    CLI_KEY = OpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\Develer\BeRTOS SDK\Command Lines")
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