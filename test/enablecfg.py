#!/usr/bin/env python
# This file is part of BeRTOS.
#
# Bertos is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this
# file does not by itself cause the resulting executable to be covered by
# the GNU General Public License.  This exception does not however
# invalidate any other reasons why the executable file might be covered by
# the GNU General Public License.
#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
#
# Author David Mugnai <dvd@develer.com>

# This file scan a module configuration file (cfg_module.h) 
# and tries to enable all configuration options.
# This is useful in nightly test to test all possible configurations.

# A whitelist of options that should not be enabled is supplied.

import sys
import re

# Options that should be left disabled
whitelist = [
	'CONFIG_BATTFS_SHUFFLE_FREE_PAGES',
	'CONFIG_FAT_FS_READONLY',
	'CONFIG_INTERNAL_COMMANDS',
	'CONFIG_KERN_IRQ',
	'CONFIG_KERN_HEAP',
	'CONFIG_KERN_PREEMPT',
	'RAMP_USE_FLOATING_POINT',
	'CONFIG_SER_HWHANDSHAKE',
	'CONFIG_KDEBUG_PORT',
]

tests = [
	'#define T_0 0\n',
	'#define T_1 0\n',
	'    #define    	T_2    	0		\n',
	'#define T_3 0    /* */\n',
	'#define T_4 0/* */\n',
]
no_tests = [
	'#define T_1 0A\n',
	'#define T_1 0x0A/* */\n',
	'#define T_1 0UL /* */\n',
	'#define T_1 0UL/* */\n',
]
pattern = r'\s*#define\s+(\w+)\s+(0)(?:\s+|/|$)'

def f(match):
	if match.group(1) in whitelist:
		return match.group(0)
	else:
		data = match.group(0)
		sx = match.start(2) - match.start(0)
		ex = match.end(2) - match.end(0)
		return data[:sx] + '1' + data[ex:]

if len(sys.argv) == 1:
	for t in tests:
		print t, re.subn(pattern, f, t)[0]
	print '-' * 42
	for t in no_tests:
		print t, re.subn(pattern, f, t)[0]
else:
	data = file(sys.argv[1]).read()
	data, count = re.subn(pattern, f, data)
	if count:
		file(sys.argv[1], 'w').write(data)
