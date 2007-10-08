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
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \version $Id$
 *
 * \brief PWM driver (interface)
 *
 * \version $Id$
 * \author Francesco Sacchi <batt@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2006/07/19 12:56:26  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.1  2005/11/04 18:08:49  bernie
 *#* Import into DevLib.
 *#*
 *#* Revision 1.1  2005/05/24 09:17:58  batt
 *#* Move drivers to top-level.
 *#*
 *#* Revision 1.3  2005/05/09 16:36:12  batt
 *#* Change some function names to accomplish coding standard.
 *#*
 *#* Revision 1.2  2005/05/02 16:03:08  batt
 *#* Remove unusefull interrupt saving.
 *#*
 *#* Revision 1.1  2005/05/02 12:36:39  batt
 *#* Add pwm driver.
 *#*
 *#*/
#ifndef DRV_PWM_H
#define DRV_PWM_H

#include <pwm_map.h>
#include <cfg/compiler.h>

typedef uint16_t pwm_duty_t;

void pwm_setDuty(PwmDev dev, pwm_duty_t duty);
void pwm_init(void);

#define PWM_MAX_DUTY 100

#endif /* DRV_PWM_H */
