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

def getBertosDirs():
    """
    Returns the path of the BeRTOS versions installed by the BeRTOS SDK installer.
    """
    return getFromRegistry(DIR_KEY)

def getBertosToolchain():
    """
    Returns the path of the executables of the toolchains installed by the BeRTOS
    SDK installer.
    """
    return getFromRegistry(TOOLCHAIN_KEY)

def getFromRegistry(key):
    """
    Returns the value of all the named values of the given key.
    """
    index = 0
    items = []
    if key:
        while True:
            try:
                item = EnumValue(key, index)[1]
                items.append(item)
                index += 1
            except WindowsError:
                break
    return items