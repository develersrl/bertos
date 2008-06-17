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

LOG_FILE="out"
DISPLAY_LINE="0"
HIDE_WARNING="TODO"
SHOW_MSG='warning|error'


if [ $# = 1 ] ; then
	printf "Use the default settings.\n"
else
	for i in "$@" ;
	do
		case $i in
		"-file")
			LOG_FILE=$2
			;;

		"-hide")
			HIDE_WARNING=$2
			;;
		"-show")
			SHOW_MSG=$2
			shift
			;;
		"-n")
			echo "n" $2
			DISPLAY_LINE=$2
			;;
		esac
	
		#skip to next srgument
		shift
	done

fi

printf "Current settings:\n"
printf "Log file: "${LOG_FILE}"\n"
printf "Show message: "${SHOW_MSG}"\n"
printf "Hide warning message: "${HIDE_WARNING}"\n"
printf "Display line around to warning/error message: "${DISPLAY_LINE}"\n" 

if [ $DISPLAY_LINE = 0 ] ; then
	GREP_ARGS=""
else
	GREP_ARGS="-n"${DISPLAY_LINE}
fi


tail -f ${LOG_FILE} | grep -v ${HIDE_WARNING} | sed -e "s/^/vi /" | sed -e "s/:/ +/" | grep $GREP_ARGS -E ${SHOW_MSG}


