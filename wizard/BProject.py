#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import copy

class BProject(object):
    """
    Simple class for store and retrieve project informations.
    """
    
    def __init__(self):
        self.infos = {}
    
    def setInfo(self, key, value):
        """
        Store the given value with the name key.
        """
        self.infos[key] = value
    
    def info(self, key):
        """
        Retrieve the value associated with the name key.
        """
        if key in self.infos:
            return copy.deepcopy(self.infos[key])
        return None
    
    def __repr__(self):
        return repr(self.infos)