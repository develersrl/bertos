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


class DefineException(Exception):
    """
    Exception raised when an error occurs parsing the module informations.
    """
    def __init__(self, value):
        self.parameter = value
    
    def __str__(self):
        return repr(self.parameter)

class ModuleDefineException(DefineException):
    """
    Exception raised when an error occurs parsing the module informations.
    """
    def __init__(self, value):
        super(ModuleDefineException, self).__init__(value)

class EnumDefineException(DefineException):
    """
    Exception raised when an error occurs parsing the enum informations.
    """
    def __init__(self, value):
        super(EnumDefineException, self).__init__(value)

class ConfigurationDefineException(DefineException):
    """
    Exception raised when an error occurs parsing the configuration parameter informations.
    """
    def __init__(self, value):
        super(ConfigurationDefineException, self).__init__(value)
