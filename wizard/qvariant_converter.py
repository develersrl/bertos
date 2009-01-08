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

def getStringList(qvariant):
    string_list = [unicode(string) for string in qvariant.toStringList()]
    return string_list

def getStringDict(qvariant):
    dict_str_str = {}
    for key, value in qvariant.toMap().items():
        dict_str_str[unicode(key)] = unicode(value.toString())
    return dict_str_str

def getBool(qvariant):
    return qvariant.toBool()

def getDict(qvariant):
    dict_str_variant = {}
    for key, value in qvariant.toMap().items():
        dict_str_variant[unicode(key)] = value
    return dict_str_variant