#!/usr/bin/env python
# encoding: utf-8
#
# This file is part of bertos.
#
# Slimqc is free software; you can redistribute it and/or modify
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
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
#
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

from PyQt4.QtCore import *

def getString(qvariant):
    if type(qvariant) != QVariant:
        return qvariant
    string = unicode(qvariant.toString())
    return string

def convertString(string):
    return QVariant(string)

def getStringList(qvariant):
    if type(qvariant) == QVariant:
        tmp = qvariant.toPyObject()
    else:
        tmp = qvariant
    string_list = []
    if tmp:
        string_list = [unicode(string) for string in tmp]
    return string_list

def convertStringList(string_list):
    return QVariant(string_list)

def getStringDict(qvariant):
    dict_str_str = {}
    try:
        for key, value in qvariant.toPyObject().items():
            dict_str_str[unicode(key)] = unicode(value)
    except:
        pass
    return dict_str_str

def convertStringDict(string_dict):
    return QVariant(string_dict)

def getBool(qvariant):
    return qvariant.toBool()

def convertBool(boolean):
    return QVariant(boolean)

def getBoolDict(qvariant):
    dict_str_bool = {}
    try:
        for key, value in qvariant.toPyObject().items():
            dict_str_bool[unicode(key)] = value
    except:
        pass
    return dict_str_bool

def convertBoolDict(dict_str_bool):
    return QVariant(dict_str_bool)

def getDict(qvariant):
    dict_str_variant = {}
    try:
        for key, value in qvariant.toMap().items():
            dict_str_variant[unicode(key)] = value
    except:
        pass
    return dict_str_variant

def convertDict(dict_str_variant):
    result_dict = {}
    for key, value in dict_str_variant.items():
        result_dict[QString(key)] = QVariant(value)
    return QVariant(result_dict)
