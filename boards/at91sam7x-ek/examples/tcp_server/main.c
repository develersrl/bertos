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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \author Andrea Righi <arighi@develer.com>
 *
 * \brief lwIP TCP/IP echo server listening on port 80.
 */

#include "hw/hw_led.h"

#include <cfg/debug.h>

#include <cpu/irq.h>
#include <cpu/power.h>

#include <drv/timer.h>
#include <drv/ser.h>

#include <kern/proc.h>
#include <kern/monitor.h>

#include <netif/ethernetif.h>

#include <lwip/ip.h>
#include <lwip/ip_addr.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>
#include <lwip/dhcp.h>

static Serial out;

/* Network interface global variables */
static struct ip_addr ipaddr, netmask, gw;
static struct netif netif;

static void init(void)
{
	/* Enable all the interrupts */
	IRQ_ENABLE;

	/* Initialize debugging module (allow kprintf(), etc.) */
	kdbg_init();
	/* Initialize system timer */
	timer_init();
	/* Initialize UART0 */
	ser_init(&out, SER_UART0);
	/* Configure UART0 to work at 115.200 bps */
	ser_setbaudrate(&out, 115200);
	/* Initialize LED driver */
	LED_INIT();

	/*
	 * Kernel initialization: processes (allow to create and dispatch
	 * processes using proc_new()).
	 */
	proc_init();

	/* Initialize TCP/IP stack */
	tcpip_init(NULL, NULL);

	/* Bring up the network interface */
	netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);
	netif_set_default(&netif);
	netif_set_up(&netif);
}

static int tot_req;

static NORETURN void monitor_process(void)
{
	int start = tot_req;

	while (1)
	{
		monitor_report();
		kprintf("tot_req=%d [%d reqs/s]\n", tot_req, tot_req - start);
		start = tot_req;
		timer_delay(1000);
	}
}

int main(void)
{
	struct netconn *server;

	/* Hardware initialization */
	init();

	proc_new(monitor_process, NULL, KERN_MINSTACKSIZE * 2, NULL);

	dhcp_start(&netif);
	/*
	 * Here we wait for an ip address, but it's not strictly
	 * necessary. The address is obtained in background and
	 * as long as we don't use network functions, we could go
	 * on with initialization
	 */
	while (!netif.ip_addr.addr)
		timer_delay(200);
	kprintf(">>> dhcp ok: ip = ip = %s (kernel %s)\n",
		ip_ntoa(&netif.ip_addr.addr),
		CONFIG_KERN_PREEMPT ? "preempt" : "coop");

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

		tot_req++;
		rx_buf_conn = netconn_recv(client);
		if (rx_buf_conn)
		{
			netbuf_data(rx_buf_conn, (void **)&rx_buf, &len);
			if (rx_buf)
				netconn_write(client, rx_buf, len, NETCONN_COPY);
			netbuf_delete(rx_buf_conn);
		}
		while (netconn_delete(client) != ERR_OK)
			cpu_relax();
	}
}
