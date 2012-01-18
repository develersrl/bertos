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
#include <lwip/api.h>

typedef int (*http_handler_t)(struct netconn *client, const char *name, char *revc_buf, size_t revc_len);

typedef struct HttpCGI
{
	unsigned type;          ///< Strategy to find string in the cgi table.
	const char *name;       ///< Request string from GET request
	http_handler_t handler; ///< Callback to process the special request
} HttpCGI;

enum
{
	HTTP_CONTENT_JSON = 0,
	HTTP_CONTENT_HTML,
	HTTP_CONTENT_CSS,
	HTTP_CONTENT_JS,
	HTTP_CONTENT_PNG,
	HTTP_CONTENT_JPEG,
	HTTP_CONTENT_GIF,
	HTTP_CONTENT_PLAIN,

	HTTP_CONTENT_CNT
};

#define CGI_MATCH_NONE   0
#define CGI_MATCH_WORD   1  ///< Select item in table only if string match
#define CGI_MATCH_EXT    2  ///< Select item in table if the extention match
#define CGI_MATCH_NAME   3  ///< Select item in table if the string is content

int http_getValue(char *tolenized_buf, size_t tolenized_buf_len, const char *key, char *value, size_t len);
int http_tokenizeGetRequest(char *raw_buf, size_t raw_len);
void http_getPageName(const char *recv_buf, size_t recv_len, char *page_name, size_t len);
size_t http_decodeUrl(const char *raw_buf, size_t raw_len, char *decodec_buf, size_t len);
int http_searchContentType(const char *name);

void http_sendOk(struct netconn *client, int content_type);
void http_sendFileNotFound(struct netconn *client, int content_type);
void http_sendInternalErr(struct netconn *client, int content_type);

void http_poll(struct netconn *server);
void http_init(http_handler_t default_callback, struct HttpCGI *table);

int http_testSetup(void);
int http_testRun(void);
int http_testTearDown(void);

#endif /* NET_HTTP_H */
