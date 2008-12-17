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

from BWizardPage import *

class BCpuPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "cpu_select.ui")
        self.setTitle(self.tr("Select the CPU"))
        