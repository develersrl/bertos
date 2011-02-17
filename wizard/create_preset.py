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
# Copyright 2010 Develer S.r.l. (http://www.develer.com/)
#
# Author: Francesco Sacchi <batt@develer.com>
#

import pickle
import sys
import os
import shutil
import pprint
import glob
import re

def remove(path):
    if os.path.isdir(path):
        shutil.rmtree(path, ignore_errors=True)
    else:
        try:
            os.remove(path)
        except OSError:
            pass

def findPath(start, target):
    pa = start
    while pa != "/":
        pa = os.path.abspath(pa + "/..")
        if os.path.exists(pa + "/" + target):
            return os.path.relpath(pa, start)

if len(sys.argv) < 3:
    print "Usage: %s <project_dir> <preset_dir>" % sys.argv[0]
    exit(0)

prj_dir = sys.argv[1]
preset_dir = sys.argv[2]

if not os.path.exists(prj_dir + "/project.bertos"):
    print "%s does not seem a Wizard created project." % prj_dir
    exit(1)

p = open(prj_dir + "/project.bertos")
s = pickle.load(p)
if s["WIZARD_VERSION"] < 3:
    print "Project version too old."
    exit(1)
pname = s["PROJECT_NAME"] 
preset_dir += pname

#find hw/ path for the board
hw_path = findPath(preset_dir, "hw")
if not hw_path:
    print "hw/ path not found in parent directories of %s" % preset_dir
    exit(1)

#find bertos/ path
bertos_path = findPath(preset_dir, "bertos")
if not bertos_path:
    print "bertos/ path not found in parent directories of %s" % preset_dir
    exit(1)

#Copy the project and remove unneeded files.
shutil.copytree(prj_dir, preset_dir)
remove(preset_dir + "/bertos")
remove(preset_dir + "/images")
remove(preset_dir + "/obj")
remove(preset_dir + "/Makefile")
remove(preset_dir + "/buildrev.h")
remove(preset_dir + "/VERSION")
remove(preset_dir + "/" + pname + ".project")
remove(preset_dir + "/" + pname + ".workspace")

#Flatten project sources.
#A wizard created project called pname
#usually has the following structure:
#
#pname
#  |
#  +-<project files>
#  |
#  +-bertos
#  |
#  +-pname
#      |   
#      +<pname sources>
#
#This has been done in order to have the chance to
#add multiple projects sharing the same bertos version.
#After the copy and after removing the bertos tree inside the 
#project, the double pname directory is redundant, so we move
#all pname sources into the parent directory
l = glob.glob(preset_dir + "/" + pname + "/*")
for f in l:
    shutil.move(f, preset_dir)

#Remove the now empty project src dir and the hw/ dir.
#hw files are located in parent directories and are common
#for all projects on the same board. 
remove(preset_dir + "/" + pname)
remove(preset_dir + "/hw")

assert(os.path.exists(preset_dir + "/" + hw_path     + "/hw"))
assert(os.path.exists(preset_dir + "/" + bertos_path + "/bertos"))

#Update wizard project info.
#A preset is still a Wizard-editable project
#but has its bertos/ and hw/ dirs shared with 
#others.
s["BERTOS_PATH"] = bertos_path
s["PROJECT_HW_PATH"] = hw_path
s["PROJECT_SRC_PATH"] = "."
s["PRESET"] = True

# Calculate relative paths useful to discover where are the sources (and the hw
# files) relatively to the BeRTOS Makefile
bertos_path = os.path.abspath(preset_dir + "/" + bertos_path)
hw_path = os.path.abspath(preset_dir + "/" + hw_path)

src_path = os.path.relpath(preset_dir, bertos_path)
hw_path  = os.path.relpath(hw_path, bertos_path)

# Src path and hw path relatively to the BeRTOS Makefile.
s["PROJECT_SRC_PATH_FROM_MAKEFILE"] = src_path
s["PROJECT_HW_PATH_FROM_MAKEFILE"] = hw_path
toolchain = s["TOOLCHAIN"]["path"]
s["TOOLCHAIN"]["path"] = os.path.basename(toolchain)

pprint.pprint(s)
p = open(preset_dir + "/project.bertos", "w")
pickle.dump(s, p)

#Create a .spec file in order to make this preset visible in the Wizard
open(preset_dir + "/.spec", "w").write("name = '%s preset'" % pname)

#Update project makefiles adapting them to the new directory layout
mk = open(preset_dir + "/" + pname + ".mk").read()
mk = re.sub(r"(%s_SRC_PATH\s*=\s*).*" % pname, r"\1%s" % src_path, mk)
mk = re.sub(r"(%s_HW_PATH\s*=\s*).*" % pname, r"\1%s" % hw_path, mk)

#remove absolute path from toolchain
mk = re.sub(r'(%s_PREFIX\s*=\s*").*?([^/]*")' % pname, r'\1\2', mk)
open(preset_dir + "/" + pname + ".mk", "w").write(mk)
