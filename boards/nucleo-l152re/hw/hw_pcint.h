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
 * Copyright 2012 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Dataflash HW control routines.
 *
 * \author Tóth Balázs <balazs.toth@jarkon.hu>
 */
 
#ifndef HW_PCINT_H
#define HW_PCINT_H

#include "cfg/cfg_pcint.h"

#define PCINT0_ISR  do{ /*nop*/ } while(0)
#define PCINT1_ISR  do{ /*nop*/ } while(0)
#define PCINT2_ISR  do{ /*nop*/ } while(0)
#define PCINT3_ISR  do{ /*nop*/ } while(0)
	
#if CONFIG_PCINT_STROBE

	#define PCINT_STROBE_INIT 	do{ /*nop*/ } while(0)
	#define PCINT_STROBE_ON   	do{ /*nop*/ } while(0)
	#define PCINT_STROBE_OFF  	do{ /*nop*/ } while(0)

#endif /* CONFIG_PCINT_STROBE */

#endif /* HW_PCINT_H */
