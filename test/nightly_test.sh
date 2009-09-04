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

#Makefile test directory
MAKEFILE_TEST_DIR="."
TEST_DIR="test"
BERTOS_DIR="bertos/"
BERTOS_DIR_BAK="bertos.saved"
OPTS=1
if [ $# = 1 ] ; then
	if [ $1 = "-h" ] ; then
		printf "Nightly test help.\n"
		printf "\t--no-silent disable the silent of the TODO/FIXME warning messages.\n"
		exit 1
	elif [ $1 = "--no-silent" ] ; then
		OPTS=0
	fi
fi

if [ $OPTS = 1 ] ; then
#Copy BeRTOS sources
printf "Starting nightlytest..\n"
	printf "Silent mode enabled: removing the TODO/FIXME messages.\n"
	cp -R $BERTOS_DIR $BERTOS_DIR_BAK || exit 1

	#Strip away TODOs and FIXME
	find $BERTOS_DIR -name "*.[ch]" | xargs perl -p -i -e 's/^\s*#warning\s*(TODO|FIXME).*//g;'
	CFG_LIST=`find $BERTOS_DIR/cfg -name "cfg_*.h"`
	for i in $CFG_LIST ;
	do
		$TEST_DIR/enablecfg.py $i
	done
fi

#Cpu targets that we want to test
TRG="avr arm"

for i in $TRG ;
do
	${TEST_DIR}/gen_mk_src.sh $i
done

#Clean and launch make on all
make -f ${MAKEFILE_TEST_DIR}/Makefile.test clean
make -f ${MAKEFILE_TEST_DIR}/Makefile.test
RET=$?
if [ $OPTS = 1 ] ; then
	#Restore original sources
	if [ -d $BERTOS_DIR_BAK ] ; then
		printf "Exiting from nightly..\n"
		rm -rf $BERTOS_DIR
		mv $BERTOS_DIR_BAK $BERTOS_DIR
	else
		printf "Unable to restore backup copy\n"
		exit 1
	fi
fi

printf "Nightly test done.\n\n"
exit $RET

