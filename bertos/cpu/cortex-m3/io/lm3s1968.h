/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \version $Id$
 *
 * \author Manuele Fanelli <qwert@develer.com>
 *
 * Luminary Micro Stellaris lm3s1968 common definitions.
 * This file is based on NUT/OS implementation. See license below.
 */

#ifndef LM3S1968_H
#define LM3S1968_H

#include <cpu/detect.h>

#if CPU_ARM_LM3S1968
	#include "lm3s1968.h"

#else
	#error Missing I/O definitions for CPU.
#endif

#include <cfg/compiler.h>

#if CPU_ARM_LM3S1968

	#define FLASH_BASE      0x00000000
	#define RAM_BASE        0x20000000

#else
	#error No base addrese register definition for selected ARM CPU

#endif

//TODO: add other peripherals





#endif /* LM3S1968_H */
