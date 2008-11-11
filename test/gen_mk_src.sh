#!/bin/bash
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
# Nighlty test for BeRTOS.
#
# Author Daniele Basile <asterix@develer.com>

#Bertos dir, write with escape char for sed regrepx
BERTOS_DIR_RE="\\.\\/bertos"

#Test script dir, where are test script
TEST_SCRIPT_DIR="test"

#Test directory, where are the test makefile
TEST_APP_DIR="examples/test/"

if [ $# \< 1 ] ; then
	printf "You must specify a cpu target!\n\n"
	printf "EXAMPLE:\n $0 <cpu target>\n\n"

	exit 1
fi

#Get list of source for selected cpu target
CSRC=`${TEST_SCRIPT_DIR}/get_source_list.sh $1 c`
ASRC=`${TEST_SCRIPT_DIR}/get_source_list.sh $1 S`
#ASRC=

#kdebug  must added to skip list because it is compiled two times and the linker fail.
CSRC=`echo $CSRC | sed -e "s/${BERTOS_DIR_RE}\\/drv\\/kdebug\\.c//g"`

printf "#This makefile was generate automatically.\n\n" > ${TEST_APP_DIR}/$1_src.mk
printf "${1}_CSRC = $CSRC\n\n" >> ${TEST_APP_DIR}/$1_src.mk
printf "${1}_ASRC = $ASRC\n\n" >> ${TEST_APP_DIR}/$1_src.mk

