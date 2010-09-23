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
import fnmatch
import copy
import pickle
import shutil
import copytree

import DefineException

from LoadException import VersionException, ToolchainException

import const

from bertos_utils import (
                            # Utility functions
                            isBertosDir, getTagSet, getInfos, updateConfigurationValues,
                            loadConfigurationInfos, loadDefineLists, loadModuleDefinition,
                            getCommentList, sub,

                            # Project creation functions
                            projectFileGenerator, versionFileGenerator, loadPlugin,
                            mergeSources,

                            # Custom exceptions
                            ParseError, SupportedException
                        )
import bertos_utils

from compatibility import updateProject

class BProject(object):
    """
    Simple class for store and retrieve project informations.
    """

    def __init__(self, project_file="", info_dict={}):
        self.infos = {}
        self._cached_queries = {}
        self.edit = False
        if project_file:
            self.edit = True
            self.loadBertosProject(project_file, info_dict)

    #--- Load methods (methods that loads data into project) ------------------#

    def loadBertosProject(self, project_file, info_dict):
        project_dir = os.path.dirname(project_file)
        project_data = pickle.loads(open(project_file, "r").read())
        updateProject(project_data)
        # If PROJECT_NAME is not defined it use the directory name as PROJECT_NAME
        # NOTE: this can throw an Exception if the user has changed the directory containing the project
        self.infos["PROJECT_NAME"] = project_data.get("PROJECT_NAME", os.path.basename(project_dir))
        self.infos["PROJECT_PATH"] = os.path.dirname(project_file)
        project_src_path = os.path.join(project_dir, project_data.get("PROJECT_SRC_PATH", project_data["PROJECT_NAME"]))
        if project_src_path:
            self.infos["PROJECT_SRC_PATH"] = project_src_path

        else:
            # In projects created with older versions of the Wizard this metadata doesn't exist
            self.infos["PROJECT_SRC_PATH"] = os.path.join(self.infos["PROJECT_PATH"], self.infos["PROJECT_NAME"])
        self.infos["PROJECT_HW_PATH"] = os.path.join(self.infos["PROJECT_PATH"], project_data.get("PROJECT_HW_PATH", self.infos["PROJECT_PATH"]))

        linked_sources_path = project_data["BERTOS_PATH"]
        sources_abspath = os.path.abspath(os.path.join(project_dir, linked_sources_path))
        project_data["BERTOS_PATH"] = sources_abspath

        self._loadBertosSourceStuff(project_data["BERTOS_PATH"], info_dict.get("BERTOS_PATH", None))

        self.infos["PRESET"] = project_data.get("PRESET", False)

        # For those projects that don't have a VERSION file create a dummy one.
        if not isBertosDir(project_dir) and not self.is_preset:
            version_file = open(os.path.join(const.DATA_DIR, "vtemplates/VERSION"), "r").read()
            open(os.path.join(project_dir, "VERSION"), "w").write(version_file.replace("$version", "").strip())

        self.loadSourceTree()
        self._loadCpuStuff(project_data["CPU_NAME"], project_data["SELECTED_FREQ"])
        self._loadToolchainStuff(project_data["TOOLCHAIN"], info_dict.get("TOOLCHAIN", None))
        self.infos["OUTPUT"] = project_data["OUTPUT"]
        self.loadModuleData(True)
        self.setEnabledModules(project_data["ENABLED_MODULES"])

    def _loadBertosSourceStuff(self, sources_path, forced_version=None):
        if forced_version:
            sources_path = forced_version
        if os.path.exists(sources_path):
            self.infos["BERTOS_PATH"] = sources_path
        else:
            raise VersionException(self)

    def _loadCpuStuff(self, cpu_name, cpu_frequency):
        self.infos["CPU_NAME"] = cpu_name
        cpu_info = self.getCpuInfos()
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
        self.infos["SELECTED_FREQ"] = cpu_frequency

    def _loadToolchainStuff(self, toolchain, forced_toolchain=None):
        toolchain = toolchain
        if forced_toolchain:
            toolchain = forced_toolchain
        if os.path.exists(toolchain["path"]):
            self.infos["TOOLCHAIN"] = toolchain
        else:
            raise ToolchainException(self)

    def loadProjectFromPreset(self, preset):
        """
        Load a project from a preset.
        NOTE: this is a stub.
        """
        project_file = os.path.join(preset, "project.bertos")
        project_data = pickle.loads(open(project_file, "r").read())
        self.loadSourceTree()
        self._loadCpuStuff(project_data["CPU_NAME"], project_data["SELECTED_FREQ"])

        # NOTE: this is a HACK!!!
        # TODO: find a better way to reuse loadModuleData
        preset_project_name = project_data.get("PROJECT_NAME", os.path.basename(preset))
        preset_prj_src_path = os.path.join(preset, project_data.get("PROJECT_SRC_PATH", os.path.join(preset, preset_project_name)))
        preset_prj_hw_path = os.path.join(preset, project_data.get("PROJECT_HW_PATH", preset))

        old_project_name = self.infos["PROJECT_NAME"]
        old_project_path = self.infos["PROJECT_PATH"]
        old_project_src_path = self.infos["PROJECT_SRC_PATH"]
        old_project_hw_path = self.infos["PROJECT_HW_PATH"]

        self.infos["PROJECT_NAME"] = preset_project_name
        self.infos["PROJECT_PATH"] = preset
        self.infos["PROJECT_SRC_PATH"] = preset_prj_src_path
        self.infos["PROJECT_HW_PATH"] = preset_prj_hw_path

        self.loadModuleData(True)
        self.setEnabledModules(project_data["ENABLED_MODULES"])

        self.infos["PROJECT_NAME"] = old_project_name
        self.infos["PROJECT_PATH"] = old_project_path
        self.infos["PROJECT_SRC_PATH"] = old_project_src_path
        self.infos["PROJECT_HW_PATH"] = old_project_hw_path
        # End of the ugly HACK!

        self.infos["PRESET_NAME"] = preset_project_name
        self.infos["PRESET_PATH"] = preset
        self.infos["PRESET_SRC_PATH"] = preset_prj_src_path
        self.infos["PRESET_HW_PATH"] = preset_prj_hw_path

    def loadProjectPresets(self):
        """
        Load the default presets (into the const.PREDEFINED_BOARDS_DIR).
        """
        # NOTE: this method does nothing (for now).
        preset_path = os.path.join(self.infos["BERTOS_PATH"], const.PREDEFINED_BOARDS_DIR)
        preset_tree = {"children": []}
        if os.path.exists(preset_path):
            preset_tree = self._loadProjectPresetTree(preset_path)
        self.infos["PRESET_TREE"] = preset_tree

    def _loadProjectPresetTree(self, path):
        _tree = {}
        _tree["info"] = self._loadPresetInfo(os.path.join(path, const.PREDEFINED_BOARD_SPEC_FILE))
        _tree["info"]["filename"] = os.path.basename(path)
        _tree["info"]["path"] = path
        _tree["children"] = {}
        entries = set(os.listdir(path))
        for entry in entries:
            _path = os.path.join(path, entry)
            if os.path.isdir(_path):
                sub_entries = set(os.listdir(_path))
                if const.PREDEFINED_BOARD_SPEC_FILE in sub_entries:
                    _tree["children"][_path] = self._loadProjectPresetTree(_path)
        # Add into the info dict the dir type (dir/project)
        if _tree["children"]:
            _tree["info"]["type"] = "dir"
        else:
            _tree["info"]["type"] = "project"
        return _tree

    def _loadPresetInfo(self, preset_spec_file):
        D = {}
        try:
            execfile(preset_spec_file, {}, D)
        except IOError, e:
            pass
        return D

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
                    raise DefineException.ModuleDefineException(os.path.join(path, filename), err.line_number, err.line)
                for module, information in module_dict.items():
                    if "depends" not in information:
                        information["depends"] = ()
                    information["depends"] += (filename.split(".")[0],)
                    information["category"] = os.path.basename(path)

                    # Hack to remove 'bertos/' from the configuration file path.
                    #
                    # The new module information format substitute paths like 'bertos/cfg/config_file.h'
                    # with the relative path into the bertos directory ('cfg/config_file.h')
                    information["configuration"] = information["configuration"].replace("bertos/", "")
                    information["hw"] = [hw.replace("bertos/", "") for hw in information["hw"]]

                    if "configuration" in information and len(information["configuration"]):
                        configuration = module_dict[module]["configuration"]
                        try:
                            cfg_file_path = os.path.join(self.bertos_srcdir, configuration)
                            configuration_info[configuration] = loadConfigurationInfos(cfg_file_path)
                        except ParseError, err:
                            raise DefineException.ConfigurationDefineException(cfg_file_path, err.line_number, err.line)
                        if edit:
                            try:
                                path = self.infos["PROJECT_SRC_PATH"]
                                cfg_file_path = os.path.join(path, configuration)
                                configuration_info[configuration] = updateConfigurationValues(configuration_info[configuration], loadConfigurationInfos(cfg_file_path))
                            except ParseError, err:
                                raise DefineException.ConfigurationDefineException(cfg_file_path, err.line_number, err.line)
                            except IOError, err:
                                # The wizard can't find the file, use the default configuration
                                pass
                module_info_dict.update(module_dict)
                configuration_info_dict.update(configuration_info)
                if to_be_parsed:
                    try:
                        list_dict = loadDefineLists(comment_list[1:])
                        list_info_dict.update(list_dict)
                    except ParseError, err:
                        raise DefineException.EnumDefineException(os.path.join(path, filename), err.line_number, err.line)
        for tag in self.infos["CPU_INFOS"]["CPU_TAGS"]:
            for filename, path in self.findDefinitions("*_" + tag + ".h"):
                comment_list = getCommentList(open(path + "/" + filename, "r").read())
                list_info_dict.update(loadDefineLists(comment_list))
        self.infos["MODULES"] = module_info_dict
        self.infos["LISTS"] = list_info_dict
        self.infos["CONFIGURATIONS"] = configuration_info_dict
        self.infos["FILES"] = file_dict

    def loadSourceTree(self):
        """
        Index BeRTOS source and load it in memory.
        """
        # Index only the BERTOS_PATH/bertos content
        bertos_sources_dir = os.path.join(self.info("BERTOS_PATH"), "bertos")
        file_dict = {}
        if os.path.exists(bertos_sources_dir):
            for element in os.walk(bertos_sources_dir):
                for f in element[2]:
                    file_dict[f] = file_dict.get(f, []) + [element[0]]
        self.infos["FILE_DICT"] = file_dict

    def reloadCpuInfo(self):
        for cpu_info in self.getCpuInfos():
            if cpu_info["CPU_NAME"] == self.infos["CPU_NAME"]:
                self.infos["CPU_INFOS"] = cpu_info

    #-------------------------------------------------------------------------#

    def createBertosProject(self):
        # NOTE: Temporary hack.
        if self.edit:
            self._editBertosProject()
        else:
            if not self.from_preset:
                self._newCustomBertosProject()
            else:
                self._newBertosProjectFromPreset()

    def _newBertosProject(self):
        for directory in (self.maindir, self.srcdir, self.prjdir, self.cfgdir, self.hwdir):
            self._createDirectory(directory)
        # Write the project file
        self._writeProjectFile(os.path.join(self.maindir, "project.bertos"))
        # VERSION file
        self._writeVersionFile(os.path.join(self.maindir, "VERSION"))
        # Destination makefile
        self._writeMakefile()
        # Copy the sources
        self._copySources(self.bertos_maindir, self.srcdir)
        # Set properly the autoenabled parameters
        self._setupAutoenabledParameters()
        # Copy all the configuration files
        self._writeCfgFiles(self.bertos_srcdir, self.cfgdir)
        # Destination wizard mk file
        self._writeWizardMkFile()

    def _newCustomBertosProject(self):
        # Create/write/copy the common things
        self._newBertosProject()
        # Copy the clean hw files
        self._createDirectory(self.hwdir)
        # Copy all the hw files
        self._writeHwFiles(self.bertos_srcdir, self.hwdir)
        # Destination user mk file
        self._writeUserMkFile()
        # Destination main.c file
        self._writeMainFile(self.prjdir + "/main.c")
        # Create project files for selected plugins
        self._createProjectFiles()

    def _newBertosProjectFromPreset(self):
        # Create/write/copy the common things
        self._newBertosProject()

        # Copy all the files and dirs except cfg/hw/*.mk
        self._writeCustomSrcFiles()

        # Copy the hw files
        self._writeAllPresetHwFiles(self.src_hwdir, self.hwdir)

        # Copyt the new *_user.mk file
        self._writeUserMkFileFromPreset()

        # Create project files for selected plugins
        self._createProjectFiles()

    def _editBertosProject(self):
        # Write the project file
        self._writeProjectFile(os.path.join(self.maindir, "project.bertos"))
        if not self.is_preset:
            # Generate this files only if the project isn't a preset
            # VERSION file
            self._writeVersionFile(os.path.join(self.maindir, "VERSION"))
            # Destination makefile
            self._writeMakefile()
            # Merge sources
            self._mergeSources(self.bertos_maindir, self.srcdir, self.old_srcdir)
            # Copy all the hw files
            self._writeHwFiles(self.bertos_srcdir, self.hwdir)

        # Destination wizard mk file (it seems that this file need to be
        # rewritten also if the project is a preset)...
        self._writeWizardMkFile()

        # Set properly the autoenabled parameters
        self._setupAutoenabledParameters()
        # Copy all the configuration files
        self._writeCfgFiles(self.bertos_srcdir, self.cfgdir)
        if not self.is_preset:
            # Create project files for selected plugins only if the project isn't a preset
            self._createProjectFiles()

    def _createProjectFiles(self):
        # Files for selected plugins
        relevants_files = {}
        for plugin in self.infos["OUTPUT"]:
            module = loadPlugin(plugin)
            relevants_files[plugin] = module.createProject(self)
        self.infos["RELEVANT_FILES"] = relevants_files

    def _writeVersionFile(self, filename):
        if not self.edit or self.old_srcdir:
            version_file = open(os.path.join(const.DATA_DIR, "vtemplates/VERSION"), "r").read()
            open(filename, "w").write(versionFileGenerator(self, version_file))

    def _writeProjectFile(self, filename):
        f = open(filename, "w")
        f.write(projectFileGenerator(self))
        f.close()

    def _writeMakefile(self):
        bertos_utils.makefileGenerator(self)

    def _writeUserMkFile(self):
        bertos_utils.userMkGenerator(self)

    def _writeUserMkFileFromPreset(self):
        bertos_utils.userMkGeneratorFromPreset(self)

    def _writeWizardMkFile(self):
        bertos_utils.mkGenerator(self)

    def _writeMainFile(self, filename):
        main = open(os.path.join(const.DATA_DIR, "srctemplates/main.c"), "r").read()
        open(filename, "w").write(main)

    def _writeHwFiles(self, source_dir, destination_dir):
        for module, information in self.infos["MODULES"].items():
            for hwfile in information["hw"]:
                if hwfile == "":
                    continue
                string = open(source_dir + "/" + hwfile, "r").read()
                hwfile_path = destination_dir + "/" + os.path.basename(hwfile)
                if not self.edit or not os.path.exists(hwfile_path):
                    # If not in editing mode it copies all the hw files. If in
                    # editing mode it copies only the files that don't exist yet
                    open(os.path.join(destination_dir,os.path.basename(hwfile)), "w").write(string)

    def _writeAllPresetHwFiles(self, source_dir, destination_dir):
        """
        Copy all but directories contained into the preset hw directory.
        It's needed because some presets need custom hw files not defined with
        Wizard directives into modules...
        """
        source_dir = os.path.join(source_dir, "hw")
        for f in os.listdir(source_dir):
            abspath = os.path.join(source_dir, f)
            if not os.path.isdir(abspath):
                # Exlude directories from the copy!
                hw_file = open(os.path.join(source_dir, f), 'r').read()
                open(os.path.join(destination_dir, f), 'w').write(hw_file)

    def _writeCfgFiles(self, source_dir, destination_dir):
        for configuration, information in self.infos["CONFIGURATIONS"].items():
            string = open(source_dir + "/" + configuration, "r").read()
            for start, parameter in information["paramlist"]:
                infos = information[parameter]
                value = infos["value"]
                if "unsigned" in infos["informations"] and infos["informations"]["unsigned"]:
                    value += "U"
                if "long" in infos["informations"] and infos["informations"]["long"]:
                    value += "L"
                string = sub(string, parameter, value)
            f = open(os.path.join(destination_dir, os.path.basename(configuration)), "w")
            f.write(string)
            f.close()

    def _writeCustomSrcFiles(self):
        origin = self.infos["PRESET_SRC_PATH"]
        # Files to be ignored (all project files, cfg dir, wizard mk file, all global ignored dirs)
        project_related_stuff = (
            "cfg",
            "hw",
            self.infos["PRESET_NAME"] + ".mk",
            self.infos["PRESET_NAME"] + "_user.mk",
            "project.bertos",
            self.infos["PRESET_NAME"] + ".project",
            self.infos["PRESET_NAME"] + ".workspace",
        ) + const.IGNORE_LIST
        for element in os.listdir(origin):
            if element not in project_related_stuff:
                full_path = os.path.join(origin, element)
                if os.path.isdir(full_path):
                    copytree.copytree(full_path, os.path.join(self.prjdir, element), ignore_list=const.IGNORE_LIST)
                else:
                    shutil.copy(full_path, self.prjdir)

    def _setupAutoenabledParameters(self):
        for module, information in self.infos["MODULES"].items():
            if "configuration" in information and information["configuration"] != "":
                configurations = self.infos["CONFIGURATIONS"]
                configuration = configurations[information["configuration"]]
                for start, parameter in configuration["paramlist"]:
                    if "type" in configuration[parameter]["informations"] and configuration[parameter]["informations"]["type"] == "autoenabled":
                        configuration[parameter]["value"] = "1" if information["enabled"] else "0"
                self.infos["CONFIGURATIONS"] = configurations

    # Project related properties
    @property
    def maindir(self):
        return self.infos.get("PROJECT_PATH", None)

    @property
    def srcdir(self):
        if self.maindir:
            return os.path.join(self.maindir, "bertos")
        else:
            return None

    @property
    def prjdir(self):
        return self.infos.get("PROJECT_SRC_PATH", None)

    @property
    def hwdir(self):
        if self.prjdir:
            return os.path.join(self.prjdir, "hw")
        else:
            return None

    @property
    def cfgdir(self):
        if self.prjdir:
            return os.path.join(self.prjdir, "cfg")
        else:
            return None

    @property
    def old_srcdir(self):
        return self.infos.get("OLD_BERTOS_PATH", None)

    # BeRTOS sources related properties
    @property
    def bertos_maindir(self):
        return self.infos.get("BERTOS_PATH", None)

    @property
    def bertos_srcdir(self):
        if self.bertos_maindir:
            return os.path.join(self.bertos_maindir, "bertos")
        else:
            return None

    @property
    def src_hwdir(self):
        if self.from_preset:
            return os.path.join(self.infos["PRESET_PATH"], self.infos["PRESET_HW_PATH"])
        else:
            return self.bertos_maindir

    @property
    def from_preset(self):
        return self.infos.get("PROJECT_FROM_PRESET", False)

    @property
    def is_preset(self):
        return self.infos.get("PRESET", False)

    def _createDirectory(self, directory):
        if not directory:
            return
        if os.path.isdir(directory):
            shutil.rmtree(directory, True)
        os.makedirs(directory)

    def _copySources(self, origin, destination):
        # If not in editing mode it copies all the bertos sources in the /bertos subdirectory of the project
        shutil.rmtree(destination, True)
        copytree.copytree(origin + "/bertos", destination, ignore_list=const.IGNORE_LIST)

    def _mergeSources(self, origin, destination, old_sources_dir):
        if old_sources_dir:
            mergeSources(destination, origin, old_sources_dir)

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

    def getCpuInfos(self):
        cpuInfos = []
        for definition in self.findDefinitions(const.CPU_DEFINITION):
            cpuInfos.append(getInfos(definition))
        return cpuInfos

    def searchFiles(self, filename):
        file_dict = self.infos["FILE_DICT"]
        return [(filename, dirname) for dirname in file_dict.get(filename, [])]

    def findDefinitions(self, ftype):
        # Maintain a cache for every scanned BERTOS_PATH
        definitions_dict = self._cached_queries.get(self.infos["BERTOS_PATH"], {})
        definitions = definitions_dict.get(ftype, None)
        if definitions is not None:
            return definitions
        file_dict = self.infos["FILE_DICT"]
        definitions = []
        for filename in file_dict:
            if fnmatch.fnmatch(filename, ftype):
                definitions += [(filename, dirname) for dirname in file_dict.get(filename, [])]

        # If no cache for the current BERTOS_PATH create an empty one
        if not definitions_dict:
            self._cached_queries[self.infos["BERTOS_PATH"]] = {}
        # Fill the empty cache with the result
        self._cached_queries[self.infos["BERTOS_PATH"]][ftype] = definitions
        return definitions

    def setEnabledModules(self, enabled_modules):
        modules = self.infos["MODULES"]
        files = {}
        for module, information in modules.items():
            information["enabled"] = module in enabled_modules
            if information["enabled"]:
                for dependency in information["depends"]:
                    if not dependency in modules:
                        files[dependency] = files.get(dependency, 0) + 1
        self.infos["MODULES"] = modules
        self.infos["FILES"] = files

    def __repr__(self):
        return "<BProject:instance %d>%s" %(id(self), repr(self.infos))
