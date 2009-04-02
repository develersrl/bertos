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

def clFiles(file_dict, directory):
    filelist = []
    filelist.append("<VirtualDirectory Name=\"%s\">" %os.path.basename(directory))
    for f in file_dict[directory]["files"]:
        filelist.append("<File Name=\"%s\"/>" %os.path.join(directory, f))
    for d in file_dict[directory]["dirs"]:
        filelist += clFiles(file_dict, os.path.join(directory, d))
    filelist.append("</VirtualDirectory>")
    return filelist

def findSources(path):
    file_dict = {}
    for root, dirs, files in os.walk(path):
        if root.find("svn") == -1:
            file_dict[root] = {"dirs": [], "files": []}
            for dir in dirs:
                if dir.find("svn") == -1:
                    file_dict[root]["dirs"].append(dir)
            for file in files:
                if file.endswith(const.EXTENSION_FILTER):
                    file_dict[root]["files"].append(file)
    return file_dict

def codeliteProjectGenerator(project_info):
    template = open("cltemplates/bertos.project").read()
    filelist = "\n".join(clFiles(findSources(project_info.info("PROJECT_PATH")), project_info.info("PROJECT_PATH")))
    while template.find("$filelist") != -1:
        template = template.replace("$filelist", filelist)
    project_name = os.path.basename(project_info.info("PROJECT_PATH"))
    while template.find("$project") != -1:
        template = template.replace("$project", project_name)
    return template

def codeliteWorkspaceGenerator(project_info):
    template = open("cltemplates/bertos.workspace").read()
    project_name = os.path.basename(project_info.info("PROJECT_PATH"))
    while template.find("$project") != -1:
        template = template.replace("$project", project_name)
    return template
