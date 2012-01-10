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
  * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
  * All Rights Reserved.
  * -->
  *
  * \brief Definition for Davicom DM9161A ethernet phy.
  *
  * \author Francesco Sacchi <batt@develer.com>
  */

#ifndef DM9161A_H
#define DM9161A_H

#define NIC_PHY_ADDR 31
#define NIC_PHY_ID   0x0181b8a0

#define PHY_INIT() \
	do { \
		phy_pinGpio(); \
		/* Disable RMII and TESTMODE */ \
		phy_pinSet(PHY_MII_PINS & ~(PHY_COL_BIT | PHY_RXDV_BIT)); \
		phy_resetPulse(); \
	} while (0)
#endif /* DM9161A_H */
