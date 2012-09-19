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
 * \brief Configuration file for 1 wire bus
 *
 * \author Peter Dannegger (danni(at)specs.de)
 * \author Martin Thomas (mthomas(at)rhrk.uni-kl.de)
 * \author Robin Gilks (g8ecj(at)gilks.org)
 * \author Francesco Sacchi <asterix@develer.com>
 */

#ifndef CFG_1WIRE_H
#define CFG_1WIRE_H

/**
 * Module logging level.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_level"
 */
#define OW_LOG_LEVEL      LOG_LVL_WARN

/**
 * Module logging format.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_format"
 */
#define OW_LOG_FORMAT     LOG_FMT_TERSE

/**
 * One bus for 1-wire devices
 *
 * $WIZ$ type = "boolean"
 */
#define OW_ONE_BUS   1


/**
 * Use internal pullup
 *
 * $WIZ$ type = "boolean"
 */
// Use AVR's internal pull-up resistor instead of external 4,7k resistor.
// Based on information from Sascha Schade. Experimental but worked in tests
// with one DS18B20 and one DS18S20 on a rather short bus (60cm), where both 
// sensores have been parasite-powered.
#define OW_USE_INTERNAL_PULLUP     1    /* 0=external, 1=internal */

/**
 *  1-wire data direction port.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "onewire_ddr_list"
 */
#define OW_DDR  DDRB


/**
 *  1-wire data pin
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "onewire_pin_list"
 */
#define OW_PIN  PB0


/**
 *  1-wire input port
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "onewire_portin_list"
 */
#define OW_IN   PINB

/**
 *  1-wire output port
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "onewire_portout_list"
 */
#define OW_OUT  PORTB


/**
 * Recovery time (T_Rec) minimum 1usec - increase for long lines 
 * 5 usecs is a value give in some Maxim AppNotes
 * 30u secs seem to be reliable for longer lines
 *
 * $WIZ$ type = "int"; min = 1; max = 50
 */
#define OW_RECOVERY_TIME         10



#endif /* CFG_1WIRE_H */

