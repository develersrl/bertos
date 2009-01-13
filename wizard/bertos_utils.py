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
import glob
import re

import const

def isBertosDir(directory):
   return os.path.exists(directory + "/VERSION")

def bertosVersion(directory):
   return open(directory + "/VERSION").readline().strip()

def createBertosProject(directory):
    if not os.path.isdir(directory):
        os.mkdir(directory)
    open(directory + "/project.bertos", "w")

def getSystemPath():
    path = os.environ["PATH"]
    if os.name == "nt":
        path = path.split(";")
    else:
        path = path.split(":")
    return path

def findToolchains(pathList):
    toolchains = []
    for element in pathList:
        for toolchain in glob.glob(element+ "/" + const.GCC_NAME):
            if not os.path.islink(toolchain):
                toolchains.append(toolchain)
    return list(set(toolchains))

def getToolchainInfo(output):
    info = {}
    expr = re.compile("Target: .*")
    target = expr.findall(output)
    if len(target) == 1:
        info["target"] = target[0].split("Target: ")[1]
    expr = re.compile("gcc version [0-9,.]*")
    version = expr.findall(output)
    if len(version) == 1:
        info["version"] = version[0].split("gcc version ")[1]
    expr = re.compile("gcc version [0-9,.]* \(.*\)")
    build = expr.findall(output)
    if len(build) == 1:
        build = build[0].split("gcc version ")[1]
        build = build[build.find("(") + 1 : build.find(")")]
        info["build"] = build
    expr = re.compile("Configured with: .*")
    configured = expr.findall(output)
    if len(configured) == 1:
        info["configured"] = configured[0].split("Configured with: ")[1]
    expr = re.compile("Thread model: .*")
    thread = expr.findall(output)
    if len(thread) == 1:
        info["thread"] = thread[0].split("Thread model: ")[1]
    return info

def findDefinitions(ftype, path):
    L = os.walk(path)
    for element in L:
        for filename in element[2]:
            if fnmatch.fnmatch(filename, ftype):
                yield (filename, element[0])

def loadCpuInfos(path):
    cpuInfos = []
    for definition in findDefinitions(const.CPU_DEFINITION, path):
        cpuInfos.append(getInfos(definition))
    return cpuInfos

def getInfos(definition):
    D = {}
    D.update(const.CPU_DEF)
    def include(filename, dict = D, directory=definition[1]):
        execfile(directory + "/" + filename, {}, D)
    D["include"] = include
    include(definition[0], D)
    D["CPU_NAME"] = definition[0].split(".")[0]
    D["DEFINITION_PATH"] = definition[1] + "/" + definition[0]
    del D["include"]
    return D
