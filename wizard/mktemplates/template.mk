#
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile template for BeRTOS wizard.
#
# Author: Lorenzo Berni <duplo@develer.com>
#
#

# Constants automatically defined by the selected modules
$constants

# Our target application
TRG += $pname

$pname_PREFIX = $prefix

$pname_SUFFIX = $suffix

$pname_CROSS = $cross

$pname_MCU = $cpuname

$pname_CSRC = \
	$csrc
	$main \
	#

$pname_PCSRC += \
	$pcsrc
	#

$pname_CPPASRC += \
	$asrc
	#

$pname_CFLAGS = $cflags
$pname_LDFLAGS = $ldflags
