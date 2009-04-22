#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import os

import const

## Plugin interface ##

PLUGIN_NAME = "CodeLite"

PLUGIN_DESCRIPTION = "Create CodeLite project files"

def createProject(project_info):
    """
    Function that creates codelite projects and return the project relevant file.
    """
    directory = project_info.info("PROJECT_PATH")
    prjdir = directory + os.sep + os.path.basename(directory)
    workspace = codeliteWorkspaceGenerator(project_info)
    open(directory + os.sep + os.path.basename(prjdir) + ".workspace", "w").write(workspace)
    project = codeliteProjectGenerator(project_info)
    open(directory + os.sep + os.path.basename(prjdir) + ".project", "w").write(project)
    return directory + os.sep + os.path.basename(prjdir) + ".workspace"

####

def clFiles(file_dict, directory):
    """
    Creates the list of the lines for the files founded in file_dict, using
    directory as the base folder.
    """
    filelist = []
    filelist.append("<VirtualDirectory Name=\"%s\">" %os.path.basename(directory))
    for f in file_dict[directory]["files"]:
        filelist.append("<File Name=\"%s\"/>" %os.path.join(directory, f))
    for d in file_dict[directory]["dirs"]:
        filelist += clFiles(file_dict, os.path.join(directory, d))
    filelist.append("</VirtualDirectory>")
    return filelist

def findSources(path):
    """
    Analyzes the directory tree from path and return a dict with filename and
    path.
    """
    if not path.endswith(os.sep):
        path += os.sep
    file_dict = {}
    for root, dirs, files in os.walk(path):
        if root.find("svn") == -1:
            file_dict[root.replace(path, "")] = {"dirs": [], "files": []}
            for dir in dirs:
                if dir.find("svn") == -1:
                    file_dict[root.replace(path, "")]["dirs"].append(dir)
            for file in files:
                if file.endswith(const.EXTENSION_FILTER):
                    file_dict[root.replace(path, "")]["files"].append(file)
    return file_dict

def codeliteProjectGenerator(project_info):
    """
    Returns the string rapresenting the codelite project.
    """
    template = open("cltemplates/bertos.project", "r").read()
    filelist = "\n".join(clFiles(findSources(project_info.info("PROJECT_PATH")), ""))
    while template.find("$filelist") != -1:
        template = template.replace("$filelist", filelist)
    project_name = os.path.basename(project_info.info("PROJECT_PATH"))
    while template.find("$project") != -1:
        template = template.replace("$project", project_name)
    return template

def codeliteWorkspaceGenerator(project_info):
    """
    Returns the string rapresentig the codelite workspace.
    """
    template = open("cltemplates/bertos.workspace", "r").read()
    project_name = os.path.basename(project_info.info("PROJECT_PATH"))
    while template.find("$project") != -1:
        template = template.replace("$project", project_name)
    return template
