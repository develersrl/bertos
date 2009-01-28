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

import os

from PyQt4.QtGui import *
from BWizardPage import *
import bertos_utils


class BOutputPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "output_select.ui")
        self.setTitle(self.tr("Choose the project output"))
