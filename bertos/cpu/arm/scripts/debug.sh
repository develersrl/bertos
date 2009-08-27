#! /usr/bin/env bash
set -x
BASE_DIR=`dirname $0`
. ${BASE_DIR}/common.sh

OUT_FILE="openocd.tmp"

sed -e "s#PROGRAMMER_TYPE#${INT_FILE}#" ${BASE_DIR}/openocd/debug.cfg | sed -e "s#PROGRAMMER_CPU#${CPU_FILE}#" > ${OUT_FILE}

openocd -f ${OUT_FILE}
OCD_RET=$?
rm -f ${OUT_FILE}
exit $OCD_RET
