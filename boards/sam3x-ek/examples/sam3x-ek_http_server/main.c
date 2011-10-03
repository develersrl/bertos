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
 * Quering from browser some cgi page, the server return a json dictionary.
 */


#include "hw/hw_http.h"
#include "hw/hw_sd.h"
#include "hw/hw_adc.h"
#include "hw/hw_sdram.h"

// Define logging setting (for cfg/log.h module).
#define LOG_LEVEL         3
#define LOG_VERBOSITY     0
#include <cfg/log.h>
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
	uint32_t up_time;
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

/* Macro to unpack the ip addres from lwip format */
#define IP_ADDR_TO_INT_TUPLE(addr) \
		(int)((addr) >>  0 & 0xff), \
		(int)((addr) >>  8 & 0xff), \
		(int)((addr) >> 16 & 0xff), \
		(int)((addr) >> 24 & 0xff)


static uint8_t tx_buf[2048];

/*
 * Return a JSON string of board status.
 */
static int cgi_status(struct netconn *client, const char *name, char *revc_buf, size_t revc_len)
{
	(void)revc_buf;
	(void)revc_len;
	(void)name;

	//Update board status.
	sprintf(status.last_connected_ip, "%d.%d.%d.%d", IP_ADDR_TO_INT_TUPLE(client->pcb.ip->remote_ip.addr));
	sprintf(status.local_ip, "%d.%d.%d.%d", IP_ADDR_TO_INT_TUPLE(client->pcb.ip->local_ip.addr));
	sprintf((char *)tx_buf, "[ \"%s\", \"%s\", %d.%d, %ld, %d ]", status.local_ip, status.last_connected_ip,
															status.internal_temp / 10, status.internal_temp % 10,
															status.up_time, status.tot_req);

	status.tot_req++;

	http_sendOk(client);
	netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);
	return 0;
}

static int cgi_logo(struct netconn *client, const char *name, char *revc_buf, size_t revc_len)
{
	(void)revc_buf;
	(void)revc_len;
	(void)name;

	http_sendOk(client);
	netconn_write(client, bertos_logo_jpg, bertos_logo_jpg_len, NETCONN_NOCOPY);
	return 0;
}

/*
 * Return the internal micro temperature string.
 */
static int cgi_temp(struct netconn *client, const char *name, char *revc_buf, size_t revc_len)
{
	(void)revc_buf;
	(void)revc_len;
	(void)name;

	sprintf((char *)tx_buf, "[%d.%d]", status.internal_temp / 10, status.internal_temp % 10);

	http_sendOk(client);
	netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);
	return 0;
}


/*
 * Return the board uptime.
 */
static int cgi_uptime(struct netconn *client, const char *name, char *revc_buf, size_t revc_len)
{
	(void)revc_buf;
	(void)revc_len;
	(void)name;



	uint32_t m = status.up_time / 60;
	uint32_t h = m / 60;
	uint32_t s = status.up_time  - (m * 60) - (h * 3600);

	sprintf((char *)tx_buf, "[\"%ldh %ldm %lds\"]", h, m, s);

	http_sendOk(client);
	netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);
	return 0;
}

/*
 * Return the VR1 potentiometer voltage.
 */
static int cgi_resistor(struct netconn *client, const char *name, char *revc_buf, size_t revc_len)
{
	(void)revc_buf;
	(void)revc_len;
	(void)name;

	uint16_t volt = ADC_RANGECONV(adc_read(1), 0, 3300);
	sprintf((char *)tx_buf, "[ \"%d.%dV\" ]",  volt / 1000, volt % 1000);

	http_sendOk(client);
	netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);
	return 0;
}

/*
 * Reply to client the request string.
 */
static int cgi_led(struct netconn *client, const char *name, char *revc_buf, size_t revc_len)
{
	(void)name;

	http_sendOk(client);
	netconn_write(client, revc_buf, revc_len, NETCONN_COPY);
	return 0;
}

/*
 * Reply to client the request string.
 */
static int cgi_echo(struct netconn *client, const char *name, char *revc_buf, size_t revc_len)
{
	(void)name;

	http_sendOk(client);
	netconn_write(client, revc_buf, revc_len, NETCONN_COPY);
	return 0;
}



/*
 * Default function that http server call every client request, if it doesn't match a cgi table.
 * In this implementation all client request are associate to real file stored on FAT file
 * sistem on SD card. If the file there is not on SD card the server reply to client the
 * error File not found, and send an harcoded page. In the same way, the server reply
 * error page if the SD card is not present.
 *
 */
static int http_htmPageLoad(struct netconn *client, const char *name, char *revc_buf, size_t revc_len)
{
	(void)revc_buf;
	(void)revc_len;

	if (SD_CARD_PRESENT())
	{

		// SD fat filesystem context
		Sd sd;
		FATFS fs;
		FatFile in_file;
		FRESULT result;

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
				result = fatfile_open(&in_file, name,  FA_OPEN_EXISTING | FA_READ);

				size_t count = 0;
				if (result == FR_OK)
				{
					LOG_INFO("Opened file '%s' size %ld\n", name, in_file.fat_file.fsize);

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
					LOG_ERR("Unable to open file: '%s' error[%d]\n",  name, result);
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

	return 0;
}

/*
 * Return to client a string that display the CHIP ID information.
 * See datasheet for more detail.
 */
static int cgi_chipInfo(struct netconn *client, const char *name, char *revc_buf, size_t revc_len)
{
	(void)revc_buf;
	(void)revc_len;
	(void)name;

	sprintf((char *)tx_buf, "{ \"core_name\":\"%s\", \"arch_name\":\"%s\", \"sram_size\":\"%s\",\
								\"flash_size\":\"%s\", \"mem_boot_type\":\"%s\" }",
	chipid_eproc_name(CHIPID_EPRCOC()),
	chipid_archnames(CHIPID_ARCH()),
	chipid_sramsize(CHIPID_SRAMSIZ()),
	chipid_nvpsize(CHIPID_NVPSIZ()),
	chipid_nvptype(CHIPID_NVTYP()));

	http_sendOk(client);
	netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);

	return 0;
}

/*
 * Static cgi table where we associate callback to page.
 */
static HttpCGI cgi_table[] =
{
	{ CGI_MATCH_WORD, "echo",                cgi_echo          },
	{ CGI_MATCH_NAME, "get_temperature",     cgi_temp          },
	{ CGI_MATCH_NAME, "get_uptime",          cgi_uptime        },
	{ CGI_MATCH_NAME, "get_resistor",        cgi_resistor      },
	{ CGI_MATCH_NAME, "set_led",             cgi_led           },
	{ CGI_MATCH_WORD, "status",              cgi_status        },
	{ CGI_MATCH_NAME, "get_chipinfo",        cgi_chipInfo      },
	{ CGI_MATCH_NAME, "bertos_logo_jpg",     cgi_logo          },
	{ CGI_MATCH_NONE,  NULL,                 NULL              }
};


int main(void)
{
	struct netconn *server;

	/* Hardware initialization */
	init();
	http_init(http_htmPageLoad, cgi_table);

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
		http_poll(server);
	}
}
