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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief PHY chip interface.
 *
 * $WIZ$
 */

#ifndef DRV_PHY_H
#define DRV_PHY_H

#include <cfg/cfg_eth.h>

/*
 * Common PHY settings
 */

/**
 * Basic mode control settings.
 *
 *\{
 */
#define NIC_PHY_BMCR            0x00    ///<  Basic mode control register address.

#define NIC_PHY_BMCR_COLTEST    BV(7)   ///<  Collision test.
#define NIC_PHY_BMCR_FDUPLEX    BV(8)   ///<  Full duplex mode.
#define NIC_PHY_BMCR_ANEGSTART  BV(9)   ///<  Restart auto negotiation.
#define NIC_PHY_BMCR_ISOLATE    BV(10)  ///<  Isolate from MII.
#define NIC_PHY_BMCR_PWRDN      BV(11)  ///<  Power-down.
#define NIC_PHY_BMCR_ANEGENA    BV(12)  ///<  Enable auto negotiation.
#define NIC_PHY_BMCR_100MBPS    BV(13)  ///<  Select 100 Mbps.
#define NIC_PHY_BMCR_LOOPBACK   BV(14)  ///<  Enable loopback mode.
#define NIC_PHY_BMCR_RESET      BV(15)  ///<  Software reset.
/* \} */

/**
 * Basic mode status settings.
 *
 *\{
 */
#define NIC_PHY_BMSR             0x01    ///<  Basic mode status register.

#define NIC_PHY_BMSR_ANCOMPL     BV(5)  ///< Auto negotiation complete.
#define NIC_PHY_BMSR_ANEGCAPABLE BV(3)  ///< Able to do auto-negotiation
#define NIC_PHY_BMSR_LINKSTAT    BV(2)  ///<  Link status.
/* \} */

#define NIC_PHY_ID1              0x02    ///<  PHY identifier register 1.
#define NIC_PHY_ID2              0x03    ///<  PHY identifier register 2.
#define NIC_PHY_ANAR             0x04    ///<  Auto negotiation advertisement register.
#define NIC_PHY_ANLPAR           0x05    ///<  Auto negotiation link partner availability register.

/**
 * Auto negotiation expansion settings.
 *
 *\{
 */
#define NIC_PHY_ANER            0x06    //  Auto negotiation expansion register address.

#define NIC_PHY_ANLPAR_10_HDX   BV(5)   ///<  10BASE-T half duplex
#define NIC_PHY_ANLPAR_10_FDX   BV(6)   ///<  10BASE-T full duplex
#define NIC_PHY_ANLPAR_TX_HDX   BV(7)   ///<  100BASE-TX half duplex
#define NIC_PHY_ANLPAR_TX_FDX   BV(8)   ///<  100BASE-TX full duplex
/* \} */


/**
 * \name Supported PHY chip.
 *
 * List of supported PHY chip.
 *
 * $WIZ$ phy_chip_list = "DAVICOM_DM9161A", "NATIONAL_DP3848I"
 * \{
 */
#define DAVICOM_DM9161A          0
#define NATIONAL_DP3848I         1
/** \} */

#if CONFIG_PHY_CHIP == DAVICOM_DM9161A
	#include <drv/dm9161a.h>
#elif CONFIG_PHY_CHIP == NATIONAL_DP3848I
	#include <drv/dp83848i.h>
#else
	#error No supported PHY chip was select.
#endif

#endif /* DRV_PHY_H */
