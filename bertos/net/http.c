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
 * \brief Simple Http server.
 *
 * This simple web server read the site's pages from SD card, and manage
 * the cases where SD is not present or page not found, using embedded pages.
 * Quering from browser the /status page, the server return a json dictionary where are store
 * some board status info, like board temperature, up-time, etc.
 *
 * notest: avr
 */

#include "http.h"

#include "hw/hw_sd.h"
#include "hw/hw_http.h"

#include "cfg/cfg_http.h"

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         HTTP_LOG_LEVEL
#define LOG_VERBOSITY     HTTP_LOG_FORMAT
#include <cfg/log.h>

#include <stdio.h>
#include <string.h>


static const char http_html_hdr_200[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
static const char http_html_hdr_404[] = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n\r\n";
static const char http_html_hdr_500[] = "HTTP/1.1 500 Internal Server Error\r\nContent-type: text/html\r\n\r\n";

static HttpCGI *cgi_table;
static http_handler_t http_callback;

/**
 * Send on \param client socket
 * the 200 Ok http header
 */
void http_sendOk(struct netconn *client)
{
	netconn_write(client, http_html_hdr_200, sizeof(http_html_hdr_200) - 1, NETCONN_NOCOPY);
}


/**
 * Send on \param client socket
 * the 404 File not found http header
 */
void http_sendFileNotFound(struct netconn *client)
{
	netconn_write(client, http_html_hdr_404, sizeof(http_html_hdr_404) - 1, NETCONN_NOCOPY);
}

/**
 * Send on \param client socket
 * the 500 internal server error http header
 */
void http_sendInternalErr(struct netconn *client)
{
	netconn_write(client, http_html_hdr_500, sizeof(http_html_hdr_500) - 1, NETCONN_NOCOPY);
}

static void get_fileName(const char *revc_buf, size_t recv_len, char *name, size_t len)
{
	int i = 0;
	char *p = strstr(revc_buf, "GET");
	if (p)
	{
		/* Find the end of the page request. */
		char *stop = strstr(revc_buf, "HTTP");
		if (!stop)
		{
			LOG_ERR("Bad GET request\n");
			name[0] = '\0';
			return;
		}

		/* skip the "/" in get string request */
		p += sizeof("GET") + 1;

		while (p != stop)
		{
			if ((size_t)i == len || (size_t)i >= recv_len)
			{
				name[i] = '\0';
				break;
			}

			name[i++] = *(p++);
		}
	}

	/* Trail white space in the string. */
	while ( --i >= 0 )
		if (name[i] != ' ' && name[i] != '\t' && name[i] != '\n')
			break;

	name[i + 1] = '\0';
}

INLINE const char *get_ext(const char *name)
{
	const char *ext = strstr(name, ".");
	if(ext && (ext + 1))
		return (ext + 1);

	return NULL;
}

static http_handler_t cgi_search(const char *name,  HttpCGI *table)
{
	if (!table)
		return NULL;

	int i = 0;
	const char *ext = get_ext(name);
	LOG_INFO("EXT %s\n", ext);
	while(table[i].name)
	{
		if (ext && table[i].type == CGI_MATCH_EXT)
		{
			LOG_INFO("Match all ext %s\n", ext);
			if (!strcmp(table[i].name, ext))
				break;
		}
		else /* (table[i].type == CGI_MATCH_NAME) */
		{
			LOG_INFO("Match all name %s\n", name);
			if (!strcmp(table[i].name, name))
				break;
		}

		i++;
	}

	return table[i].handler;
}

static char req_string[80];

/**
 * Http polling function.
 *
 * Call this functions to process each client connections.
 *
 */
void http_poll(struct netconn *server)
{
	struct netconn *client;
	struct netbuf *rx_buf_conn;
	char *rx_buf;
	uint16_t len;

	client = netconn_accept(server);
	if (!client)
		return;

	rx_buf_conn = netconn_recv(client);
	if (rx_buf_conn)
	{
		netbuf_data(rx_buf_conn, (void **)&rx_buf, &len);
		if (rx_buf)
		{
			memset(req_string, 0, sizeof(req_string));
			get_fileName(rx_buf, len, req_string, sizeof(req_string));

			LOG_INFO("Search %s\n", req_string);
			if (req_string[0] == '\0')
				strcpy(req_string, HTTP_DEFAULT_PAGE);

			http_handler_t cgi = cgi_search(req_string, cgi_table);
			if (cgi)
			{
				if (cgi(client, req_string, rx_buf, len) < 0)
				{
					LOG_ERR("Internal server error\n");
					http_sendInternalErr(client);
					netconn_write(client, http_server_error, http_server_error_len - 1, NETCONN_NOCOPY);
				}
			}
			else
			{
				http_callback(client, req_string, rx_buf, len);
			}
		}
		netconn_close(client);
		netbuf_delete(rx_buf_conn);
	}
	netconn_delete(client);
}

/**
 * Init the http server.
 *
 * The simply http server call for each client request the default_callback function. The
 * user should define this callback to manage the client request, i.e. reading site's page
 * from SD card. The user can define the cgi_table, where associate one callback to the user string.
 * In this way the user could filter some client request and redirect they to custom callback, i.e.
 * the client could request status of the device only loading the particular page name.
 *
 * \param default_callback fuction that server call for all request, that does'nt match cgi table.
 * \param table of callcack to call when client request a particular page.
 */
void http_init(http_handler_t default_callback, struct HttpCGI *table)
{
	ASSERT(default_callback);

	cgi_table = table;
	http_callback = default_callback;
}

