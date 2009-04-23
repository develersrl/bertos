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

"""
Awful module for the conversion from python types to qvariant, for make the wizard compatible with older version of pyqt (<= 4.4.3)
"""

from PyQt4.QtCore import *
import pickle

def getString(qvariant):
    if type(qvariant) == str or type(qvariant) == unicode:
        string = qvariant
    else:
        string = unicode(qvariant.toString())
    return string

def convertString(string):
    return QVariant(string)

def getStringList(qvariant):
    string_list = []
    if type(qvariant) == list:
        string_list = qvariant
    else:
        for element in qvariant.toStringList():
            string_list.append(unicode(element))
    return string_list

def convertStringList(string_list):
    result = []
    for element in string_list:
        result.append(QString(element))
    return QVariant(QStringList(result))

def getStringDict(qvariant):
    a = str(qvariant.toByteArray())
    if len(a) == 0:
        dict_str_str = {}
    else:
        dict_str_str = pickle.loads(a)
    return dict_str_str

def convertStringDict(dict_str_str):
    a = pickle.dumps(dict_str_str)
    return QVariant(QByteArray(a))

def getBool(qvariant):
    return qvariant.toBool()

def convertBool(boolean):
    return QVariant(boolean)

def getBoolDict(qvariant):
    a = str(qvariant.toByteArray())
    if len(a) == 0:
        dict_str_bool = {}
    else:
        dict_str_bool = pickle.loads(a)
    return dict_str_bool

def convertBoolDict(dict_str_bool):
    a = pickle.dumps(dict_str_bool)
    return QVariant(QByteArray(a))

def getDict(qvariant):
    a = str(qvariant.toByteArray())
    if len(a) == 0:
        dict_str_bool = {}
    else:
        dict_str_bool = pickle.loads(a)
    return dict_str_bool

def convertDict(dict_str_variant):
    a = pickle.dumps(dict_str_variant)
    return QVariant(QByteArray(a))
