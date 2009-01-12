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
    string_list = []
    for element in qvariant.toStringList():
        string_list.append(unicode(element))
    return string_list

def convertStringList(string_list):
    result = []
    for element in string_list:
        result.append(QString(element))
    return QVariant(QStringList(result))

def getStringDict(qvariant):
    dict_str_str = {}
    qvariant_item = qvariant.toPyObject()
    if qvariant_item == NotImplemented:
        qvariant_item = {}
    for key, value in qvariant_item.items():
        dict_str_str[unicode(key)] = unicode(value.toString())
    return dict_str_str

def convertStringDict(string_dict):
    result = {}
    for key, value in string_dict.items():
        result[QString(key)] = QVariant(QString(value))
    return QVariant(result)

def getBool(qvariant):
    return qvariant.toBool()

def convertBool(boolean):
    return QVariant(boolean)

def getBoolDict(qvariant):
    dict_str_bool = {}
    qvariant_item = qvariant.toPyObject()
    if qvariant_item == NotImplemented:
        qvariant_item = {}
    for key, value in qvariant_item.items():
        dict_str_bool[unicode(key)] = value.toBool()
    return dict_str_bool

def convertBoolDict(dict_str_bool):
    result = {}
    for key, value in dict_str_bool:
        result[QString(key)] = QVariant(value)
    return QVariant(result)

def getDict(qvariant):
    dict_str_variant = {}
    qvariant_item = qvariant.toPyObject()
    if qvariant_item == NotImplemented:
        qvariant_item = {}
    for key, value in qvariant_item.items():
        dict_str_variant[unicode(key)] = value
    return dict_str_variant

def convertDict(dict_str_variant):
    result = {}
    for key, value in dict_str_variant.items():
        if type(value) == str or type(value) == unicode:
            result[QString(key)] = QVariant(QString(value))
        elif type(value) == list:
            result[QString(key)] = QVariant(QStringList(value))
        elif type(value) == dict:
            result[QString(key)] = QVariant(convertDict(value))
    return QVariant(result)
