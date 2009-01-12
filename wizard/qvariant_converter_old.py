#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id:$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

from PyQt4.QtCore import *

def getString(qvariant):
    string = qvariant.toPyObject()
    return string

def convertString(string):
    return QVariant(string)

def getStringList(qvariant):
    string_list = qvariant.toPyObject()
    return string_list

def convertStringList(string_list):
    return QVariant(string_list)

def getStringDict(qvariant):
    dict_str_str = qvariant.toPyObject()
    return dict_str_str

def convertStringDict(string_dict):
    return QVariant(string_dict)

def getBool(qvariant):
    return qvariant.toBool()

def convertBool(boolean):
    return QVariant(boolean)

def getBoolDict(qvariant):
    dict_str_bool = qvariant.toPyObject()
    return dict_str_bool

def convertBoolDict(dict_str_bool):
    return QVariant(dict_str_bool)

def getDict(qvariant):
    dict_str_variant = qvariant.toPyObject()
    return dict_str_variant

def convertDict(dict_str_variant):
    return QVariant(dict_str_variant)
