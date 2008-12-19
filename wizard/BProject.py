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
        self.infos = {}
    
    def setInfo(self, key, value):
        self.infos[key] = value
    
    def info(self, key):
        if key in self.infos.keys():
            return self.infos[key]
        return None