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
 * Copyright 2010,2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Andrea Righi <arighi@develer.com>
 *
 * \brief lwIP TCP/IP echo server listening on port 80.
 */

#include "bertos.c"

#include "hw/hw_sd.h"
#include "hw/hw_adc.h"
#include "hw/hw_sdram.h"

#include <cfg/debug.h>

#include <cpu/irq.h>
#include <cpu/power.h>

#include <drv/timer.h>
#include <drv/ser.h>
#include <drv/sd.h>
#include <drv/dmac_sam3.h>
#include <drv/adc.h>

#include <kern/proc.h>
#include <kern/monitor.h>

#include <netif/ethernetif.h>

#include <lwip/ip.h>
#include <lwip/ip_addr.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>
#include <lwip/dhcp.h>

#include <fs/fat.h>

#include <stdio.h>
#include <string.h>

/* Network interface global variables */
static struct ip_addr ipaddr, netmask, gw;
static struct netif netif;

// SD fat filesystem context
static Sd sd;
static FATFS fs;
static FatFile in_file;

typedef struct BoardStatus
{
	char local_ip[sizeof("123.123.123.123")];
	char last_connected_ip[sizeof("123.123.123.123")];
	uint16_t internal_temp;
	ticks_t up_time;
	size_t tot_req;
} BoardStatus;

static BoardStatus status;

static void init(void)
{
	/* Enable all the interrupts */
	IRQ_ENABLE;

	/* Initialize debugging module (allow kprintf(), etc.) */
	kdbg_init();
	/* Initialize system timer */
	timer_init();

	/*
	 * Kernel initialization: processes (allow to create and dispatch
	 * processes using proc_new()).
	 */
	proc_init();
	sdram_init();

	/* Initialize TCP/IP stack */
	tcpip_init(NULL, NULL);

	/* Bring up the network interface */
	netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);
	netif_set_default(&netif);
	netif_set_up(&netif);

	dmac_init();

	adc_init();
	/* Enable the adc to read internal temperature sensor */
	hw_enableTempRead();
}

static NORETURN void status_process(void)
{
	while (1)
	{
		status.internal_temp = hw_convertToDegree(adc_read(ADC_TEMPERATURE_CH));
		status.up_time++;
		timer_delay(1000);
	}
}

static void get_fileName(char *revc_buf, char *name, size_t len)
{
	char *p = strstr(revc_buf, "GET");
	if (p)
	{
		//skip the "/" in get string request
		p += sizeof("GET") + 1;
		for (size_t i = 0; *p != ' '; i++,p++)
		{
			if (i > len)
				break;
			name[i] = *p;
		}
	}
}

static const char http_html_hdr_200[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
static const char http_html_hdr_404[] = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n\r\n";

static const char http_file_not_found[] = "\
<!DOCTYPE html PUBLIC \"-//IETF//DTD HTML 2.0//EN\"> \
<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\"> \
<title>404 Not Found</title></head><body><img src=\"bertos_jpg.jpg\"><h1>404 Not Found</h1>\
<p>The requested URL was not found on this server.</p><hr>\
<address>BeRTOS simple HTTP server</address></body></html>";


static const char http_sd_not_present[] = " \
<!DOCTYPE html PUBLIC \"-//IETF//DTD HTML 2.0//EN\">  \
<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\">  \
<title>BeRTOS simple HTTP Server</title></head><body><img src=\"bertos_jpg.jpg\"> \
<h1>BeRTOS simple HTTP Server</h1><p>Simple Http server, the site's pages are stored on SD card, check it if is present.</p><hr>\
<a href=\"http://www.bertos.org\">www.BeRTOS.org</a></body></html> \
";

static uint8_t tx_buf[2048];

#define IP_ADDR_TO_INT_TUPLE(addr) \
		(int)((addr) >>  0 & 0xff), \
		(int)((addr) >>  8 & 0xff), \
		(int)((addr) >> 16 & 0xff), \
		(int)((addr) >> 24 & 0xff)


static char file_name[80];
int main(void)
{
	struct netconn *server;
	FRESULT result;

	/* Hardware initialization */
	init();

	proc_new(status_process, NULL, KERN_MINSTACKSIZE * 2, NULL);

	dhcp_start(&netif);
	while (!netif.ip_addr.addr)
		timer_delay(DHCP_FINE_TIMER_MSECS);
	kprintf("dhcp ok: ip = %d.%d.%d.%d\n", IP_ADDR_TO_INT_TUPLE(netif.ip_addr.addr));

	server = netconn_new(NETCONN_TCP);
	netconn_bind(server, IP_ADDR_ANY, 80);
	netconn_listen(server);

	while (1)
	{
		struct netconn *client;
		struct netbuf *rx_buf_conn;
		char *rx_buf;
		u16_t len;

		client = netconn_accept(server);
		if (!client)
			continue;

		//Update board status.
		sprintf(status.local_ip, "%d.%d.%d.%d", IP_ADDR_TO_INT_TUPLE(client->pcb.ip->remote_ip.addr));
		sprintf(status.last_connected_ip, "%d.%d.%d.%d", IP_ADDR_TO_INT_TUPLE(client->pcb.ip->local_ip.addr));
		status.tot_req++;

		rx_buf_conn = netconn_recv(client);
		if (rx_buf_conn)
		{
			netbuf_data(rx_buf_conn, (void **)&rx_buf, &len);
			if (rx_buf)
			{
				memset(file_name, 0, sizeof(file_name));
				memset(tx_buf, 0, sizeof(tx_buf));

				get_fileName(rx_buf, file_name, sizeof(file_name));

				kprintf("%s\n", file_name);
				if (strlen(file_name) == 0)
					strcpy(file_name, "index.htm");

				if (!strcmp("bertos_jpg.jpg", file_name))
				{
					netconn_write(client, bertos_jpg, sizeof(bertos_jpg), NETCONN_NOCOPY);
				}
				else if (!strcmp("status", file_name))
				{
					sprintf((char *)tx_buf, "[ %s, %s, %d.%d, %ld, %d ]", status.local_ip, status.last_connected_ip,
																status.internal_temp / 10, status.internal_temp % 10,
																status.up_time, status.tot_req);

					netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);
				}
				else if (SD_CARD_PRESENT())
				{
					bool sd_ok = sd_init(&sd, NULL, 0);
					if (sd_ok)
					{
						kprintf("Mount FAT filesystem.\n");
						result = f_mount(0, &fs);
						if (result != FR_OK)
						{
							kprintf("Mounting FAT volumes error[%d]\n", result);
							sd_ok = false;
							f_mount(0, NULL);
						}

						if (sd_ok)
						{
							result = fatfile_open(&in_file, file_name,  FA_OPEN_EXISTING | FA_READ);

							size_t count = 0;
							if (result == FR_OK)
							{
								kprintf("Opened file '%s' size %ld\n", file_name, in_file.fat_file.fsize);

								netconn_write(client, http_html_hdr_200, sizeof(http_html_hdr_200) - 1, NETCONN_NOCOPY);

								while (count < in_file.fat_file.fsize)
								{
									int len = kfile_read(&in_file.fd, tx_buf, sizeof(tx_buf));
									netconn_write(client, tx_buf, len, NETCONN_COPY);
									count += len;
								}

								kfile_flush(&in_file.fd);
								kfile_close(&in_file.fd);

								kprintf("Sent: %d\n", count);
							}
							else
							{
								kprintf("Unable to open file: '%s' error[%d]\n",  file_name, result);
								netconn_write(client, http_html_hdr_404, sizeof(http_html_hdr_404) - 1, NETCONN_NOCOPY);
								netconn_write(client, http_file_not_found, sizeof(http_file_not_found) - 1, NETCONN_NOCOPY);
							}
						}
					}
					f_mount(0, NULL);
					kprintf("Umount FAT filesystem.\n");
				}
				else
				{
					netconn_write(client, http_html_hdr_404, sizeof(http_html_hdr_404) - 1, NETCONN_NOCOPY);
					netconn_write(client, http_sd_not_present, sizeof(http_sd_not_present), NETCONN_NOCOPY);
				}
			}
			netconn_close(client);
			netbuf_delete(rx_buf_conn);
		}
		netconn_delete(client);
	}
}
