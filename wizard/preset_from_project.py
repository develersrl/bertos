#!/usr/bin/env python
# encoding: utf-8
#
# This file is part of bertos.
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

import sys
import os
import shutil
import pickle

from optparse import OptionParser

def createBertosPreset(source, destination):
    os.makedirs(destination)
    project_file = os.path.join(source, "project.bertos")
    shutil.copy(project_file, destination)
    project_info = pickle.loads(open(project_file, "r").read())
    try:
        user_project_dir = os.path.join(source, project_info["PROJECT_NAME"])
    except KeyError:
        user_project_dir = os.path.join(source, os.path.basename(source))
        if not os.path.exists(user_project_dir):
            print "Project in %s is not well formed" %source
    hw_dir = os.path.join(user_project_dir, "hw")
    new_hw_dir = os.path.join(destination, "hw")
    shutil.copytree(hw_dir, new_hw_dir)
    cfg_dir = os.path.join(user_project_dir, "cfg")
    new_cfg_dir = os.path.join(destination, "cfg")
    shutil.copytree(cfg_dir, new_cfg_dir)
    main_file = os.path.join(user_project_dir, "main.c")    
    new_main_file = os.path.join(destination, "main.c")
    shutil.copy(main_file, new_main_file)
    print "Preset correctly created: REMEMBER TO WRITE THE description FILE INTO THE PRESET DIRECTORY"

def main():
    args = sys.argv
    if len(args) != 3:
        print 'Invalid usage:\nuse: ./preset_from_project.py PROJECT_DIRECTORY DESTINATION_DIRECTORY'
        return
    if not os.path.exists(args[1]):
        print 'The given project directory (%s) doesn\'t exists' %args[1]
        return
    if not os.path.exists(os.path.join(args[1], 'project.bertos')):
        print 'No project.bertos file found in %s' %args[1]
        return
    if os.path.exists(args[2]):
        print "Destination directory (%s) already exists" %args[2]
    createBertosPreset(args[1], args[2])

if __name__ == '__main__':
    main()

