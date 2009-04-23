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
