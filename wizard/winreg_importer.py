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

DIR_KEY = OpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\Develer\BeRTOS SDK\BeRTOS Dirs")

def getBertosDirs():
    index = 0
    dirs = []
    while True:
        try:
            dir = EnumValue(DIR_KEY, index)[1]
            dirs.append(dir)
            index += 1
        except WindowsError:
            break
    return dirs
       