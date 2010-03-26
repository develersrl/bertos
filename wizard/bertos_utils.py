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
import glob
import re
import shutil
# Use custom copytree function
import copytree
import pickle

import const
import plugins
import DefineException
import BProject

from LoadException import VersionException, ToolchainException

def isBertosDir(directory):
   return os.path.exists(directory + "/VERSION")

def bertosVersion(directory):
   return open(directory + "/VERSION").readline().strip()

def loadBertosProject(project_file, info_dict):
    project_data = pickle.loads(open(project_file, "r").read())
    project_info = BProject.BProject()
    project_info.setInfo("PROJECT_PATH", os.path.dirname(project_file))
    if "SOURCES_PATH" in info_dict:
        project_data["SOURCES_PATH"] = info_dict["SOURCES_PATH"]
    if os.path.exists(project_data["SOURCES_PATH"]):
        project_info.setInfo("SOURCES_PATH", project_data["SOURCES_PATH"])
    else:
        raise VersionException(project_info)
    loadSourceTree(project_info)
    cpu_name = project_data["CPU_NAME"]
    project_info.setInfo("CPU_NAME", cpu_name)
    cpu_info = loadCpuInfos(project_info)
    for cpu in cpu_info:
        if cpu["CPU_NAME"] == cpu_name:
            project_info.setInfo("CPU_INFOS", cpu)
            break
    tag_list = getTagSet(cpu_info)
    # Create, fill and store the dict with the tags
    tag_dict = {}
    for element in tag_list:
        tag_dict[element] = False
    infos = project_info.info("CPU_INFOS")
    for tag in tag_dict:
        if tag in infos["CPU_TAGS"] + [infos["CPU_NAME"], infos["TOOLCHAIN"]]:
            tag_dict[tag] = True
        else:
            tag_dict[tag] = False
    project_info.setInfo("ALL_CPU_TAGS", tag_dict)
    if "TOOLCHAIN" in info_dict:
        project_data["TOOLCHAIN"] = info_dict["TOOLCHAIN"]
    if os.path.exists(project_data["TOOLCHAIN"]["path"]):
        project_info.setInfo("TOOLCHAIN", project_data["TOOLCHAIN"])
    else:
        raise ToolchainException(project_info)
    project_info.setInfo("SELECTED_FREQ", project_data["SELECTED_FREQ"])
    project_info.setInfo("OUTPUT", project_data["OUTPUT"])
    loadModuleData(project_info, True)
    setEnabledModules(project_info, project_data["ENABLED_MODULES"])
    return project_info

def setEnabledModules(project_info, enabled_modules):
    modules = project_info.info("MODULES")
    files = {}
    for module, information in modules.items():
        information["enabled"] = module in enabled_modules
	for dependency in information["depends"]:
            if not dependency in modules:
	        if dependency in files:
		    files[dependency] += 1
		else:
		    files[dependency] = 1
    project_info.setInfo("MODULES", modules)
    project_info.setInfo("FILES", files)

def enabledModules(project_info):
    enabled_modules = []
    for name, module in project_info.info("MODULES").items():
        if module["enabled"]:
            enabled_modules.append(name)
    return enabled_modules

def mergeSources(srcdir, new_sources, old_sources):
    # The current mergeSources function provide only a raw copy of the sources in the
    # created project.
    #
    # TODO: implement the three way merge algorithm
    #
    shutil.rmtree(srcdir, True)
    copytree.copytree(os.path.join(new_sources, "bertos"), srcdir, ignore_list=const.IGNORE_LIST)

def projectFileGenerator(project_info):
    directory = project_info.info("PROJECT_PATH")
    project_data = {}
    enabled_modules = []
    for module, information in project_info.info("MODULES").items():
        if information["enabled"]:
            enabled_modules.append(module)
    project_data["ENABLED_MODULES"] = enabled_modules
    project_data["SOURCES_PATH"] = project_info.info("SOURCES_PATH")
    project_data["TOOLCHAIN"] = project_info.info("TOOLCHAIN")
    project_data["CPU_NAME"] = project_info.info("CPU_NAME")
    project_data["SELECTED_FREQ"] = project_info.info("SELECTED_FREQ")
    project_data["OUTPUT"] = project_info.info("OUTPUT")
    return pickle.dumps(project_data)

def createBertosProject(project_info, edit=False):
    directory = project_info.info("PROJECT_PATH")
    sources_dir = project_info.info("SOURCES_PATH")
    old_sources_dir = project_info.info("OLD_SOURCES_PATH")
    if not edit:
        if os.path.isdir(directory):
            shutil.rmtree(directory, True)        
        os.makedirs(directory)
    # Write the project file
    f = open(directory + "/project.bertos", "w")
    f.write(projectFileGenerator(project_info))
    f.close()
    # Destination source dir
    srcdir = directory + "/bertos"
    if not edit:
        # If not in editing mode it copies all the bertos sources in the /bertos subdirectory of the project
        shutil.rmtree(srcdir, True)
        copytree.copytree(sources_dir + "/bertos", srcdir, ignore_list=const.IGNORE_LIST)
    elif old_sources_dir:
        # If in editing mode it merges the current bertos sources with the selected ones
        # TODO: implement the three way merge algotihm
        #
        mergeSources(srcdir, sources_dir, old_sources_dir)
    # Destination makefile
    makefile = directory + "/Makefile"
    makefile = open(os.path.join(const.DATA_DIR, "mktemplates/Makefile"), 'r').read()
    makefile = makefileGenerator(project_info, makefile)
    open(directory + "/Makefile", "w").write(makefile)
    # Destination project dir
    prjdir = directory + "/" + os.path.basename(directory)
    if not edit:
        shutil.rmtree(prjdir, True)
        os.mkdir(prjdir)
    # Destination hw files
    hwdir = prjdir + "/hw"
    if not edit:
        shutil.rmtree(hwdir, True)
        os.mkdir(hwdir)
    # Copy all the hw files
    for module, information in project_info.info("MODULES").items():
        for hwfile in information["hw"]:
            string = open(sources_dir + "/" + hwfile, "r").read()
            hwfile_path = hwdir + "/" + os.path.basename(hwfile)
            if not edit or not os.path.exists(hwfile_path):
                # If not in editing mode it copies all the hw files. If in
                # editing mode it copies only the files that don't exist yet
                open(hwdir + "/" + os.path.basename(hwfile), "w").write(string)
    # Destination configurations files
    cfgdir = prjdir + "/cfg"
    if not edit:
        shutil.rmtree(cfgdir, True)
        os.mkdir(cfgdir)
    # Set properly the autoenabled parameters
    for module, information in project_info.info("MODULES").items():
        if "configuration" in information and information["configuration"] != "":
            configurations = project_info.info("CONFIGURATIONS")
            configuration = configurations[information["configuration"]]
            for start, parameter in configuration["paramlist"]:
                if "type" in configuration[parameter]["informations"] and configuration[parameter]["informations"]["type"] == "autoenabled":
                    configuration[parameter]["value"] = "1" if information["enabled"] else "0"
            project_info.setInfo("CONFIGURATIONS", configurations)
    # Copy all the configuration files
    for configuration, information in project_info.info("CONFIGURATIONS").items():
        string = open(sources_dir + "/" + configuration, "r").read()
        for start, parameter in information["paramlist"]:
            infos = information[parameter]
            value = infos["value"]
            if "unsigned" in infos["informations"] and infos["informations"]["unsigned"]:
                value += "U"
            if "long" in infos["informations"] and infos["informations"]["long"]:
                value += "L"
            string = sub(string, parameter, value)
        f = open(cfgdir + "/" + os.path.basename(configuration), "w")
        f.write(string)
        f.close()
    if not edit:
        # Destination user mk file (only on project creation)
        makefile = open(os.path.join(const.DATA_DIR, "mktemplates/template.mk"), "r").read()
        makefile = mkGenerator(project_info, makefile)
        open(prjdir + "/" + os.path.basename(prjdir) + ".mk", "w").write(makefile)
    # Destination wizard mk file
    makefile = open(os.path.join(const.DATA_DIR, "mktemplates/template_wiz.mk"), "r").read()
    makefile = mkGenerator(project_info, makefile)
    open(prjdir + "/" + os.path.basename(prjdir) + "_wiz.mk", "w").write(makefile)
    # Destination main.c file
    if not edit:
        main = open(os.path.join(const.DATA_DIR, "srctemplates/main.c"), "r").read()
        open(prjdir + "/main.c", "w").write(main)
    # Files for selected plugins
    relevants_files = {}
    for plugin in project_info.info("OUTPUT"):
        module = loadPlugin(plugin)
        relevants_files[plugin] = module.createProject(project_info)
    project_info.setInfo("RELEVANT_FILES", relevants_files)

def loadPlugin(plugin):
    """
    Returns the given plugin module.
    """
    return getattr(__import__("plugins", {}, {}, [plugin]), plugin)
    
def mkGenerator(project_info, makefile):
    """
    Generates the mk file for the current project.
    """
    mk_data = {}
    mk_data["$pname"] = os.path.basename(project_info.info("PROJECT_PATH"))
    mk_data["$cpuclockfreq"] = project_info.info("SELECTED_FREQ")
    cpu_mk_parameters = []
    for key, value in project_info.info("CPU_INFOS").items():
        if key.startswith(const.MK_PARAM_ID):
            cpu_mk_parameters.append("%s = %s" %(key.replace("MK", mk_data["$pname"]), value))
    mk_data["$cpuparameters"] = "\n".join(cpu_mk_parameters)
    mk_data["$csrc"], mk_data["$pcsrc"], mk_data["$cppasrc"], mk_data["$cxxsrc"], mk_data["$asrc"], mk_data["$constants"] = csrcGenerator(project_info)
    mk_data["$prefix"] = replaceSeparators(project_info.info("TOOLCHAIN")["path"].split("gcc")[0])
    mk_data["$suffix"] = replaceSeparators(project_info.info("TOOLCHAIN")["path"].split("gcc")[1])
    mk_data["$main"] = os.path.basename(project_info.info("PROJECT_PATH")) + "/main.c"
    for key in mk_data:
        while makefile.find(key) != -1:
            makefile = makefile.replace(key, mk_data[key])
    return makefile

def makefileGenerator(project_info, makefile):
    """
    Generate the Makefile for the current project.
    """
    # TODO write a general function that works for both the mk file and the Makefile
    while makefile.find("$pname") != -1:
        makefile = makefile.replace("$pname", os.path.basename(project_info.info("PROJECT_PATH")))
    return makefile

def csrcGenerator(project_info):
    modules = project_info.info("MODULES")
    files = project_info.info("FILES")
    if "harvard" in project_info.info("CPU_INFOS")["CPU_TAGS"]:
        harvard = True
    else:
        harvard = False
    # file to be included in CSRC variable
    csrc = []
    # file to be included in PCSRC variable
    pcsrc = []
    # files to be included in CPPASRC variable
    cppasrc = []
    # files to be included in CXXSRC variable
    cxxsrc = []
    # files to be included in ASRC variable
    asrc = []
    # constants to be included at the beginning of the makefile
    constants = {}
    for module, information in modules.items():
        module_files = set([])
        dependency_files = set([])
        # assembly sources
        asm_files = set([])
        hwdir = os.path.basename(project_info.info("PROJECT_PATH")) + "/hw" 
        if information["enabled"]:
            if "constants" in information:
                constants.update(information["constants"])
            cfiles, sfiles = findModuleFiles(module, project_info)
            module_files |= set(cfiles)
            asm_files |= set(sfiles)
            for file in information["hw"]:
                if file.endswith(".c"):
                    module_files |= set([hwdir + "/" + os.path.basename(file)])
            for file_dependency in information["depends"] + tuple(files.keys()):
                    dependencyCFiles, dependencySFiles = findModuleFiles(file_dependency, project_info)
                    dependency_files |= set(dependencyCFiles)
                    asm_files |= set(dependencySFiles)
            for file in module_files:
                if not harvard or information.get("harvard", "both") == "both":
                    csrc.append(file)
                if harvard and "harvard" in information:
                    pcsrc.append(file)
            for file in dependency_files:
                csrc.append(file)
            for file in project_info.info("CPU_INFOS")["C_SRC"]:
                csrc.append(file)
            for file in project_info.info("CPU_INFOS")["PC_SRC"]:
                pcsrc.append(file)
            for file in asm_files:
                cppasrc.append(file)
    for file in project_info.info("CPU_INFOS")["CPPA_SRC"]:
        cppasrc.append(file)
    for file in project_info.info("CPU_INFOS")["CXX_SRC"]:
        cxxsrc.append(file)
    for file in project_info.info("CPU_INFOS")["ASRC"]:
        asrc.append(file)
    csrc = set(csrc)
    csrc = " \\\n\t".join(csrc) + " \\"
    pcsrc = set(pcsrc)
    pcsrc = " \\\n\t".join(pcsrc) + " \\"
    cppasrc = set(cppasrc)
    cppasrc = " \\\n\t".join(cppasrc) + " \\"
    cxxsrc = set(cxxsrc)
    cxxsrc = " \\\n\t".join(cxxsrc) + " \\"
    asrc = set(asrc)
    asrc = " \\\n\t".join(asrc) + " \\"
    constants = "\n".join([os.path.basename(project_info.info("PROJECT_PATH")) + "_" + key + " = " + unicode(value) for key, value in constants.items()])
    return csrc, pcsrc, cppasrc, cxxsrc, asrc, constants

def findModuleFiles(module, project_info):
    # Find the files related to the selected module
    cfiles = []
    sfiles = []
    # .c files related to the module and the cpu architecture
    for filename, path in findDefinitions(module + ".c", project_info) + \
            findDefinitions(module + "_" + project_info.info("CPU_INFOS")["TOOLCHAIN"] + ".c", project_info):
        path = path.replace(project_info.info("SOURCES_PATH") + os.sep, "")
        path = replaceSeparators(path)
        cfiles.append(path + "/" + filename)
    # .s files related to the module and the cpu architecture
    for filename, path in findDefinitions(module + ".s", project_info) + \
            findDefinitions(module + "_" + project_info.info("CPU_INFOS")["TOOLCHAIN"] + ".s", project_info) + \
            findDefinitions(module + ".S", project_info) + \
            findDefinitions(module + "_" + project_info.info("CPU_INFOS")["TOOLCHAIN"] + ".S", project_info):
        path = path.replace(project_info.info("SOURCES_PATH") + os.sep, "")
        path = replaceSeparators(path)
        sfiles.append(path + "/" + filename)
    # .c and .s files related to the module and the cpu tags
    for tag in project_info.info("CPU_INFOS")["CPU_TAGS"]:
        for filename, path in findDefinitions(module + "_" + tag + ".c", project_info):
            path = path.replace(project_info.info("SOURCES_PATH") + os.sep, "")
            if os.sep != "/":
                path = replaceSeparators(path)
            cfiles.append(path + "/" + filename)
        for filename, path in findDefinitions(module + "_" + tag + ".s", project_info) + \
                findDefinitions(module + "_" + tag + ".S", project_info):
            path = path.replace(project_info.info("SOURCES_PATH") + os.sep, "")
            path = replaceSeparators(path)
            sfiles.append(path + "/" + filename)
    return cfiles, sfiles

def replaceSeparators(path):
    """
    Replace the separators in the given path with unix standard separator.
    """
    if os.sep != "/":
        while path.find(os.sep) != -1:
            path = path.replace(os.sep, "/")
    return path

def getSystemPath():
    path = os.environ["PATH"]
    if os.name == "nt":
        path = path.split(";")
    else:
        path = path.split(":")
    return path

def findToolchains(path_list):
    toolchains = []
    for element in path_list:
        for toolchain in glob.glob(element+ "/" + const.GCC_NAME):
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

def getToolchainName(toolchain_info):
    name = "GCC " + toolchain_info["version"] + " - " + toolchain_info["target"].strip()
    return name

def loadSourceTree(project):
    fileList = [f for f in os.walk(project.info("SOURCES_PATH"))]
    project.setInfo("FILE_LIST", fileList)

def findDefinitions(ftype, project):
    L = project.info("FILE_LIST")
    definitions = []
    for element in L:
        for filename in element[2]:
            if fnmatch.fnmatch(filename, ftype):
                definitions.append((filename, element[0]))
    return definitions

def loadCpuInfos(project):
    cpuInfos = []
    for definition in findDefinitions(const.CPU_DEFINITION, project):
        cpuInfos.append(getInfos(definition))
    return cpuInfos

def getTagSet(cpu_info):
    tag_set = set([])
    for cpu in cpu_info:
        tag_set |= set([cpu["CPU_NAME"]])
        tag_set |= set(cpu["CPU_TAGS"])
        tag_set |= set([cpu["TOOLCHAIN"]])
    return tag_set
        

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

def getCommentList(string):
    comment_list = re.findall(r"/\*{2}\s*([^*]*\*(?:[^/*][^*]*\*+)*)/", string)
    comment_list = [re.findall(r"^\s*\* *(.*?)$", comment, re.MULTILINE) for comment in comment_list]
    return comment_list

def loadModuleDefinition(first_comment):
    to_be_parsed = False
    module_definition = {}
    for num, line in enumerate(first_comment):
        index = line.find("$WIZ$")
        if index != -1:
            to_be_parsed = True
            try:
                exec line[index + len("$WIZ$ "):] in {}, module_definition
            except:
                raise ParseError(num, line[index:])
        elif line.find("\\brief") != -1:
            module_definition["module_description"] = line[line.find("\\brief") + len("\\brief "):]
    module_dict = {}
    if "module_name" in module_definition:
        module_name = module_definition[const.MODULE_DEFINITION["module_name"]]
        del module_definition[const.MODULE_DEFINITION["module_name"]]
        module_dict[module_name] = {}
        if const.MODULE_DEFINITION["module_depends"] in module_definition:
            depends = module_definition[const.MODULE_DEFINITION["module_depends"]]
            del module_definition[const.MODULE_DEFINITION["module_depends"]]
            if type(depends) == str:
                depends = (depends,)
            module_dict[module_name]["depends"] = depends
        else:
            module_dict[module_name]["depends"] = ()
        if const.MODULE_DEFINITION["module_configuration"] in module_definition:
            module_dict[module_name]["configuration"] = module_definition[const.MODULE_DEFINITION["module_configuration"]]
            del module_definition[const.MODULE_DEFINITION["module_configuration"]]
        else:
            module_dict[module_name]["configuration"] = ""
        if "module_description" in module_definition:
            module_dict[module_name]["description"] = module_definition["module_description"]
            del module_definition["module_description"]
        if const.MODULE_DEFINITION["module_harvard"] in module_definition:
            harvard = module_definition[const.MODULE_DEFINITION["module_harvard"]]
            module_dict[module_name]["harvard"] = harvard
            del module_definition[const.MODULE_DEFINITION["module_harvard"]]
        if const.MODULE_DEFINITION["module_hw"] in module_definition:
            hw = module_definition[const.MODULE_DEFINITION["module_hw"]]
            del module_definition[const.MODULE_DEFINITION["module_hw"]]
            if type(hw) == str:
                hw = (hw, )
            module_dict[module_name]["hw"] = hw
        else:
            module_dict[module_name]["hw"] = ()
        if const.MODULE_DEFINITION["module_supports"] in module_definition:
            supports = module_definition[const.MODULE_DEFINITION["module_supports"]]
            del module_definition[const.MODULE_DEFINITION["module_supports"]]
            module_dict[module_name]["supports"] = supports
        module_dict[module_name]["constants"] = module_definition
        module_dict[module_name]["enabled"] = False
    return to_be_parsed, module_dict

def isSupported(project, module=None, property_id=None):
    if not module and property_id:
        item = project.info("CONFIGURATIONS")[property_id[0]][property_id[1]]["informations"]
    else:
        item = project.info("MODULES")[module]
    tag_dict = project.info("ALL_CPU_TAGS")
    if "supports" in item:
        support_string = item["supports"]
        supported = {}
        try:
            exec "supported = " + support_string in tag_dict, supported
        except:
            raise SupportedException(support_string)
        return supported["supported"]
    else:
        return True

def loadDefineLists(comment_list):
    define_list = {}
    for comment in comment_list:
        for num, line in enumerate(comment):
            index = line.find("$WIZ$")
            if index != -1:
                try:
                    exec line[index + len("$WIZ$ "):] in {}, define_list
                except:
                    raise ParseError(num, line[index:])
    for key, value in define_list.items():
        if type(value) == str:
            define_list[key] = (value,)
    return define_list

def getDescriptionInformations(comment):
    """
    Take the doxygen comment and strip the wizard informations, returning the tuple
    (comment, wizard_information)
    """
    brief = ""
    description = ""
    information = {}
    for num, line in enumerate(comment):
        index = line.find("$WIZ$")
        if index != -1:
            if len(brief) == 0:
                brief += line[:index].strip()
            else:
                description += " " + line[:index]
            try:
                exec line[index + len("$WIZ$ "):] in {}, information
            except:
                raise ParseError(num, line[index:])
        else:
            if len(brief) == 0:
                brief += line.strip()
            else:
                description += " " + line
                description = description.strip()
    return brief.strip(), description.strip(), information

def getDefinitionBlocks(text):
    """
    Take a text and return a list of tuple (description, name-value).
    """
    block = []
    block_tmp = re.finditer(r"/\*{2}\s*([^*]*\*(?:[^/*][^*]*\*+)*)/\s*#define\s+((?:[^/]*?/?)+)\s*?(?:/{2,3}[^<].*?)?$", text, re.MULTILINE)
    for match in block_tmp:
        # Only the first element is needed
        comment = match.group(1)
        define = match.group(2)
        start = match.start()
        block.append(([re.findall(r"^\s*\* *(.*?)$", line, re.MULTILINE)[0] for line in comment.splitlines()], define, start))
    for match in re.finditer(r"/{3}\s*([^<].*?)\s*#define\s+((?:[^/]*?/?)+)\s*?(?:/{2,3}[^<].*?)?$", text, re.MULTILINE):
        comment = match.group(1)
        define = match.group(2)
        start = match.start()
        block.append(([comment], define, start))
    for match in re.finditer(r"#define\s*(.*?)\s*/{3}<\s*(.+?)\s*?(?:/{2,3}[^<].*?)?$", text, re.MULTILINE):
        comment = match.group(2)
        define = match.group(1)
        start = match.start()
        block.append(([comment], define, start))
    return block

def loadModuleData(project, edit=False):
    module_info_dict = {}
    list_info_dict = {}
    configuration_info_dict = {}
    file_dict = {}
    for filename, path in findDefinitions("*.h", project) + findDefinitions("*.c", project) + findDefinitions("*.s", project) + findDefinitions("*.S", project):
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
                        configuration_info[configuration] = loadConfigurationInfos(project.info("SOURCES_PATH") + "/" + configuration)
                    except ParseError, err:
                        raise DefineException.ConfigurationDefineException(project.info("SOURCES_PATH") + "/" + configuration, err.line_number, err.line)
                    if edit:
                        try:
                            path = os.path.basename(project.info("PROJECT_PATH"))
                            directory = project.info("PROJECT_PATH")
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
    for filename, path in findDefinitions("*_" + project.info("CPU_INFOS")["TOOLCHAIN"] + ".h", project):
        comment_list = getCommentList(open(path + "/" + filename, "r").read())
        list_info_dict.update(loadDefineLists(comment_list))
    for tag in project.info("CPU_INFOS")["CPU_TAGS"]:
        for filename, path in findDefinitions("*_" + tag + ".h", project):
            comment_list = getCommentList(open(path + "/" + filename, "r").read())
            list_info_dict.update(loadDefineLists(comment_list))
    project.setInfo("MODULES", module_info_dict)
    project.setInfo("LISTS", list_info_dict)
    project.setInfo("CONFIGURATIONS", configuration_info_dict)
    project.setInfo("FILES", file_dict)

def formatParamNameValue(text):
    """
    Take the given string and return a tuple with the name of the parameter in the first position
    and the value in the second.
    """
    block = re.findall("\s*([^\s]+)\s*(.+?)\s*$", text, re.MULTILINE)
    return block[0]

def loadConfigurationInfos(path):
    """
    Return the module configurations found in the given file as a dict with the
    parameter name as key and a dict containig the fields above as value:
        "value": the value of the parameter
        "description": the description of the parameter
        "informations": a dict containig optional informations:
            "type": "int" | "boolean" | "enum"
            "min": the minimum value for integer parameters
            "max": the maximum value for integer parameters
            "long": boolean indicating if the num is a long
            "unsigned": boolean indicating if the num is an unsigned
            "value_list": the name of the enum for enum parameters
            "conditional_deps": the list of conditional dependencies for boolean parameters
    """
    configuration_infos = {}
    configuration_infos["paramlist"] = []
    for comment, define, start in getDefinitionBlocks(open(path, "r").read()):
        name, value = formatParamNameValue(define)
        brief, description, informations = getDescriptionInformations(comment)
        configuration_infos["paramlist"].append((start, name))
        configuration_infos[name] = {}
        configuration_infos[name]["value"] = value
        configuration_infos[name]["informations"] = informations
        if not "type" in configuration_infos[name]["informations"]:
            configuration_infos[name]["informations"]["type"] = findParameterType(configuration_infos[name])
        if ("type" in configuration_infos[name]["informations"] and
                configuration_infos[name]["informations"]["type"] == "int" and
                configuration_infos[name]["value"].find("L") != -1):
            configuration_infos[name]["informations"]["long"] = True
            configuration_infos[name]["value"] = configuration_infos[name]["value"].replace("L", "")
        if ("type" in configuration_infos[name]["informations"] and
                configuration_infos[name]["informations"]["type"] == "int" and
                configuration_infos[name]["value"].find("U") != -1):
            configuration_infos[name]["informations"]["unsigned"] = True
            configuration_infos[name]["value"] = configuration_infos[name]["value"].replace("U", "")
        if "conditional_deps" in configuration_infos[name]["informations"]:
            if (type(configuration_infos[name]["informations"]["conditional_deps"]) == str or
                    type(configuration_infos[name]["informations"]["conditional_deps"]) == unicode):
                configuration_infos[name]["informations"]["conditional_deps"] = (configuration_infos[name]["informations"]["conditional_deps"], )
            elif type(configuration_infos[name]["informations"]["conditional_deps"]) == tuple:
                pass
            else:
                configuration_infos[name]["informations"]["conditional_deps"] = ()
        configuration_infos[name]["description"] = description
        configuration_infos[name]["brief"] = brief
    return configuration_infos

def updateConfigurationValues(def_conf, user_conf):
    for param in def_conf["paramlist"]:
        if param[1] in user_conf and "value" in user_conf[param[1]]:
            def_conf[param[1]]["value"] = user_conf[param[1]]["value"]
    return def_conf

def findParameterType(parameter):
    if "value_list" in parameter["informations"]:
        return "enum"
    if "min" in parameter["informations"] or "max" in parameter["informations"] or re.match(r"^\d+U?L?$", parameter["value"]) != None:
        return "int"

def sub(string, parameter, value):
    """
    Substitute the given value at the given parameter define in the given string
    """
    return re.sub(r"(?P<define>#define\s+" + parameter + r"\s+)([^\s]+)", r"\g<define>" + value, string)

def isInt(informations):
    """
    Return True if the value is a simple int.
    """
    if ("long" not in informatios or not informations["long"]) and ("unsigned" not in informations or informations["unsigned"]):
        return True
    else:
        return False

def isLong(informations):
    """
    Return True if the value is a long.
    """
    if "long" in informations and informations["long"] and "unsigned" not in informations:
        return True
    else:
        return False

def isUnsigned(informations):
    """
    Return True if the value is an unsigned.
    """
    if "unsigned" in informations and informations["unsigned"] and "long" not in informations:
        return True
    else:
        return False

def isUnsignedLong(informations):
    """
    Return True if the value is an unsigned long.
    """
    if "unsigned" in informations and "long" in informations and informations["unsigned"] and informations["long"]:
        return True
    else:
        return False

class ParseError(Exception):
    def __init__(self, line_number, line):
        Exception.__init__(self)
        self.line_number = line_number
        self.line = line

class SupportedException(Exception):
    def __init__(self, support_string):
        Exception.__init__(self)
        self.support_string = support_string
