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
    def __init__(self, path, line_number, line):
        self.path = path
        self.line_number = line_number
        self.line = line
    
    def __str__(self):
        return repr(self.path)

class ModuleDefineException(DefineException):
    """
    Exception raised when an error occurs parsing the module informations.
    """
    def __init__(self, path, line_number, line):
        super(ModuleDefineException, self).__init__(path, line_number, line)

class EnumDefineException(DefineException):
    """
    Exception raised when an error occurs parsing the enum informations.
    """
    def __init__(self, path, line_number, line):
        super(EnumDefineException, self).__init__(path, line_number, line)

class ConfigurationDefineException(DefineException):
    """
    Exception raised when an error occurs parsing the configuration parameter informations.
    """
    def __init__(self, path, line_number, line):
        super(ConfigurationDefineException, self).__init__(path, line_number, line)
