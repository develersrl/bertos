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
import bertos_utils

class BCpuPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, "cpu_select.ui")
        self.setTitle(self.tr("Select the CPU"))
    
    def _populateCpuList(self):
        cpus = bertos_utils.findDefinitions("cdef", self._projectInfoRetrieve("SOURCES_PATH"))
        for cpu in cpus.keys():
            self.pageContent.cpuList.addItem(QListWidgetItem(cpu))
    
    def reloadData(self):
        self._populateCpuList()
        