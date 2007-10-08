/**
 * \file
 * <!--
 * Copyright 2003, 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2001 Bernardo Innocenti <bernie@codewiz.org>
 * This file is part of DevLib - See README.devlib for information.
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

#include <cfg/cpu.h>
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
