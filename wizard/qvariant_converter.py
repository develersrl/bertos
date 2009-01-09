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
    string = unicode(qvariant.toString())
    return string

def convertString(string):
    return QVariant(string)

def getStringList(qvariant):
    string_list = [unicode(string) for string in qvariant.toStringList()]
    return string_list

def convertStringList(string_list):
    return QVariant(string_list)

def getStringDict(qvariant):
    dict_str_str = {}
    for key, value in qvariant.toMap().items():
        dict_str_str[unicode(key)] = unicode(value.toString())
    return dict_str_str

def convertStringDict(string_dict):
    result_dict = {}
    for key, value in string_dict.items():
        result_dict[QString(key)] = QString(value)
    return QVariant(result_dict)

def getBool(qvariant):
    return qvariant.toBool()

def convertBool(boolean):
    return QVariant(boolean)

def getBoolDict(qvariant):
    dict_str_bool = {}
    for key, value in qvariant.toMap().items():
        dict_str_bool[unicode(key)] = value.toBool()
    return dict_str_bool

def convertBoolDict(dict_str_bool):
    result_dict = {}
    for key, value in dict_str_bool.items():
        result_dict[QString(key)] = value
    return QVariant(result_dict)

def getDict(qvariant):
    dict_str_variant = {}
    for key, value in qvariant.toMap().items():
        dict_str_variant[unicode(key)] = value
    return dict_str_variant

def convertDict(dict_str_variant):
    result_dict = {}
    for key, value in dict_str_variant.items():
        result_dict[QString(key)] = value
    return QVariant(result_dict)