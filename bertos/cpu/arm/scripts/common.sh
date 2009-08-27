BASE_DIR=`dirname $0`
INT_FILE=${BASE_DIR}/openocd/${PROGRAMMER_TYPE}.cfg 
CPU_FILE=${BASE_DIR}/openocd/${PROGRAMMER_CPU}.cfg 

if [ ! -f ${INT_FILE} ]; then
	printf "Interface ${PROGRAMMER_TYPE} not supported\n";
	exit 1;
fi

if [ ! -f ${CPU_FILE} ]; then
	printf "CPU ${PROGRAMMER_CPU} not supported\n";
	exit 1;
fi

