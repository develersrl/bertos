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
CFLAGS="-W -Wall -Wextra -Ibertos -Ibertos/emul -std=gnu99 -fno-builtin -D_DEBUG -D_TEST -DARCH=ARCH_EMUL"

CXX=g++
CXXFLAGS="$CFLAGS"

TESTS=${TESTS:-`find . -name "*_test.c*"`}

for test in $TESTS; do
	[ $VERBOSE -gt 0 ] && echo "Running $test..."
	case "$test" in
	*.cpp)
		$CXX $CXXFLAGS $test -o images/testcase || exit 1
		./testcase || echo "FAILED: $test"
		rm -f testcase
	;;
	*.c)
		$CC $CFLAGS $test -o testcase || exit 1
		./testcase || echo "FAILED: $test"
		rm -f testcase
	;;
	esac
done

