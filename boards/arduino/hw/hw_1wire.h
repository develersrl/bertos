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
 *  Copyright (C) 2012 Robin Gilks, Peter Dannegger, Martin Thomas
 * -->
 *
 * \brief Driver for Dallas 1-wire devices
 *
 *
 * \author Peter Dannegger (danni(at)specs.de)
 * \author Martin Thomas (mthomas(at)rhrk.uni-kl.de)
 * \author Robin Gilks (g8ecj(at)gilks.org)
 *
 * $WIZ$ module_name = "hw_1wire"
 */

#ifndef HW_1WIRE_H_
#define HW_1WIRE_H_

#include "drv/ow_1wire.h"
#include "cfg/cfg_1wire.h"
#include "cfg/compiler.h"

#include <stdint.h>
#include <avr/io.h>

#include <drv/timer.h>
#include <cpu/irq.h>



// if all devices are hanging off one pin (or there is only one device)
#if OW_ONE_BUS == 1

#define OW_GET_IN()   ( OW_IN & (1<<OW_PIN))
#define OW_OUT_LOW()  ( OW_OUT &= (~(1 << OW_PIN)) )
#define OW_OUT_HIGH() ( OW_OUT |= (1 << OW_PIN) )
#define OW_DIR_IN()   ( OW_DDR &= (~(1 << OW_PIN )) )
#define OW_DIR_OUT()  ( OW_DDR |= (1 << OW_PIN) )

#define OW_CONF_DELAYOFFSET 0

#else

/*******************************************/
/* Hardware connection                     */
/*******************************************/

/* Define OW_ONE_BUS if only one 1-Wire-Bus is used
   in the application -> shorter code.
   If not defined make sure to call ow_set_bus() before using 
   a bus. Runtime bus-select increases code size by around 300 
   bytes so use OW_ONE_BUS if possible */


#if ( CPU_FREQ < 1843200 )
#warning | Experimental multi-bus-mode is not tested for
#warning | frequencies below 1,84MHz. Use OW_ONE_WIRE or
#warning | faster clock-source (i.e. internal 2MHz R/C-Osc.).
#endif
#define OW_CONF_CYCLESPERACCESS 13
#define OW_CONF_DELAYOFFSET ( (uint16_t)( ((OW_CONF_CYCLESPERACCESS) * 1000000L) / CPU_FREQ ) )

/* set bus-config with ow_set_bus() */
uint8_t OW_PIN_MASK;
volatile uint8_t *OW_IN;
volatile uint8_t *OW_OUT;
volatile uint8_t *OW_DDR;

#define OW_GET_IN()   ( *OW_IN & OW_PIN_MASK )
#define OW_OUT_LOW()  ( *OW_OUT &= (uint8_t) ~OW_PIN_MASK )
#define OW_OUT_HIGH() ( *OW_OUT |= (uint8_t)  OW_PIN_MASK )
#define OW_DIR_IN()   ( *OW_DDR &= (uint8_t) ~OW_PIN_MASK )
#define OW_DIR_OUT()  ( *OW_DDR |= (uint8_t)  OW_PIN_MASK )

/**
 * Set the port/data direction input pin dynamically
 *
 * \param in input port
 * \param out output port
 * \param ddr data direction register
 * \param pin I/O pin (bit number on port)
 *
 */
void
ow_set_bus(volatile uint8_t * in, volatile uint8_t * out, volatile uint8_t * ddr, uint8_t pin)
{
	OW_DDR = ddr;
	OW_OUT = out;
	OW_IN = in;
	OW_PIN_MASK = (1 << pin);
	ow_reset();
}

#endif

// now follows the functions that run at the lowest level and are only referenced from the ow_1wire driver module

/**
 * Get the state of an input pin
 *
 * \return I/O pin value
 */
INLINE uint8_t
ow_input_pin_state(void)
{
	return OW_GET_IN();
}


/**
 * Enable parasitic mode (set line high to power device)
 *
 */
INLINE void
ow_parasite_enable(void)
{
	OW_OUT_HIGH();
	OW_DIR_OUT();
}

/**
 * Disable parasitic mode
 *
 */
INLINE void
ow_parasite_disable(void)
{
	OW_DIR_IN();
#if (!OW_USE_INTERNAL_PULLUP)
	OW_OUT_LOW();
#endif
}


/**
 * Disable parasitic mode
 *
 * \return non zero = error code
 */
INLINE uint8_t
ow_reset_intern(void)
{
	uint8_t err;

	OW_OUT_LOW();
	OW_DIR_OUT();
	// pull OW-Pin low for 480us
	timer_udelay(480);

	ATOMIC(
				// set Pin as input - wait for clients to pull low
				OW_DIR_IN();
#if OW_USE_INTERNAL_PULLUP
				OW_OUT_HIGH();
#endif
				timer_udelay(64);
				// no presence detect
				err = OW_GET_IN();
				// if err!=0: nobody pulled to low, still high
		);

	// after a delay the clients should release the line
	// and input-pin gets back to high by pull-up-resistor
	timer_udelay(480 - 64);
	if (OW_GET_IN() == 0)
	{
		// short circuit, expected low but got high
		err = 1;
	}

	return err;
}


/**
 * Internal function to output a bit
 * Timing issue when using runtime-bus-selection (!OW_ONE_BUS):
 * The master should sample at the end of the 15-slot after initiating
 * the read-time-slot. The variable bus-settings need more
 * cycles than the constant ones so the delays had to be shortened 
 * to achive a 15uS overall delay 
 * Setting/clearing a bit in I/O Register needs 1 cyle in OW_ONE_BUS
 * but around 14 cyles in configureable bus (us-Delay is 4 cyles per uS)
 *
 * \param b bit to output
 * \param with_parasite_enable flag to indicate whether parasitic mode to be used
 */
INLINE uint8_t
ow_bit_io_intern(uint8_t b, uint8_t with_parasite_enable)
{
	ATOMIC(
#if OW_USE_INTERNAL_PULLUP
			OW_OUT_LOW();
#endif
			// drive bus low
			OW_DIR_OUT();
			// T_INT > 1usec accoding to timing-diagramm
			timer_udelay(2);
			if (b)
			{
				// to write "1" release bus, resistor pulls high
				OW_DIR_IN();
#if OW_USE_INTERNAL_PULLUP
				OW_OUT_HIGH();
#endif
			}

			// "Output data from the DS18B20 is valid for 15usec after the falling
			// edge that initiated the read time slot. Therefore, the master must
			// release the bus and then sample the bus state within 15ussec from
			// the start of the slot."
			timer_udelay(15 - 2 - OW_CONF_DELAYOFFSET); if (OW_GET_IN() == 0)
			{
				// sample at end of read-timeslot
				b = 0;
			}

			timer_udelay(60 - 15 - 2 + OW_CONF_DELAYOFFSET);
#if OW_USE_INTERNAL_PULLUP
			OW_OUT_HIGH();
#endif
			OW_DIR_IN(); 
			if (with_parasite_enable)
			{
				ow_parasite_enable();
			}
	); /* ATOMIC */

	// may be increased for longer wires
	timer_udelay(OW_RECOVERY_TIME);

	return b;
}



#endif
