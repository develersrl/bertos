#!/usr/bin/env python
# encoding: utf-8
#
# This file is part of BeRTOS.
#
# Bertos is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this
# file does not by itself cause the resulting executable to be covered by
# the GNU General Public License.  This exception does not however
# invalidate any other reasons why the executable file might be covered by
# the GNU General Public License.
#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
#
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
    # Do not create an empty VDir.
    # TODO: this is *really* ugly, but an empty VDir is worse
    if directory:
        filelist.append("<VirtualDirectory Name=\"%s\">" %os.path.basename(directory))
    for f in file_dict[directory]["files"]:
        filelist.append("<File Name=\"%s\"/>" %os.path.join(directory, f))
    for d in file_dict[directory]["dirs"]:
        filelist += clFiles(file_dict, os.path.join(directory, d))
    if directory:
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
    # also follow all symlinks under POSIX OSes
    if os.name == 'posix':
        file_list = os.walk(path, followlinks=True)
    else:
        file_list = os.walk(path)
    for root, dirs, files in file_list:
        if root.find("svn") == -1:
            file_dict[root.replace(path, "")] = {"dirs": [], "files": []}
            for dir in dirs:
                # TODO: place the directory name in a constant file.
                if dir.find("svn") == -1 and dir != "images" and dir != "obj" and dir != "doc":
                    file_dict[root.replace(path, "")]["dirs"].append(dir)
            for file in files:
                if file.endswith(const.EXTENSION_FILTER) and file != "buildrev.h":
                    file_dict[root.replace(path, "")]["files"].append(file)
    return file_dict

def codeliteProjectGenerator(project_info):
    """
    Returns the string rapresenting the codelite project.
    """
    template = open(os.path.join(const.DATA_DIR, "cltemplates/bertos.project"), "r").read()
    filelist = "\n".join(clFiles(findSources(project_info.info("PROJECT_PATH")), ""))
    debugger_path = project_info.info("TOOLCHAIN")["path"].replace("gcc", "gdb")
    init_script = project_info.info("CPU_INFOS")["GDB_INIT_SCRIPT"]
    template = template.replace("$filelist", filelist)
    project_name = os.path.basename(project_info.info("PROJECT_PATH"))
    template = template.replace("$project", project_name)
    template = template.replace("$debuggerpath", debugger_path)
    template = template.replace("$initscript", init_script)
    return template

def codeliteWorkspaceGenerator(project_info):
    """
    Returns the string rapresentig the codelite workspace.
    """
    template = open(os.path.join(const.DATA_DIR, "cltemplates/bertos.workspace"), "r").read()
    project_name = os.path.basename(project_info.info("PROJECT_PATH"))
    template = template.replace("$project", project_name)
    return template
