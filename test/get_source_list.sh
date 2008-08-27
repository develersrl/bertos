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

#Directories
BERTOS_DIR="./bertos"
CPU_DIR="${BERTOS_DIR}/cpu"

#Directory to exclude
OS_DIR="${BERTOS_DIR}/os"
EMUL_DIR="${BERTOS_DIR}/emul"
APP_DIR="./app"

if [ $# \< 2 ] ; then
	printf "You must specify a cpu target, and source file type!\n"
	printf "EXAMPLE:\n $0 <cpu target> <source type>\n\n"

	exit 1
fi
CPU_TARGET=$1
#Create a list of source file whitout a cpu specific source
GEN_SRC=`find . \( -name \.svn -prune -o -path $CPU_DIR -prune -o -path $APP_DIR  -prune -o -path $OS_DIR -prune -o -path $EMUL_DIR -prune \) -o -name *.${2} -print | xargs`

#Select c and asm sources for selected cpu target
TRG_SRC=`find ${CPU_DIR}/$CPU_TARGET -name \.svn -prune -o -name *.${2} -print | xargs`

#Generate a list of all source for each cpu
SRC_ALL=${GEN_SRC}" "${TRG_SRC}

# Find the files that contain the string
# NOTEST <cpu target> and put it in a list

NOTEST="notest:"
for src in $SRC_ALL ;
do
	grep "$NOTEST\s*$CPU_TARGET" $src 2>&1 > /dev/null
	if [ $? = 0 ] ; then
		EXCLUDE_LIST+=$src
	fi	
done

# Remove the exclude list files from the sources to be
# compiled
for src in $EXCLUDE_LIST ;
do
	SRC_ALL=`echo $SRC_ALL | sed -e "s;$src;;g"`
done

printf "${SRC_ALL}"

