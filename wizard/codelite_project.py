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

import const

def clFiles(fileDict, directory):
    filelist = []
    filelist.append("<VirtualDirectory Name=\"%s\">" %os.path.basename(directory))
    for f in fileDict[directory]["files"]:
        filelist.append("<File Name=\"%s\"/>" %os.path.join(directory, f))
    for d in fileDict[directory]["dirs"]:
        filelist += clFiles(fileDict, os.path.join(directory, d))
    filelist.append("</VirtualDirectory>")
    return filelist

def findSources(path):
    fileDict = {}
    for root, dirs, files in os.walk(path):
        if root.find("svn") == -1:
            fileDict[root] = {"dirs": [], "files": []}
            for dir in dirs:
                if dir.find("svn") == -1:
                    fileDict[root]["dirs"].append(dir)
            for file in files:
                if file.endswith(const.EXTENSION_FILTER):
                    fileDict[root]["files"].append(file)
    return fileDict
