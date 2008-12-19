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

import os
import fnmatch

import const

def isBertosDir(directory):
   return os.path.exists(directory + "/VERSION")

def bertosVersion(directory):
   return open(directory + "/VERSION").readline().strip()

def createBertosProject(directory):
    if not os.path.isdir(directory):
        os.mkdir(directory)
    open(directory + "/project.bertos", "w")

def findDefinitions(ftype, path):
    L = os.walk(path)
    for element in L:
        for filename in element[2]:
            if fnmatch.fnmatch(filename, "*." + ftype):
                yield (filename, element[0])

def loadCpuInfos(path):
    cpuInfos = []
    for definition in findDefinitions(const.CPU_DEFINITION, path):
        D = {}
        D.update(const.CPU_DEF)
        def include(filename, dict = D, directory=definition[1]):
            execfile(directory + "/" + filename, {}, D)
        D["include"] = include
        include(definition[0], D)
        D["CPU_NAME"] = definition[0].split(".")[0]
        cpuInfos.append(D)
    return cpuInfos
