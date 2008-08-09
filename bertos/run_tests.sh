#!/bin/bash
#
# Copyright 2005 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Author: Bernie Innocenti <bernie@codewiz.org>
#
# $Id$
#

VERBOSE=1

CC=gcc
CFLAGS="-W -Wall -Wextra -I. -Iemul -std=gnu99 -fno-builtin -D_DEBUG -D_TEST -DARCH=ARCH_EMUL"

CXX=g++
CXXFLAGS="$CFLAGS"

TESTS=${TESTS:-`find . -name "*_test.c*"`}

for test in $TESTS; do
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

