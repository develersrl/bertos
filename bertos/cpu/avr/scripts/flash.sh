#!/usr/bin/env bash

avrdude -p ${PROGRAMMER_CPU} -c ${PROGRAMMER_TYPE} -P ${PROGRAMMER_PORT} -U ${IMAGE_FILE}
