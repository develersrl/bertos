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

import re
import DefineException

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
    return toBeParsed, moduleDict

def loadDefineList(commentList):
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

class ParseError(Exception):
    def __init__(self, line_number, line):
        Exception.__init__(self)
        self.line_number = line_number
        self.line = line

def main():
    try:
        commentList = getCommentList(open("test/to_parse.h", "r").read())
        print loadModuleDefinition(commentList[0])
        print loadDefineList(commentList[1:])
    except ParseError, err:
        print "Error: line %d - %s" % (err.line_number, err.line)

if __name__ == '__main__':
    main()