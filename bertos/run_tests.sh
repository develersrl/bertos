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

TESTS=${TESTS:-`find . \
	\( -name .svn -prune -o -name .git -prune -o -name .hg  -prune \) \
	-o -name "*_test.c*" -print` }

TESTOUT="testout"

mkdir -p "$TESTOUT"

for src in $TESTS; do
	name="`basename $src | sed -e 's/\.cpp$//' -e 's/\.c$//'`"
	exe="./$TESTOUT/$name"

	case "$src" in
	*.cpp) BUILDCMD="$CXX $CXXFLAGS $src -o $exe" ;;
	*.c)   BUILDCMD="$CC  $CXXFLAGS $src -o $exe" ;;
	esac

	[ $VERBOSE -gt 0 ] && echo "Building $name..."
	$BUILDCMD 2>&1 | tee $name.build || echo "FAILED [BUILD]: $name"

	[ $VERBOSE -gt 0 ] && echo "Running $name..."
	$exe 2>&1 | tee $name.out || echo "FAILED [RUN]: $name"
	#rm -f "$exe"
done

