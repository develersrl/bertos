#!/bin/bash
#
# Copyright 2005 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Author: Bernardo Innocenti <bernie@develer.com>
#
# $Id$
#


VERBOSE=1

CC=gcc
CFLAGS="-W -Wall -Wextra -I. -fno-builtin -D_DEBUG -D_TEST"

CXX=g++
CXXFLAGS="$CFLAGS"

for test in `find . -name "*_test.*"`; do
	[ $VERBOSE -gt 0 ] && echo "Running $test..."
	case "$test" in
	*.cpp)
		$CXX $CXXFLAGS $test -o test || exit 1
		./test || exit 1
		rm -f test
	;;
	*.c)
		$CC $CFLAGS $test -o test || exit 1
		./test || exit 1
		rm -f test
	;;
	esac
done

