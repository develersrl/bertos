#
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile template for BeRTOS wizard.
#
# Author: Lorenzo Berni <duplo@develer.com>
#
#

# Set to 1 for debug builds
$pname_DEBUG = 1

# Our target application
TRG += $pname

$pname_PREFIX = $prefix

$pname_SUFFIX = $suffix

$pname_MCU = $cpuname

$pname_CSRC = \
	$csrc
	#

$pname_PCSRC += $pcsrc


$pname_CFLAGS = $cflags
$pname_LDFLAGS = $ldflags
