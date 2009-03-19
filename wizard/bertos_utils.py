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
import shutil

import const
import codelite_project
import DefineException

def isBertosDir(directory):
   return os.path.exists(directory + "/VERSION")

def bertosVersion(directory):
   return open(directory + "/VERSION").readline().strip()

def createBertosProject(projectInfo):
    directory = projectInfo.info("PROJECT_PATH")
    sourcesDir = projectInfo.info("SOURCES_PATH")
    if not os.path.isdir(directory):
        os.mkdir(directory)
    f = open(directory + "/project.bertos", "w")
    f.write(repr(projectInfo))
    f.close()
    ## Destination source dir
    srcdir = directory + "/bertos"
    shutil.rmtree(srcdir, True)
    shutil.copytree(sourcesDir + "/bertos", srcdir)
    ## Destination makefile
    makefile = directory + "/Makefile"
    if os.path.exists(makefile):
        os.remove(makefile)
    makefile = open("mktemplates/Makefile").read()
    makefile = makefileGenerator(projectInfo, makefile)
    open(directory + "/Makefile", "w").write(makefile)
    ## Destination project dir
    prjdir = directory + "/" + os.path.basename(directory)
    shutil.rmtree(prjdir, True)
    os.mkdir(prjdir)
    ## Destination configurations files
    cfgdir = prjdir + "/cfg"
    shutil.rmtree(cfgdir, True)
    os.mkdir(cfgdir)
    for key, value in projectInfo.info("CONFIGURATIONS").items():
        string = open(sourcesDir + "/" + key, "r").read()
        for parameter, infos in value.items():
            value = infos["value"]
            if "unsigned" in infos["informations"].keys() and infos["informations"]["unsigned"]:
                value += "U"
            if "long" in infos["informations"].keys() and infos["informations"]["long"]:
                value += "L"
            string = sub(string, parameter, value)
        f = open(cfgdir + "/" + os.path.basename(key), "w")
        f.write(string)
        f.close()
    ## Destinatio mk file
    makefile = open("mktemplates/template.mk", "r").read()
    makefile = mkGenerator(projectInfo, makefile)
    open(prjdir + "/" + os.path.basename(prjdir) + ".mk", "w").write(makefile)
    if "codelite" in projectInfo.info("OUTPUT"):
        workspace = codeliteWorkspaceGenerator(projectInfo)
        open(directory + "/" + os.path.basename(prjdir) + ".workspace", "w").write(workspace)
        project = codeliteProjectGenerator(projectInfo)
        open(directory + "/" + os.path.basename(prjdir) + ".project", "w").write(project)

def mkGenerator(projectInfo, makefile):
    """
    Generates the mk file for the current project.
    """
    mkData = {}
    mkData["$pname"] = os.path.basename(projectInfo.info("PROJECT_PATH"))
    mkData["$cpuname"] = projectInfo.info("CPU_INFOS")["CORE_CPU"]
    mkData["$cflags"] = " ".join(projectInfo.info("CPU_INFOS")["C_FLAGS"])
    mkData["$ldflags"] = " ".join(projectInfo.info("CPU_INFOS")["LD_FLAGS"])
    mkData["$csrc"], mkData["$pcsrc"] = csrcGenerator(projectInfo)
    mkData["$prefix"] = projectInfo.info("TOOLCHAIN")["path"].split("gcc")[0]
    mkData["$suffix"] = projectInfo.info("TOOLCHAIN")["path"].split("gcc")[1]
    mkData["$cross"] = projectInfo.info("TOOLCHAIN")["path"].split("gcc")[0]
    for key in mkData:
        while makefile.find(key) != -1:
            makefile = makefile.replace(key, mkData[key])
    return makefile

def makefileGenerator(projectInfo, makefile):
    """
    Generate the Makefile for the current project.
    """
    # TODO: write a general function that works for both the mk file and the Makefile
    while makefile.find("project_name") != -1:
        makefile = makefile.replace("project_name", os.path.basename(projectInfo.info("PROJECT_PATH")))
    return makefile

def csrcGenerator(projectInfo):
    modules = projectInfo.info("MODULES")
    if "harvard" in projectInfo.info("CPU_INFOS")["CPU_TAGS"]:
        pcsrc_need = projectInfo.info("CPU_INFOS")["PC_SRC"]
    else:
        pcsrc_need = []
    csrc = []
    pcsrc = []
    for module, information in modules.items():
        if information["enabled"]:
            for filename, path in findDefinitions(module + ".c", projectInfo):
                path = path.replace(projectInfo.info("SOURCES_PATH"), projectInfo.info("PROJECT_PATH"))
                csrc.append(path + "/" + filename)
                if module in pcsrc_need:
                    pcsrc.append(path + "/" + filename)
            for filename, path in findDefinitions(module + "_" + projectInfo.info("CPU_INFOS")["TOOLCHAIN"] + ".c", projectInfo):
                path = path.replace(projectInfo.info("SOURCES_PATH"), projectInfo.info("PROJECT_PATH"))
                csrc.append(path + "/" + filename)
                if module in pcsrc_need:
                    pcsrc.append(path + "/" + filename)
            for tag in projectInfo.info("CPU_INFOS")["CPU_TAGS"]:
                for filename, path in findDefinitions(module + "_" + tag + ".c", projectInfo):
                    path = path.replace(projectInfo.info("SOURCES_PATH"), projectInfo.info("PROJECT_PATH"))
                    csrc.append(path + "/" + filename)
                    if module in pcsrc_need:
                        pcsrc.append(path + "/" + filename)
    csrc = " \\\n\t".join(csrc) + " \\"
    pcsrc = " \\\n\t".join(pcsrc) + " \\"
    return csrc, pcsrc

def codeliteProjectGenerator(projectInfo):
    template = open("cltemplates/bertos.project").read()
    filelist = "\n".join(codelite_project.clFiles(codelite_project.findSources(projectInfo.info("PROJECT_PATH")), projectInfo.info("PROJECT_PATH")))
    while template.find("$filelist") != -1:
        template = template.replace("$filelist", filelist)
    projectName = os.path.basename(projectInfo.info("PROJECT_PATH"))
    while template.find("$project") != -1:
        template = template.replace("$project", projectName)
    return template

def codeliteWorkspaceGenerator(projectInfo):
    template = open("cltemplates/bertos.workspace").read()
    projectName = os.path.basename(projectInfo.info("PROJECT_PATH"))
    while template.find("$project") != -1:
        template = template.replace("$project", projectName)
    return template
    
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
    commentList = re.findall(r"/\*{2}\s*([^*]*\*(?:[^/*][^*]*\*+)*)/", string)
    commentList = [re.findall(r"^\s*\* *(.*?)$", comment, re.MULTILINE) for comment in commentList]
    return commentList

def loadModuleDefinition(first_comment):
    toBeParsed = False
    moduleDefinition = {}
    for num, line in enumerate(first_comment):
        index = line.find("$WIZ$")
        if index != -1:
            toBeParsed = True
            try:
                exec line[index + len("$WIZ$ "):] in {}, moduleDefinition
            except:
                raise ParseError(num, line[index:])
        elif line.find("\\brief") != -1:
            moduleDefinition["module_description"] = line[line.find("\\brief") + len("\\brief "):]
    moduleDict = {}
    if "module_name" in moduleDefinition.keys():
        moduleDict[moduleDefinition["module_name"]] = {}
        if "module_depends" in moduleDefinition.keys():
            if type(moduleDefinition["module_depends"]) == str:
                moduleDefinition["module_depends"] = (moduleDefinition["module_depends"],)
            moduleDict[moduleDefinition["module_name"]]["depends"] = moduleDefinition["module_depends"]
        else:
            moduleDict[moduleDefinition["module_name"]]["depends"] = ()
        if "module_configuration" in moduleDefinition.keys():
            moduleDict[moduleDefinition["module_name"]]["configuration"] = moduleDefinition["module_configuration"]
        else:
            moduleDict[moduleDefinition["module_name"]]["configuration"] = ""
        if "module_description" in moduleDefinition.keys():
            moduleDict[moduleDefinition["module_name"]]["description"] = moduleDefinition["module_description"]
        moduleDict[moduleDefinition["module_name"]]["enabled"] = False
    return toBeParsed, moduleDict

def loadDefineLists(commentList):
    defineList = {}
    for comment in commentList:
        for num, line in enumerate(comment):
            index = line.find("$WIZ$")
            if index != -1:
                try:
                    exec line[index + len("$WIZ$ "):] in {}, defineList
                except:
                    raise ParseError(num, line[index:])
    for key, value in defineList.items():
        if type(value) == str:
            defineList[key] = (value,)
    return defineList

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
    block_tmp = re.findall(r"/\*{2}\s*([^*]*\*(?:[^/*][^*]*\*+)*)/\s*#define\s+((?:[^/]*?/?)+)\s*?(?:/{2,3}[^<].*?)?$", text, re.MULTILINE)
    for comment, define in block_tmp:
        # Only the first element is needed
        block.append(([re.findall(r"^\s*\* *(.*?)$", line, re.MULTILINE)[0] for line in comment.splitlines()], define))
    for comment, define in re.findall(r"/{3}\s*([^<].*?)\s*#define\s+((?:[^/]*?/?)+)\s*?(?:/{2,3}[^<].*?)?$", text, re.MULTILINE):
        block.append(([comment], define))
    for define, comment in re.findall(r"#define\s*(.*?)\s*/{3}<\s*(.+?)\s*?(?:/{2,3}[^<].*?)?$", text, re.MULTILINE):
        block.append(([comment], define))
    return block

def loadModuleData(project):
    moduleInfoDict = {}
    listInfoDict = {}
    configurationInfoDict = {}
    for filename, path in findDefinitions("*.h", project):
        commentList = getCommentList(open(path + "/" + filename, "r").read())
        if len(commentList) > 0:
            moduleInfo = {}
            configurationInfo = {}
            try:
                toBeParsed, moduleDict = loadModuleDefinition(commentList[0])
            except ParseError, err:
                raise DefineException.ModuleDefineException(path, err.line_number, err.line)
            for module, information in moduleDict.items():
                information["category"] = os.path.basename(path)
                if "configuration" in information.keys() and len(information["configuration"]):
                    configuration = moduleDict[module]["configuration"]
                    try:
                        configurationInfo[configuration] = loadConfigurationInfos(project.info("SOURCES_PATH") + "/" + configuration)
                    except ParseError, err:
                        raise DefineException.ConfigurationDefineException(project.info("SOURCES_PATH") + "/" + configuration, err.line_number, err.line)
            moduleInfoDict.update(moduleDict)
            configurationInfoDict.update(configurationInfo)
            if toBeParsed:
                try:
                    listDict = loadDefineLists(commentList[1:])
                    listInfoDict.update(listDict)
                except ParseError, err:
                    raise DefineException.EnumDefineException(path, err.line_number, err.line)
    for filename, path in findDefinitions("*_" + project.info("CPU_INFOS")["TOOLCHAIN"] + ".h", project):
        commentList = getCommentList(open(path + "/" + filename, "r").read())
        listInfoDict.update(loadDefineLists(commentList))
    for tag in project.info("CPU_INFOS")["CPU_TAGS"]:
        for filename, path in findDefinitions("*_" + tag + ".h", project):
            commentList = getCommentList(open(path + "/" + filename, "r").read())
            listInfoDict.update(loadDefineLists(commentList))
    project.setInfo("MODULES", moduleInfoDict)
    project.setInfo("LISTS", listInfoDict)
    project.setInfo("CONFIGURATIONS", configurationInfoDict)
    
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
            "value_list": the name of the enum for enum parameters
    """
    configurationInfos = {}
    for comment, define in getDefinitionBlocks(open(path, "r").read()):
        name, value = formatParamNameValue(define)
        brief, description, informations = getDescriptionInformations(comment)
        configurationInfos[name] = {}
        configurationInfos[name]["value"] = value
        configurationInfos[name]["informations"] = informations
        if ("type" in configurationInfos[name]["informations"].keys() and
                configurationInfos[name]["informations"]["type"] == "int" and
                configurationInfos[name]["value"].find("L") != -1):
            configurationInfos[name]["informations"]["long"] = True
            configurationInfos[name]["value"] = configurationInfos[name]["value"].replace("L", "")
        if ("type" in configurationInfos[name]["informations"].keys() and
                configurationInfos[name]["informations"]["type"] == "int" and
                configurationInfos[name]["value"].find("U") != -1):
            configurationInfos[name]["informations"]["unsigned"] = True
            configurationInfos[name]["value"] = configurationInfos[name]["value"].replace("U", "")
        configurationInfos[name]["description"] = description
        configurationInfos[name]["brief"] = brief
    return configurationInfos

def sub(string, parameter, value):
    """
    Substitute the given value at the given parameter define in the given string
    """
    return re.sub(r"(?P<define>#define\s+" + parameter + r"\s+)([^\s]+)", r"\g<define>" + value, string)

def isInt(informations):
    """
    Return True if the value is a simple int.
    """
    if ("long" not in informatios.keys() or not informations["long"]) and ("unsigned" not in informations.keys() or informations["unsigned"]):
        return True
    else:
        return False

def isLong(informations):
    """
    Return True if the value is a long.
    """
    if "long" in informations.keys() and informations["long"] and "unsigned" not in informations.keys():
        return True
    else:
        return False

def isUnsigned(informations):
    """
    Return True if the value is an unsigned.
    """
    if "unsigned" in informations.keys() and informations["unsigned"] and "long" not in informations.keys():
        return True
    else:
        return False

def isUnsignedLong(informations):
    """
    Return True if the value is an unsigned long.
    """
    if "unsigned" in informations.keys() and "long" in informations.keys() and informations["unsigned"] and informations["long"]:
        return True
    else:
        return False

class ParseError(Exception):
    def __init__(self, line_number, line):
        Exception.__init__(self)
        self.line_number = line_number
        self.line = line
