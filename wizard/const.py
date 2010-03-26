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

import os, sys

_tmp = sys.argv[0]
if os.path.islink(_tmp):
    _tmp = os.readlink(_tmp)
DATA_DIR = os.path.dirname(os.path.abspath(_tmp))
del _tmp

CPU_DEF = {
    "CPU_NAME": "",
    "CPU_DIR": "",
    "DEFINITION_PATH": "",
    "TOOLCHAIN": "",
    "CPU_TAGS": [],
    "CPPA_SRC" : [],
    "CXX_SRC": [],
    "ASRC": [],
    "C_SRC": [],
    "PC_SRC" : [],
    "CPU_DESC" : [],
    "CPU_DEFAULT_FREQ": "1000000",
    "GDB_INIT_SCRIPT": "",
}

TOOLCHAIN_ITEMS = ("ld", "as")

CPU_DEFINITION = "*.cdef"

GCC_NAME = "*gcc*"

MODULE_CONFIGURATION = "cfg_*.h"

UI_LOCATION = "ui"

EXTENSION_FILTER = (
    ".c",
    ".cpp",
    ".cxx",
    ".h",
    ".c++",
    ".ld",
    ".S",
    ".mk",
    "Makefile",
)

IGNORE_LIST = (
    ".svn",
    "CVS",
    ".git",
)

MODULE_DEFINITION = {
    "module_name": "module_name",
    "module_configuration": "module_configuration",
    "module_depends": "module_depends",
    "module_harvard": "module_harvard",
    "module_hw": "module_hw",
    "module_supports": "module_supports",
}

MK_PARAM_ID = "MK_"
