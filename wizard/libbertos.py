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

from PyQt4.QtCore import *

def isBertosDir(directory):
    version_file = directory + QDir.separator() + "VERSION"
    qfile = QFile(version_file)
    return qfile.exists()

def bertosVersion(directory):
    version_file = directory + QDir.separator() + "VERSION"
    qfile = QFile(version_file)
    qfile.open(QIODevice.ReadOnly)
    file = QTextStream(qfile)
    return file.readLine()