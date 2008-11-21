#!/bin/bash
#
# Copyright 2005 Develer S.r.l. (http://www.develer.com/)
# Copyright 2008 Bernie Innocenti <bernie@codewiz.org>
#
# Version: $Id$
# Author:  Bernie Innocenti <bernie@codewiz.org>
#

# Testsuite output level:
#  0 - quiet
#  1 - progress output
#  2 - build warnings
#  3 - execution output
#  4 - build commands
VERBOSE=${VERBOSE:-1}

CC=gcc
#FIXME: -Ibertos/emul should not be needed
CFLAGS="-W -Wall -Wextra -Ibertos -Ibertos/emul -std=gnu99 -fno-builtin -D_DEBUG -DARCH=(ARCH_EMUL|ARCH_UNITTEST)"

CXX=g++
CXXFLAGS="$CFLAGS"

TESTS=${TESTS:-`find . \
	\( -name .svn -prune -o -name .git -prune -o -name .hg  -prune \) \
	-o -name "*_test.c*" -print` }

TESTOUT="testout"

buildout='/dev/null'
runout='/dev/null'
[ "$VERBOSE" -ge 2 ] && buildout='/dev/stdout'
[ "$VERBOSE" -ge 3 ] && runout='/dev/stdout'

# Needed to get build/exec result code rather than tee's
set -o pipefail

rm -rf "${TESTOUT}.old"
mv -f "${TESTOUT}" "$TESTOUT.old"
mkdir -p "$TESTOUT"

for src in $TESTS; do
	name="`basename $src | sed -e 's/\.cpp$//' -e 's/\.c$//'`"
	exe="./$TESTOUT/$name"

	case "$src" in
	*.cpp) BUILDCMD="$CXX $CXXFLAGS $src -o $exe" ;;
	*.c)   BUILDCMD="$CC  $CXXFLAGS $src -o $exe" ;;
	esac

	[ $VERBOSE -gt 0 ] && echo "Building $name..."
	[ $VERBOSE -gt 4 ] && echo " $BUILDCMD"
	if $BUILDCMD 2>&1 | tee >$buildout $TESTOUT/$name.build; then
		[ $VERBOSE -gt 0 ] && echo "Running $name..."
		if ! $exe 2>&1 | tee >$runout $TESTOUT/$name.out; then
			echo "FAILED [RUN]: $name"
		fi
	else
		echo "FAILED [BUILD]: $name"
	fi
done

