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

#include <lwip/tcpip.h>

typedef int (*http_handler_t)(struct netconn *client, const char *name, char *revc_buf, size_t revc_len);

typedef struct HttpCGI
{
	unsigned type;          ///< Strategy to find string in the cgi table.
	const char *name;       ///< Request string from GET request
	http_handler_t handler; ///< Callback to process the special request
} HttpCGI;


#define CGI_MATCH_NONE   0
#define CGI_MATCH_WORD   1  ///< Select item in table only if string match
#define CGI_MATCH_EXT    2  ///< Select item in table if the extention match
#define CGI_MATCH_NAME   3  ///< Select item in table if the string is content

void http_getPageName(const char *revc_buf, size_t recv_len, char *page_name, size_t len);

void http_sendOk(struct netconn *client);
void http_sendFileNotFound(struct netconn *client);
void http_sendInternalErr(struct netconn *client);

void http_poll(struct netconn *server);
void http_init(http_handler_t default_callback, struct HttpCGI *table);

int http_testSetup(void);
int http_testRun(void);
int http_testTearDown(void);

#endif /* NET_HTTP_H */
