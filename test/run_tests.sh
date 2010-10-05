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
CFLAGS="-W -Wall -Wextra -Wundef -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -Wsign-compare -Wmissing-noreturn \
-O0 -g3 -ggdb -Ibertos -Ibertos/emul -std=gnu99 -fno-builtin -D_DEBUG -DARCH=(ARCH_EMUL|ARCH_UNITTEST) \
-DCPU_FREQ=(12288000UL) -ffunction-sections -fdata-sections -Wl,--gc-sections"

CXX=g++
CXXFLAGS="$CFLAGS"

TESTS=${TESTS:-`find . \
	\( -name .svn -prune -o -name .git -prune -o -name .hg  -prune \) \
	-o -name "*_test.c" -print` }

TESTOUT="testout"
SRC_LIST="
	bertos/algo/ramp.c
	bertos/drv/kdebug.c
	bertos/drv/timer.c
	bertos/kern/monitor.c
	bertos/kern/proc.c
	bertos/kern/signal.c
	bertos/kern/sem.c
	bertos/kern/preempt.c
	bertos/mware/event.c
	bertos/mware/formatwr.c
	bertos/mware/hex.c
	bertos/mware/sprintf.c
	bertos/os/hptime.c
	bertos/struct/kfile_fifo.c
	bertos/struct/heap.c
	bertos/struct/hashtable.c
	bertos/fs/fatfs/ff.c
	bertos/emul/diskio_emul.c
	bertos/fs/fat.c
	bertos/fs/battfs.c
	bertos/emul/switch_ctx_emul.S
	bertos/mware/ini_reader.c
	bertos/emul/kfile_posix.c
	bertos/algo/crc_ccitt.c
	bertos/algo/crc.c
	bertos/struct/kfile_mem.c
	bertos/net/ax25.c
	bertos/net/afsk.c
	bertos/net/nmeap/src/nmeap01.c
	bertos/net/nmea.c
	bertos/cfg/kfile_debug.c
	bertos/io/kblock.c
	bertos/io/kblock_ram.c
	bertos/io/kblock_posix.c
	bertos/io/kfile.c
	bertos/sec/cipher.c
	bertos/sec/cipher/blowfish.c
	bertos/sec/cipher/aes.c
	bertos/sec/kdf/pbkdf1.c
	bertos/sec/kdf/pbkdf2.c
	bertos/sec/hash/sha1.c
	bertos/sec/hash/md5.c
	bertos/sec/hash/ripemd.c
	bertos/sec/mac/hmac.c
	bertos/sec/mac/omac.c
"

buildout='/dev/null'
runout='/dev/null'
[ "$VERBOSE" -ge 2 ] && buildout='/dev/stdout'
[ "$VERBOSE" -ge 3 ] && runout='/dev/stdout'

# Needed to get build/exec result code rather than tee's
set -o pipefail

rm -rf "${TESTOUT}.old"
if [ -d "${TESTOUT}" ] ; then
	mv -f "${TESTOUT}" "$TESTOUT.old"
fi
mkdir -p "$TESTOUT"

for src in $TESTS; do
	name="`basename $src | sed -e 's/\.cpp$//' -e 's/\.c$//'`"
	testdir="./$TESTOUT/$name"
	cfgdir="$testdir/cfg"
	mkdir -p "$cfgdir"
	exe="$testdir/$name"

	PREPARECMD="test/parsetest.py $src"
	BUILDCMD="$CC -I$testdir $CFLAGS $src $SRC_LIST -o $exe"
	export testdir name cfgdir

	[ $VERBOSE -gt 0 ] && echo "Preparing $name..."
	[ $VERBOSE -gt 4 ] && echo " $PREPARECMD"
	if $PREPARECMD 2>&1 | tee -a >>$buildout $testdir/$name.prep; then
		[ $VERBOSE -gt 0 ] && echo "Building $name..."
		[ $VERBOSE -gt 4 ] && echo " $BUILDCMD"
		if $BUILDCMD 2>&1 | tee -a >>$buildout $testdir/$name.build; then
			[ $VERBOSE -gt 0 ] && echo "Running $name..."
			if ! $exe 2>&1 | tee -a >>$runout $testdir/$name.out; then
				echo "FAILED [RUN]: $name"
				cat $testdir/$name.out
				exit 2
			fi
		else
			echo "FAILED [BUILD]: $name"
			cat $testdir/$name.build
			exit 3
		fi
	else
		echo "FAILED [PREPARING]: $name"
		cat $testdir/$name.prep
		exit 4
	fi
done

