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
 * Copyright 2004, 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Thermo-control driver
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 *
 * This module implements multiple thermo controls, which is the logic needed to try
 * keeping the temperature of a device constant. For this module, a "device" is a black box
 * whose temperature can be measured, and which has a mean to make it hotter or colder.
 * For instance, a device could be the combination of a NTC (analog temperature reader) and
 * a Peltier connected to the same physic block.
 *
 * This module relies on a low-level driver to communicate with the device (implementation
 * of the black box). This low-level driver also controls the units in which the temperature
 * is expressed: thermo control treats it just as a number.
 *
 */

/*#*
 *#* $Log$
 *#* Revision 1.3  2006/09/20 20:12:41  marco
 *#* Names convention, MOD_* macros.
 *#*
 *#* Revision 1.2  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/11/04 17:59:47  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.2  2005/06/14 10:13:36  batt
 *#* Better thermo errors handling.
 *#*
 *#* Revision 1.1  2005/05/24 09:17:58  batt
 *#* Move drivers to top-level.
 *#*
 *#* Revision 1.4  2005/05/10 16:55:10  batt
 *#* Add timeout to thermo-regulator; better thermo control handling; change thermo_getStatus() to thermo_status().
 *#*
 *#* Revision 1.3  2005/05/10 09:26:54  batt
 *#* Add thermo_getStatus for getting status/errors of thermo control.
 *#*
 *#* Revision 1.2  2005/05/09 19:18:40  batt
 *#* Remove old logs.
 *#*
 *#* Revision 1.1  2005/05/09 16:40:44  batt
 *#* Add thermo-control driver
 *#*/


#ifndef DRV_THERMO_H
#define DRV_THERMO_H

#include <drv/ntc.h>
#include <thermo_map.h>

void thermo_init(void);


/**
 * Set the target temperature at which a given device should be kept.
 *
 * \param dev Device
 * \param temperature Target temperature
 */
void thermo_setTarget(ThermoDev dev, deg_t temperature);

/** Start thermo control for a certain device \a dev */
void thermo_start(ThermoDev dev);

/** Stop thermo control for a certain device \a dev */
void thermo_stop(ThermoDev dev);

/** Clear errors for channel \a dev */
void thermo_clearErrors(ThermoDev dev);

/** Return the status of the specific \a dev thermo-device. */
thermostatus_t thermo_status(ThermoDev dev);

/**
 * Return the current temperature of a device currently under thermo
 * control.
 *
 * \param dev Device
 * \return Current temperature (Celsius degrees * 10)
 */
deg_t thermo_readTemperature(ThermoDev dev);


#endif /* DRV_THERMO_H */
