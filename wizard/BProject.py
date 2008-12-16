#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id:$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

class BProject(object):
    
    def __init__(self):
        pass
    
    def openProject(self, path):
        # Fill the BProject fields, opening an existing project
        pass
    
    def setProjectPath(self, path):
        self.projectPath = path
    
    def setSourcePath(self, path):
        self.sourcePath = path
    
    