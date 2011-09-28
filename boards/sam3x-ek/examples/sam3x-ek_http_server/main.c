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
 * \author Andrea Righi <arighi@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * \brief Simple Http server.
 *
 * This simple web server read the site's pages from SD card, and manage
 * the cases where SD is not present or page not found, using embedded pages.
 * Quering from browser the /status page, the server return a json dictionary where are store
 * some board status info, like board temperature, up-time, etc.
 */


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

#include <net/http.h>

#include <netif/ethernetif.h>

#include <lwip/ip.h>
#include <lwip/ip_addr.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>
#include <lwip/dhcp.h>

#include <fs/fat.h>

#include <icons/bertos.h>

#include <stdio.h>
#include <string.h>

/* Network interface global variables */
static struct ip_addr ipaddr, netmask, gw;
static struct netif netif;

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

#define IP_ADDR_TO_INT_TUPLE(addr) \
		(int)((addr) >>  0 & 0xff), \
		(int)((addr) >>  8 & 0xff), \
		(int)((addr) >> 16 & 0xff), \
		(int)((addr) >> 24 & 0xff)


static uint8_t tx_buf[1024];

static int cgi_status(char *revc_buf, struct netconn *client)
{
	(void)revc_buf;

	//Update board status.
	sprintf(status.last_connected_ip, "%d.%d.%d.%d", IP_ADDR_TO_INT_TUPLE(client->pcb.ip->remote_ip.addr));
	sprintf(status.local_ip, "%d.%d.%d.%d", IP_ADDR_TO_INT_TUPLE(client->pcb.ip->local_ip.addr));
	sprintf((char *)tx_buf, "[ %s, %s, %d.%d, %ld, %d ]", status.local_ip, status.last_connected_ip,
															status.internal_temp / 10, status.internal_temp % 10,
															status.up_time, status.tot_req);

	status.tot_req++;

	http_sendOk(client);
	netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);
	return 0;
}

static int cgi_logo(char *revc_buf, struct netconn *client)
{
	(void)revc_buf;

	http_sendOk(client);
	netconn_write(client, bertos_logo_jpg, bertos_logo_jpg_len, NETCONN_NOCOPY);
	return 0;
}


static HttpCGI cgi_table[] =
{
	{ "status",              cgi_status },
	{ "bertos_logo_jpg.jpg", cgi_logo   },
	{ NULL, NULL }
};


int main(void)
{
	struct netconn *server;


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
		http_server(server, cgi_table);
	}
}
