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
 */

#include "http.h"

#include "hw/hw_sd.h"
#include "hw/hw_http.h"

#include "cfg/cfg_http.h"

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         HTTP_LOG_LEVEL
#define LOG_VERBOSITY     HTTP_LOG_FORMAT
#include <cfg/log.h>

#include <drv/sd.h>

#include <fs/fat.h>

#include <stdio.h>
#include <string.h>


static const char http_html_hdr_200[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
static const char http_html_hdr_404[] = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n\r\n";

void http_sendOk(struct netconn *client)
{
	netconn_write(client, http_html_hdr_200, sizeof(http_html_hdr_200) - 1, NETCONN_NOCOPY);
}

void http_sendFileNotFound(struct netconn *client)
{
	netconn_write(client, http_html_hdr_404, sizeof(http_html_hdr_404) - 1, NETCONN_NOCOPY);
}


static void get_fileName(const char *revc_buf, size_t recv_len, char *name, size_t len)
{
	int i = 0;
	char *p = strstr(revc_buf, "GET");
	if (p)
	{
		//Find the end of the page request.
		char *stop = strstr(revc_buf, "HTTP");
		if (!stop)
		{
			LOG_ERR("Bad GET request\n");
			name[0] = '\0';
			return;
		}

		//skip the "/" in get string request
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

	//Trail white space in the string.
	while ( --i >= 0 )
		if (name[i] != ' ' && name[i] != '\t' && name[i] != '\n')
			break;

	name[i + 1] = '\0';
}

static http_gci_handler_t cgi_search(const char *name,  HttpCGI *table)
{
	for (int i = 0; table[i].name; i++)
	{
		if (!strcmp(table[i].name, name))
			return table[i].handler;
	}
	return NULL;
}

static uint8_t tx_buf[2048];
static char file_name[80];

void http_server(struct netconn *server, struct HttpCGI *table)
{
	// SD fat filesystem context
	Sd sd;
	FATFS fs;
	FatFile in_file;
	struct netconn *client;
	struct netbuf *rx_buf_conn;
	char *rx_buf;
	u16_t len;
	FRESULT result;

	client = netconn_accept(server);
	if (!client)
		return;

	rx_buf_conn = netconn_recv(client);
	if (rx_buf_conn)
	{
		netbuf_data(rx_buf_conn, (void **)&rx_buf, &len);

		if (rx_buf)
		{
			memset(file_name, 0, sizeof(file_name));
			get_fileName(rx_buf, len, file_name, sizeof(file_name));

			LOG_INFO("Search %s\n", file_name);
			if (file_name[0] == '\0')
				strcpy(file_name, HTTP_DEFAULT_PAGE);

			http_gci_handler_t cgi = cgi_search(file_name,  table);
			if (cgi)
			{
				cgi(client, rx_buf, len);
			}
			else if (SD_CARD_PRESENT())
			{
				bool sd_ok = sd_init(&sd, NULL, 0);
				if (sd_ok)
				{
					LOG_INFO("Mount FAT filesystem.\n");
					result = f_mount(0, &fs);
					if (result != FR_OK)
					{
						LOG_ERR("Mounting FAT volumes error[%d]\n", result);
						sd_ok = false;
						f_mount(0, NULL);
					}

					if (sd_ok)
					{
						result = fatfile_open(&in_file, file_name,  FA_OPEN_EXISTING | FA_READ);

						size_t count = 0;
						if (result == FR_OK)
						{
							LOG_INFO("Opened file '%s' size %ld\n", file_name, in_file.fat_file.fsize);

							http_sendOk(client);

							while (count < in_file.fat_file.fsize)
							{
								int len = kfile_read(&in_file.fd, tx_buf, sizeof(tx_buf));
								netconn_write(client, tx_buf, len, NETCONN_COPY);
								count += len;
							}

							kfile_flush(&in_file.fd);
							kfile_close(&in_file.fd);

							LOG_INFO("Sent: %d\n", count);
						}
						else
						{
							LOG_ERR("Unable to open file: '%s' error[%d]\n",  file_name, result);
							http_sendFileNotFound(client);
							netconn_write(client, http_file_not_found, http_file_not_found_len - 1, NETCONN_NOCOPY);
						}
					}
				}
				f_mount(0, NULL);
				LOG_INFO("Umount FAT filesystem.\n");
			}
			else
			{
				http_sendFileNotFound(client);
				netconn_write(client, http_sd_not_present, http_sd_not_present_len, NETCONN_NOCOPY);
			}
		}
		netconn_close(client);
		netbuf_delete(rx_buf_conn);
	}
	netconn_delete(client);
}
