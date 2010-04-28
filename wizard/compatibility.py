#!/usr/bin/env python
# encoding: utf-8
#
# This file is part of slimqc.
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
# Copyright 2010 Develer S.r.l. (http://www.develer.com/)
#
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import const
import os
from bertos_utils import replaceSeparators, csrcGenerator

def _userMkGenerator(project_info):
    makefile = open(os.path.join(const.DATA_DIR, "mktemplates/old/template.mk"), "r").read()
    destination = os.path.join(project_info.prjdir, os.path.basename(project_info.prjdir) + ".mk")
    # Deadly performances loss was here :(
    mk_data = {}
    mk_data["$pname"] = os.path.basename(project_info.info("PROJECT_PATH"))
    mk_data["$main"] = os.path.basename(project_info.info("PROJECT_PATH")) + "/main.c"
    for key in mk_data:
        makefile = makefile.replace(key, mk_data[key])
    open(destination, "w").write(makefile)

def _mkGenerator(project_info):
    """
    Generates the mk file for the current project.
    """
    makefile = open(os.path.join(const.DATA_DIR, "mktemplates/old/template_wiz.mk"), "r").read()
    destination = os.path.join(project_info.prjdir, os.path.basename(project_info.prjdir) + "_wiz.mk")
    mk_data = {}
    mk_data["$pname"] = project_info.info("PROJECT_NAME")
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
        makefile = makefile.replace(key, mk_data[key])
    open(destination, "w").write(makefile)

def _makefileGenerator(project_info):
    """
    Generate the Makefile for the current project.
    """
    makefile = open(os.path.join(const.DATA_DIR, "mktemplates/old/Makefile"), "r").read()
    destination = os.path.join(project_info.maindir, "Makefile")
    # TODO write a general function that works for both the mk file and the Makefile
    makefile = makefile.replace("$pname", project_info.info("PROJECT_NAME"))
    open(destination, "w").write(makefile)

def updateProject(project_data):
    """
    Update incrementally the project_data loaded from a BeRTOS Wizard project
    file.
    """
    wizard_version = project_data.get("WIZARD_VERSION", 0)
    if wizard_version < 1:
        # Ignore the BERTOS_PATH inside the project file for older project
        project_data["SOURCES_PATH"] = project_dir
    if wizard_version < 2:
        # Use SOURCES_PATH instead of BERTOS_PATH for backward compatibility
        project_data["BERTOS_PATH"] = project_data["SOURCES_PATH"]
    if wizard_version < 3:
        # Use older makefile templates and generators using monkey patching
        import bertos_utils
        bertos_utils.mkGenerator = _mkGenerator
        bertos_utils.userMkGenerator = _userMkGenerator
        bertos_utils.makefileGenerator = _makefileGenerator
    return project_data

