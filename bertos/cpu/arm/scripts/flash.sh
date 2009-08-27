#! /usr/bin/env bash
#set -x

BASE_DIR=`dirname $0`
. ${BASE_DIR}/common.sh

OUT_FILE="openocd.tmp"

sed -e "s#PROGRAMMER_TYPE#${INT_FILE}#" ${BASE_DIR}/openocd/flash.cfg | sed -e "s#PROGRAMMER_CPU#${CPU_FILE}#" \
	| sed -e "s#IMAGE_FILE#${IMAGE_FILE}#" > ${OUT_FILE}

openocd -d 1 -f ${OUT_FILE}
OCD_RET=$?
rm -f ${OUT_FILE}
exit $OCD_RET
