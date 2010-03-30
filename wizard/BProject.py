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

import os
import fnmatch
import copy

class BProject(object):
    """
    Simple class for store and retrieve project informations.
    """
    
    def __init__(self):
        self.infos = {}
        self._cached_queries = {}
    
    def setInfo(self, key, value):
        """
        Store the given value with the name key.
        """
        self.infos[key] = value
    
    def info(self, key, default=None):
        """
        Retrieve the value associated with the name key.
        """
        if key in self.infos:
            return copy.deepcopy(self.infos[key])
        return default

    def loadSourceTree(self):
        # Index only the SOURCES_PATH/bertos content
        bertos_sources_dir = os.path.join(self.info("SOURCES_PATH"), 'bertos')
        if os.path.exists(bertos_sources_dir):
            fileList = [f for f in os.walk(bertos_sources_dir)]
        else:
            fileList = []
        self.setInfo("FILE_LIST", fileList)

    def findDefinitions(self, ftype):
        definitions = self._cached_queries.get(ftype, None)
        if definitions is not None:
            return definitions
        L = self.infos["FILE_LIST"]
        definitions = []
        for element in L:
            for filename in element[2]:
                if fnmatch.fnmatch(filename, ftype):
                    definitions.append((filename, element[0]))
        self._cached_queries[ftype] = definitions
        return definitions

    def __repr__(self):
        return repr(self.infos)