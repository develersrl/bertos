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
 * Copyright 2012 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \defgroup syslog_module SYSLOG module.
 * \ingroup net
 * \{
 *
 * \brief SYSLOG System Log modulelog.
 *
 * This module allow the user to send all debug message to one syslog server.
 * The only things that we need to use it, is to have a valid ethernet interface and the
 * ip address of the remote syslog server, then the syslog module redirect all LOG_* (INFO, WARN, ERR)
 * message to syslog server, optionally we can send both message on serial and on syslog.
 *
 * The usage pattern is as follows:
 * \code
 * //Init the network, es using dhcp:
 *
 * //Initialize TCP/IP stack
 * tcpip_init(NULL, NULL);
 *
 * //Bring up the network interface
 * netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);
 * netif_set_default(&netif);
 * netif_set_up(&netif);
 *
 * dhcp_start(&netif);
 * while (!netif.ip_addr.addr)
 *   timer_delay(DHCP_FINE_TIMER_MSECS);
 *
 * //lwip address struct
 * struct ip_addr server_addr;
 * // convert address to ip_address
 * IP4_ADDR(&server_addr, 192, 168, 0, 2);
 *
 * //init the syslog module
 * syslog_init(&syslog, server_addr);
 *
 * //now all LOG_*(message) are logged on
 * //syslog server.
 * //see the cfg_syslog.h for all settings.
 * \endcode
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "syslog"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_syslog.h"
 * $WIZ$ module_depends = "lwip", "debug"
 */

#ifndef NET_SYSLOG_H
#define NET_SYSLOG_H

#include <lwip/netif.h>
#include <lwip/ip_addr.h>

typedef struct SysLog
{
	struct netconn *syslog_server;
	struct netbuf *send_buf;
	struct ip_addr server_addr;

	uint32_t syslog_cnt;
} SysLog;


uint32_t syslog_count(void);
struct ip_addr syslog_ip(void);
void syslog_setIp(struct ip_addr addr);

int syslog_printf(const char *fmt, ...);

void syslog_init(SysLog *syslog_ctx, struct ip_addr addr);

/** \} */ //defgroup syslog_module.

#endif /* NET_SYSLOG_H */
