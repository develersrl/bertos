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
 * Copyright 2012 Jack Bradach <jack@bradach.net>
 *
 * -->
 *
 * \brief Maxim MAX31855 Cold-Junction Compensated Thermocouple-to-Digital Converter
 *
 * \author Jack Bradach <jack@bradach.net>
 *
 * $WIZ$ module_name = "max31855"
 * $WIZ$ module_depends = "kfile"
 * $WIZ$ module_configuration = ""
 * $WIZ$ module_hw = "bertos/hw/hw_max31855.h"
 */

#ifndef DRV_MAX31855_H
#define DRV_MAX31855_H

#include <cpu/types.h>
#include <io/kfile.h>

/** Status return codes for the MAX31855 are implemented such
 * that any non-zero return code indicates a fault has been
 * detected.  Since multiple faults can be signaled simultaneously,
 * the bits set in the returned value must be compared with the bit
 * vectors of the faults.  If you are reading in "external only"
 * temperature mode, a non-zero value simply indicates that a
 * fault has occured, but you don't have any information about
 * what specific fault happened.
 */
#define MAX31855_OK 0

/** Error bit vectors for the MAX31855.  The first three values
 * pull double-duty as the bit vectors used to indicate which
 * faults are occuring when both internal and external temperatures
 * are read.
 */
enum max31855_fault {
    /** Indicates an open circuit on the thermocouple */
    MAX31855_FAULT_OC = 0,
    
    /** Indicates that the thermocouple is shorted to ground. */
    MAX31855_FAULT_SCG,
    
    /** Indicates that the thermocouple is shorted to Vcc. */
    MAX31855_FAULT_SCV,
    
    /** Indicates that a fault has occured. */
    MAX31855_FAULT_BIT = 16,
    
    /** If we only read the external value, bit 0 is the fault bit.
     * This is physically the same bit as MAX31855_FAULT_BIT.
     */
    MAX31855_FAULT_BIT_EXT = 0
};

#define MAX31855_FAULT_MASK (BV(MAX31855_FAULT_OC) | BV(MAX31855_FAULT_SCG) | BV(MAX31855_FAULT_SCV))

#define MAX31855_SIZE_EXT   0x2
#define MAX31855_SIZE_BOTH  0x4

/* External temperature data is the upper
 * word, when both temperatures are read.
 */
#define MAX31855_EXT_BIT_OFFSET 16

/* The internal and external temp data
 * is left-shifted a couple places
 */
#define MAX31855_EXT_DATA_SHIFT 2
#define MAX31855_INT_DATA_SHIFT 4

#define MAX31855_INT_MASK       0xFFFF

#define MAX31855_SIGN_BIT   15

int max31855_rd_cel(KFile *fd, float *tmp_ext, float *tmp_int);
int max31855_rd_fah(KFile *fd, float *tmp_ext, float *tmp_int);
int max31855_rd_kel(KFile *fd, float *tmp_ext, float *tmp_int);

void max31855_init(void);

#endif /* DRV_MAX31855_H */
