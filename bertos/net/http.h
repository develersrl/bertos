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
 * \brief Simple HTTP server
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "http"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_http.h"
 * $WIZ$ module_depends = "lwip", "kfile", "sd"
 * $WIZ$ module_hw = "bertos/hw/hw_http.h", "bertos/hw/hw_http.c"
 */

#ifndef NET_HTTP_H
#define NET_HTTP_H


#include <netif/ethernetif.h>

#include <lwip/ip.h>
#include <lwip/ip_addr.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>
#include <lwip/dhcp.h>

typedef int (*http_gci_handler_t)(char *revc_buf, struct netconn *client);

typedef struct HttpCGI
{
	const char *name;
	http_gci_handler_t handler;
} HttpCGI;

void http_sendOk(struct netconn *client);
void http_sendFileNotFound(struct netconn *client);
void http_server(struct netconn *server, struct HttpCGI *gci);

#endif /* NET_HTTP_H */
