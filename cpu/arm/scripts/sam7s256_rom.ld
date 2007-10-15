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
 * \version $Id: sysirq_at91.c 18273 2007-10-11 14:53:02Z batt $
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief Script linker for Atmel AT91 SAM7S256 processors.
 *
 */


ENTRY(ResetHandler)
STARTUP(crtat91sam7x256_rom.o)  /* DA SOSTITUIRE */
SEARCH_DIR(.)
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY
{
  rom(rx) : org = 0x00000000, len = 256k
  ram(rw) : org = 0x00200000, len = 64k
}

SECTIONS
{
    .text :
    {
	    * ( .vectors );
	    . = ALIGN ( 4 );
	    * ( .init );
	    . = ALIGN ( 4 );
	    * ( .rodata );
	    . = ALIGN ( 4 );
	    * ( .rodata* );
	    . = ALIGN ( 4 );
	} > rom

	. = ALIGN ( 4 );

	_etext = .;
	PROVIDE ( __etext = . );

	.data : AT ( __etext )
	{
		PROVIDE ( __data_start = . );
		* ( .data )
		. = ALIGN ( 4 );
		* ( .ramfunc )
		_edata = .;
		PROVIDE ( __data_end = . );
	}
	> ram

	.bss :
	{
		PROVIDE ( __bss_start = . );
		* ( .bss )
		* ( COMMON )
		. = ALIGN ( 4 );
		PROVIDE ( __bss_end = . );
		. += 0x400;
		PROVIDE ( __exp_stack = . );
		. += 0x400;
		PROVIDE ( __stack = . );
		PROVIDE ( __heap_start = . );
	} > ram

}
