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
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#


# This variable contains the wizard version. This will be stored into the 
# project files to let the Wizard know how to use them.
# Previous versions:
#   0 - the project file doesn't contain the version number.
#       When the current Wizard is opening project done with this version of
#       the Wizard it has to change the original SOURCES_PATH to the local path
#   1 - SOURCES_PATH used instead of BERTOS_PATH
#   2 - Use the old makefile templates (into /mktemplates/old) and the old logic
#       Instead of the new one.
#   3 - Current version.


# NOTE: Change this variable may cause the wizard to not work properly. 
WIZARD_VERSION = 3
