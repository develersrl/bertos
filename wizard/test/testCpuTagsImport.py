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

import sys
import os


sys.path.append("../")

from BProject import BProject
from bertos_utils import *

def main():
    p = BProject()
    p.setInfo("SOURCES_PATH", "../../")
    loadSourceTree(p)
    info = loadCpuInfos(p)
    tags = getTagSet(info)
    print tags


if __name__ == '__main__':
    main()

