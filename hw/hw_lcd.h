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
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001 Bernardo Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief LCD low-level hardware macros
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2006/09/20 17:39:24  marco
 *#* Low level lcd for avr.
 *#*
 *#*/

#ifndef HW_LCD_H
#define HW_LCD_H

#include <appconfig.h>
//#include <hw.h>

#include <cpu/cpu.h>
#include <cfg/macros.h> /* BV() */
#include <cfg/debug.h>

#include <avr/io.h>
#include <stdbool.h>
#include <inttypes.h>

/**
 * \name LCD I/O pins/ports
 * @{
 */
#define LCD_RS    BV(PG3)
#define LCD_RW    BV(PG0)
#define LCD_E     BV(PG2)
#define LCD_DB0   BV(PA0)
#define LCD_DB1   BV(PA1)
#define LCD_DB2   BV(PA2)
#define LCD_DB3   BV(PA3)
#define LCD_DB4   BV(PA4)
#define LCD_DB5   BV(PA5)
#define LCD_DB6   BV(PA6)
#define LCD_DB7   BV(PA7)
/*@}*/

/**
 * \name DB high nibble (DB[4-7])
 * @{
 */
#define LCD_PORT     PORTG
#define LCD_DB_PORT  PORTA
#define LCD_PIN      PING
#define LCD_DB_PIN   PINA
#define LCD_DDR      DDRG
#define LCD_DB_DDR   DDRA

#if CONFIG_LCD_4BIT
	#define LCD_MASK    (LCD_DB7 | LCD_DB6 | LCD_DB5 | LCD_DB4)
	#define LCD_SHIFT   4
#else
	#define LCD_MASK (uint8_t)0xff
	#define LCD_SHIFT 0
#endif
/*@}*/

/**
 * \name LCD bus control macros
 * @{
 */
#define LCD_CLR_RS      (LCD_PORT &= ~LCD_RS)
#define LCD_SET_RS      (LCD_PORT |=  LCD_RS)
#define LCD_CLR_RD      (LCD_PORT &= ~LCD_RW)
#define LCD_SET_RD      (LCD_PORT |=  LCD_RW)
#define LCD_CLR_E       (LCD_PORT &= ~LCD_E)
#define LCD_SET_E       (LCD_PORT |=  LCD_E)

#if CONFIG_LCD_4BIT
	#define LCD_WRITE_H(x)  (LCD_DB_PORT = (LCD_DB_PORT & ~LCD_MASK) | (((x) >> (4 - LCD_SHIFT)) & LCD_MASK))
	#define LCD_WRITE_L(x)  (LCD_DB_PORT = (LCD_DB_PORT & ~LCD_MASK) | (((x) << LCD_SHIFT) & LCD_MASK))
	#define LCD_READ_H      ((LCD_DB_PIN & LCD_MASK) >> (4 - LCD_SHIFT))
	#define LCD_READ_L      ((LCD_DB_PIN & LCD_MASK) >> LCD_SHIFT)
#else
	#define LCD_WRITE(x) (LCD_DB_PORT = (x))
	#define LCD_READ (LCD_DB_PIN)
#endif
/*@}*/

/** Set data bus direction to output (write to display) */
#define LCD_DB_OUT  (LCD_DB_DDR |= LCD_MASK)

/** Set data bus direction to input (read from display) */
#define LCD_DB_IN   (LCD_DB_DDR &= ~LCD_MASK)

/** Delay for write (Enable pulse width, 220ns) */
#define LCD_DELAY_WRITE \
	do { \
		NOP; \
		NOP; \
		NOP; \
		NOP; \
		NOP; \
	} while (0)

/** Delay for read (Data ouput delay time, 120ns) */
#define LCD_DELAY_READ \
	do { \
		NOP; \
		NOP; \
		NOP; \
		NOP; \
	} while (0)


INLINE void lcd_bus_init(void)
{
	cpuflags_t flags;
	IRQ_SAVE_DISABLE(flags);

	LCD_PORT = (LCD_PORT & ~(LCD_E | LCD_RW)) | LCD_RS;
	LCD_DDR  |= LCD_RS | LCD_RW | LCD_E;

	/*
	 * Data bus is in output state most of the time:
	 * LCD r/w functions assume it is left in output state
	 */
	LCD_DB_OUT;


	IRQ_RESTORE(flags);
}

#endif /* HW_LCD_H */
