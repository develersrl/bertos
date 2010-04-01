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
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import os
import fnmatch
import copy
import pickle

import DefineException

from LoadException import VersionException, ToolchainException

import const

from bertos_utils import (
                            # Utility functions
                            isBertosDir, getTagSet, setEnabledModules, getInfos,
                            loadConfigurationInfos, loadDefineLists, loadModuleDefinition,
                            getCommentList, updateConfigurationValues,

                            # Custom exceptions
                            ParseError, SupportedException
                        )

class BProject(object):
    """
    Simple class for store and retrieve project informations.
    """

    def __init__(self, project_file="", info_dict={}):
        self.infos = {}
        self._cached_queries = {}
        if project_file:
            self.loadBertosProject(project_file, info_dict)

    def loadBertosProject(self, project_file, info_dict):
        project_dir = os.path.dirname(project_file)
        project_data = pickle.loads(open(project_file, "r").read())
        # If PROJECT_NAME is not defined it use the directory name as PROJECT_NAME
        # NOTE: this can throw an Exception if the user has changed the directory containing the project
        self.infos["PROJECT_NAME"] = project_data.get("PROJECT_NAME", os.path.basename(project_dir))
        self.infos["PROJECT_PATH"] = os.path.dirname(project_file)
        # Check for the Wizard version
        wizard_version = project_data.get("WIZARD_VERSION", 0)
        # Ignore the SOURCES_PATH inside the project file
        project_data["SOURCES_PATH"] = project_dir
        if "SOURCES_PATH" in info_dict:
            project_data["SOURCES_PATH"] = info_dict["SOURCES_PATH"]
        if os.path.exists(project_data["SOURCES_PATH"]):
            self.infos["SOURCES_PATH"] = project_data["SOURCES_PATH"]
        else:
            raise VersionException(self)
        if not isBertosDir(project_dir):
            version_file = open(os.path.join(const.DATA_DIR, "vtemplates/VERSION"), "r").read()
            open(os.path.join(project_dir, "VERSION"), "w").write(version_file.replace("$version", "").strip())
        self.loadSourceTree()
        cpu_name = project_data["CPU_NAME"]
        self.infos["CPU_NAME"] = cpu_name
        cpu_info = self.loadCpuInfos()
        for cpu in cpu_info:
            if cpu["CPU_NAME"] == cpu_name:
                self.infos["CPU_INFOS"] = cpu
                break
        tag_list = getTagSet(cpu_info)
        # Create, fill and store the dict with the tags
        tag_dict = {}
        for element in tag_list:
            tag_dict[element] = False
        infos = self.info("CPU_INFOS")
        for tag in tag_dict:
            if tag in infos["CPU_TAGS"] + [infos["CPU_NAME"], infos["TOOLCHAIN"]]:
                tag_dict[tag] = True
            else:
                tag_dict[tag] = False
        self.infos["ALL_CPU_TAGS"] = tag_dict
        if "TOOLCHAIN" in info_dict:
            project_data["TOOLCHAIN"] = info_dict["TOOLCHAIN"]
        if os.path.exists(project_data["TOOLCHAIN"]["path"]):
            self.infos["TOOLCHAIN"] = project_data["TOOLCHAIN"]
        else:
            raise ToolchainException(self)
        self.infos["SELECTED_FREQ"] = project_data["SELECTED_FREQ"]
        self.infos["OUTPUT"] = project_data["OUTPUT"]
        self.loadModuleData(True)
        setEnabledModules(self, project_data["ENABLED_MODULES"])

    def loadProjectPresets(self, preset_directory):
        pass

    def loadModuleData(self, edit=False):
        module_info_dict = {}
        list_info_dict = {}
        configuration_info_dict = {}
        file_dict = {}
        for filename, path in self.findDefinitions("*.h") + self.findDefinitions("*.c") + self.findDefinitions("*.s") + self.findDefinitions("*.S"):
            comment_list = getCommentList(open(path + "/" + filename, "r").read())
            if len(comment_list) > 0:
                module_info = {}
                configuration_info = {}
                try:
                    to_be_parsed, module_dict = loadModuleDefinition(comment_list[0])
                except ParseError, err:
                    raise DefineException.ModuleDefineException(path, err.line_number, err.line)
                for module, information in module_dict.items():
                    if "depends" not in information:
                        information["depends"] = ()
                    information["depends"] += (filename.split(".")[0],)
                    information["category"] = os.path.basename(path)
                    if "configuration" in information and len(information["configuration"]):
                        configuration = module_dict[module]["configuration"]
                        try:
                            configuration_info[configuration] = loadConfigurationInfos(self.infos["SOURCES_PATH"] + "/" + configuration)
                        except ParseError, err:
                            raise DefineException.ConfigurationDefineException(self.infos["SOURCES_PATH"] + "/" + configuration, err.line_number, err.line)
                        if edit:
                            try:
                                path = self.infos["PROJECT_NAME"]
                                directory = self.infos["PROJECT_PATH"]
                                user_configuration = loadConfigurationInfos(directory + "/" + configuration.replace("bertos", path))
                                configuration_info[configuration] = updateConfigurationValues(configuration_info[configuration], user_configuration)
                            except ParseError, err:
                                raise DefineException.ConfigurationDefineException(directory + "/" + configuration.replace("bertos", path))
                module_info_dict.update(module_dict)
                configuration_info_dict.update(configuration_info)
                if to_be_parsed:
                    try:
                        list_dict = loadDefineLists(comment_list[1:])
                        list_info_dict.update(list_dict)
                    except ParseError, err:
                        raise DefineException.EnumDefineException(path, err.line_number, err.line)
        for filename, path in self.findDefinitions("*_" + self.infos["CPU_INFOS"]["TOOLCHAIN"] + ".h"):
            comment_list = getCommentList(open(path + "/" + filename, "r").read())
            list_info_dict.update(loadDefineLists(comment_list))
        for tag in self.infos["CPU_INFOS"]["CPU_TAGS"]:
            for filename, path in self.findDefinitions("*_" + tag + ".h"):
                comment_list = getCommentList(open(path + "/" + filename, "r").read())
                list_info_dict.update(loadDefineLists(comment_list))
        self.infos["MODULES"] = module_info_dict
        self.infos["LISTS"] = list_info_dict
        self.infos["CONFIGURATIONS"] = configuration_info_dict
        self.infos["FILES"] = file_dict

    def loadCpuInfos(self):
        cpuInfos = []
        for definition in self.findDefinitions(const.CPU_DEFINITION):
            cpuInfos.append(getInfos(definition))
        return cpuInfos

    def reloadCpuInfo(self):
        for cpu_info in self.loadCpuInfos():
            if cpu_info["CPU_NAME"] == self.infos["CPU_NAME"]:
                self.infos["CPU_INFOS"] = cpu_info

    def setInfo(self, key, value):
        """
        Store the given value with the name key.
        """
        self.infos[key] = value

    def info(self, key, default=None):
        """
        Retrieve the value associated with the name key.
        """
        if key in self.infos:
            return copy.deepcopy(self.infos[key])
        return default

    def loadSourceTree(self):
        # Index only the SOURCES_PATH/bertos content
        bertos_sources_dir = os.path.join(self.info("SOURCES_PATH"), 'bertos')
        file_dict = {}
        if os.path.exists(bertos_sources_dir):
            for element in os.walk(bertos_sources_dir):
                for f in element[2]:
                    file_dict[f] = file_dict.get(f, []) + [element[0]]
        self.infos["FILE_DICT"] = file_dict

    def searchFiles(self, filename):
        file_dict = self.infos["FILE_DICT"]
        return [(filename, dirname) for dirname in file_dict.get(filename, [])]

    def findDefinitions(self, ftype):
        # Maintain a cache for every scanned SOURCES_PATH
        definitions_dict = self._cached_queries.get(self.infos["SOURCES_PATH"], {})
        definitions = definitions_dict.get(ftype, None)
        if definitions is not None:
            return definitions
        file_dict = self.infos["FILE_DICT"]
        definitions = []
        for filename in file_dict:
            if fnmatch.fnmatch(filename, ftype):
                definitions += [(filename, dirname) for dirname in file_dict.get(filename, [])]

        # If no cache for the current SOURCES_PATH create an empty one
        if not definitions_dict:
            self._cached_queries[self.infos["SOURCES_PATH"]] = {}
        # Fill the empty cache with the result
        self._cached_queries[self.infos["SOURCES_PATH"]][ftype] = definitions
        return definitions

    def __repr__(self):
        return repr(self.infos)