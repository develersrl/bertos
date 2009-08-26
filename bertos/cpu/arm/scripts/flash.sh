#! /usr/bin/env bash
set -x

BASE_DIR=`dirname $0`
INT_FILE=${BASE_DIR}/openocd/${PROGRAMMER_TYPE}.tcl 
CPU_FILE=${BASE_DIR}/openocd/${PROGRAMMER_CPU}.tcl 

if [ ! -f ${INT_FILE} ]; then
	printf "Interface ${PROGRAMMER_TYPE} not supported\n";
	exit 1;
fi

if [ ! -f ${CPU_FILE} ]; then
	printf "CPU ${PROGRAMMER_CPU} not supported\n";
	exit 1;
fi

sed -e "s#PROGRAMMER_TYPE#${INT_FILE}#" ${BASE_DIR}/openocd/flash.cfg | sed -e "s#PROGRAMMER_CPU#${CPU_FILE}#" > openocd.tmp
