#! /usr/bin/env bash
set -x

BASE_DIR=`dirname $0`
INT_FILE=${BASE_DIR}/openocd/${PROGRAMMER_TYPE}.cfg 
CPU_FILE=${BASE_DIR}/openocd/${PROGRAMMER_CPU}.cfg 
OUT_FILE="openocd.tmp"

if [ ! -f ${INT_FILE} ]; then
	printf "Interface ${PROGRAMMER_TYPE} not supported\n";
	exit 1;
fi

if [ ! -f ${CPU_FILE} ]; then
	printf "CPU ${PROGRAMMER_CPU} not supported\n";
	exit 1;
fi

sed -e "s#PROGRAMMER_TYPE#${INT_FILE}#" ${BASE_DIR}/openocd/flash.cfg | sed -e "s#PROGRAMMER_CPU#${CPU_FILE}#" \
	| sed -e "s#IMAGE_FILE#${IMAGE_FILE}#" > ${OUT_FILE}

openocd -f ${OUT_FILE}
OCD_RET=$?
rm -f ${OUT_FILE}
exit $OCD_RET
