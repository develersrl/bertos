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
