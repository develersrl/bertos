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


#include "bitmaps.h"

#include "hw/hw_http.h"
#include "hw/hw_sd.h"
#include "hw/hw_adc.h"
#include "hw/hw_sdram.h"
#include "hw/hw_led.h"
#include "hw/hw_lcd.h"

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
#include <drv/lcd_hx8347.h>

#include <kern/proc.h>
#include <kern/monitor.h>
#include <struct/heap.h>

#include <net/http.h>

#include <netif/ethernetif.h>

#include <lwip/ip.h>
#include <lwip/ip_addr.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>
#include <lwip/dhcp.h>

#include <gfx/gfx.h>
#include <gfx/font.h>
#include <gfx/text.h>

#include <fs/fat.h>

#include <icons/bertos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Network interface global variables */
static struct ip_addr ipaddr, netmask, gw;
static struct netif netif;


#define GET_LED_STATUS(status, led_id)     (((status) & BV((led_id))) >> (led_id))
#define CLEAR_LED_STATUS(status, led_id)   ((status) &= ~BV((led_id)))
#define SET_LED_STATUS(status, led_id)     ((status) |= BV((led_id)))

typedef struct BoardStatus
{
	char local_ip[sizeof("123.123.123.123")];
	char last_connected_ip[sizeof("123.123.123.123")];
	uint8_t led_status;
	uint16_t internal_temp;
	uint32_t up_time;
	size_t tot_req;
} BoardStatus;

static BoardStatus status;
static uint8_t raster[RAST_SIZE(LCD_WIDTH, LCD_HEIGHT)];
static Bitmap *lcd_bitmap;
extern Font font_gohu;
static int lcd_brightness = LCD_BACKLIGHT_MAX;
static struct Heap heap;
static uint8_t tx_buf[2048];


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
	dmac_init();
	adc_init();

	/* Enable the adc to read internal temperature sensor */
	hw_enableTempRead();
	LED_INIT();

	heap_init(&heap, (void *)SDRAM_BASE, SDRAM_SIZE);
	lcd_bitmap = heap_allocmem(&heap, RAST_SIZE(LCD_WIDTH, LCD_HEIGHT));
	if (lcd_bitmap)
		kprintf("Allocated memory for display raster, addr 0x%x\n", (unsigned)lcd_bitmap);
	else
	{
		kprintf("Error allocating memory for LCD raster!\n");
	}

	lcd_hx8347_init();
	lcd_setBacklight(lcd_brightness);

	gfx_bitmapInit(lcd_bitmap, raster, LCD_WIDTH, LCD_HEIGHT);
	gfx_setFont(lcd_bitmap, &font_luBS14);
	lcd_hx8347_blitBitmap(lcd_bitmap);

	/* Initialize TCP/IP stack */
	tcpip_init(NULL, NULL);

	/* Bring up the network interface */
	netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);
	netif_set_default(&netif);
	netif_set_up(&netif);
}

static int sec_to_strDhms(uint32_t sec_time, char *str, size_t len)
{
    uint32_t h = (sec_time / 3600);
    uint32_t d = h / 24;
    uint32_t m = ((sec_time - (h * 3600)) / 60);
    uint32_t s = (sec_time - (m * 60) - (h * 3600));

    if (len < sizeof("xxxxd xxh xxm xxs"))
        return -1;

    sprintf(str, "%ldd %ldh %ldm %lds", d, (h >= 24) ? h - 24 : h, m, s);

    return 0;
}


static NORETURN void proc_displayRefresh(void)
{
	while (1)
	{
		//LOG_INFO("Refresh display\n");

		status.internal_temp = hw_convertToDegree(adc_read(ADC_TEMPERATURE_CH));
		status.up_time += 1;
		sec_to_strDhms(status.up_time, (char *)tx_buf, sizeof(tx_buf));

		gfx_bitmapClear(lcd_bitmap);
		text_style(lcd_bitmap, STYLEF_BOLD | STYLEF_UNDERLINE, STYLEF_BOLD | STYLEF_UNDERLINE);
		text_xprintf(lcd_bitmap, 0, 0, TEXT_CENTER | TEXT_FILL, "BeRTOS Simple Http Server");
		text_style(lcd_bitmap, 0, STYLEF_MASK);
		text_xprintf(lcd_bitmap, 2, 0, 0, "Board ip: %s", status.local_ip);
		text_xprintf(lcd_bitmap, 3, 0, 0, "Last connected ip: %s", status.last_connected_ip);
		text_xprintf(lcd_bitmap, 4, 0, 0, "Temperature: %d.%dC",	status.internal_temp / 10, status.internal_temp % 10);
		text_xprintf(lcd_bitmap, 5, 0, 0, "Up time: %s", tx_buf);

		lcd_hx8347_blitBitmap(lcd_bitmap);

		timer_delay(1000);
	}
}

/*
 * Return a JSON string of board status.
 */
static int cgi_status(struct netconn *client, const char *name, char *recv_buf, size_t recv_len)
{
	(void)recv_buf;
	(void)recv_len;
	(void)name;

	status.tot_req++;
	uint16_t volt = adc_read(1);

	//Update board status.
	sprintf(status.last_connected_ip, "%d.%d.%d.%d", IP_ADDR_TO_INT_TUPLE(client->pcb.ip->remote_ip.addr));
	sprintf(status.local_ip, "%d.%d.%d.%d", IP_ADDR_TO_INT_TUPLE(client->pcb.ip->local_ip.addr));

	sprintf((char *)tx_buf,	"{ \"local_ip\":\"%s\",\"last_connected_ip\":\"%s\", \"temp\":%d.%d,\"volt\":%d,\"up_time\":%ld,\"tot_req\":%d, \
\"leds\":{ \"0\":\"%d\", \"1\":\"%d\", \"2\":\"%d\"}}",
								status.local_ip, status.last_connected_ip,
								status.internal_temp / 10, status.internal_temp % 10,
								volt, status.up_time, status.tot_req,
								GET_LED_STATUS(status.led_status, 0),
								GET_LED_STATUS(status.led_status, 1),
								GET_LED_STATUS(status.led_status, 2));




	http_sendOk(client, HTTP_CONTENT_JSON);
	netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);
	return 0;
}

static int cgi_logo(struct netconn *client, const char *name, char *recv_buf, size_t recv_len)
{
	(void)recv_buf;
	(void)recv_len;
	(void)name;

	http_sendOk(client, HTTP_CONTENT_JPEG);
	netconn_write(client, bertos_logo_jpg, bertos_logo_jpg_len, NETCONN_NOCOPY);
	return 0;
}

/*
 * Return the internal micro temperature string.
 */
static int cgi_temp(struct netconn *client, const char *name, char *recv_buf, size_t recv_len)
{
	(void)recv_buf;
	(void)recv_len;
	(void)name;

	sprintf((char *)tx_buf, "[%d.%d]", status.internal_temp / 10, status.internal_temp % 10);

	http_sendOk(client, HTTP_CONTENT_JSON);
	netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);
	return 0;
}


/*
 * Return the board uptime.
 */
static int cgi_uptime(struct netconn *client, const char *name, char *recv_buf, size_t recv_len)
{
	(void)recv_buf;
	(void)recv_len;
	(void)name;

	sec_to_strDhms(status.up_time, (char *)tx_buf, sizeof(tx_buf));

	http_sendOk(client, HTTP_CONTENT_JSON);
	netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);
	return 0;
}

/*
 * Return the VR1 potentiometer voltage.
 */
static int cgi_resistor(struct netconn *client, const char *name, char *recv_buf, size_t recv_len)
{
	(void)recv_buf;
	(void)recv_len;
	(void)name;

	uint16_t volt = ADC_RANGECONV(adc_read(1), 0, 3300);
	kprintf("volt %d\n", volt);
	sprintf((char *)tx_buf, "[ \"%d.%dV\" ]",  volt / 1000, volt % 1000);

	http_sendOk(client, HTTP_CONTENT_JSON);
	netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);
	return 0;
}


#define CGI_LED_ID_KEY    "n"
#define CGI_LED_CMD_KEY   "set"

static char key_value[80];

/*
 * Reply to client the request string.
 */
static int cgi_led(struct netconn *client, const char *name, char *recv_buf, size_t recv_len)
{
	(void)recv_buf;
	(void)recv_len;
	(void)name;

	char *query_str = strstr(name, "?") + 1;
	size_t query_str_len = strlen(query_str);
	int led_id;
	int led_cmd;

	int len = http_tokenizeGetRequest(query_str, query_str_len);

	LOG_INFO("Found %d key/value pair\n", len);

	if (http_getValue(query_str, query_str_len, CGI_LED_ID_KEY, key_value, sizeof(key_value)) < 0)
	{
		LOG_ERR("key %s, not found\n", CGI_LED_ID_KEY);
		goto error;
	}

	LOG_INFO("Found key %s = %s\n", CGI_LED_ID_KEY, key_value);
	led_id = atoi(key_value);


	if (http_getValue(query_str, query_str_len, CGI_LED_CMD_KEY, key_value, sizeof(key_value)) < 0)
	{
		LOG_ERR("key %s, not found\n", CGI_LED_CMD_KEY);
		goto error;
	}

	LOG_INFO("Found key %s = %s\n", CGI_LED_CMD_KEY, key_value);
	led_cmd = atoi(key_value);

	if (led_cmd)
	{
		LED_ON(led_id);
		SET_LED_STATUS(status.led_status, led_id);
	}
	else
	{
		LED_OFF(led_id);
		CLEAR_LED_STATUS(status.led_status, led_id);
	}

	sprintf((char *)tx_buf, "{\"n\":%d, \"set\":,%d}", led_id, led_cmd);

	http_sendOk(client, HTTP_CONTENT_JSON);
	netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);
	return 0;

error:
	http_sendInternalErr(client, HTTP_CONTENT_JSON);
	return 0;
}

static int cgi_ledStatus(struct netconn *client, const char *name, char *recv_buf, size_t recv_len)
{
	(void)recv_buf;
	(void)recv_len;
	(void)name;

	sprintf((char *)tx_buf, "{ \"0\":\"%d\", \"1\":\"%d\", \"2\":\"%d\"}",
								GET_LED_STATUS(status.led_status, 0),
								GET_LED_STATUS(status.led_status, 1),
								GET_LED_STATUS(status.led_status, 2));

	http_sendOk(client, HTTP_CONTENT_JSON);
	netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);
	return 0;
}


#define CGI_MSG_CMD_KEY   "msg"

static int cgi_displayMsg(struct netconn *client, const char *name, char *recv_buf, size_t recv_len)
{
	(void)recv_buf;
	(void)recv_len;
	(void)name;

	char *query_str = strstr(name, "?") + 1;
	size_t query_str_len = strlen(query_str);

	int len = http_tokenizeGetRequest(query_str, query_str_len);

	LOG_INFO("Found %d key/value pair\n", len);

	if (http_getValue(query_str, query_str_len, CGI_MSG_CMD_KEY, key_value, sizeof(key_value)) > 0)
	{

		LOG_INFO("Found key %s = %s\n", CGI_MSG_CMD_KEY, key_value);

		gfx_bitmapClear(lcd_bitmap);
		text_style(lcd_bitmap, STYLEF_BOLD | STYLEF_UNDERLINE, STYLEF_BOLD | STYLEF_UNDERLINE);
		text_xprintf(lcd_bitmap, 0, 0, TEXT_CENTER | TEXT_FILL, "BeRTOS Simple Http Server");
		text_style(lcd_bitmap, 0, STYLEF_MASK);
		text_xprintf(lcd_bitmap, 2, 0, TEXT_CENTER | TEXT_FILL, "Your message:");
		text_xprintf(lcd_bitmap, 10, 0, TEXT_CENTER, "%s", key_value);

		lcd_hx8347_blitBitmap(lcd_bitmap);

		http_sendOk(client, HTTP_CONTENT_JSON);
		return 0;
	}

	http_sendInternalErr(client, HTTP_CONTENT_JSON);
	return 0;
}

/*
 * Reply to client the request string.
 */
static int cgi_echo(struct netconn *client, const char *name, char *recv_buf, size_t recv_len)
{
	(void)name;

	http_sendOk(client, HTTP_CONTENT_PLAIN);
	netconn_write(client, recv_buf, recv_len, NETCONN_COPY);
	return 0;
}

/*
 * Return to client a string that display the CHIP ID information.
 * See datasheet for more detail.
 */
static int cgi_chipInfo(struct netconn *client, const char *name, char *recv_buf, size_t recv_len)
{
	(void)recv_buf;
	(void)recv_len;
	(void)name;

	sprintf((char *)tx_buf, "{ \"core_name\":\"%s\", \"arch_name\":\"%s\", \"sram_size\":\"%s\",\"flash_size\":\"%s\", \"mem_boot_type\":\"%s\" }",
						chipid_eproc_name(CHIPID_EPRCOC()),
						chipid_archnames(CHIPID_ARCH()),
						chipid_sramsize(CHIPID_SRAMSIZ()),
						chipid_nvpsize(CHIPID_NVPSIZ()),
						chipid_nvptype(CHIPID_NVTYP()));

	http_sendOk(client, HTTP_CONTENT_JSON);
	netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);

	return 0;
}


static int cgi_error(struct netconn *client, const char *name, char *recv_buf, size_t recv_len)
{
	(void)client;
	(void)name;
	(void)recv_buf;
	(void)recv_len;

	return -1;
}


/*
 * Default function that http server call every client request, if it doesn't match a cgi table.
 * In this implementation all client request are associate to real file stored on FAT file
 * sistem on SD card. If the file there is not on SD card the server reply to client the
 * error File not found, and send an harcoded page. In the same way, the server reply
 * error page if the SD card is not present.
 *
 */
static int http_htmPageLoad(struct netconn *client, const char *name, char *recv_buf, size_t recv_len)
{
	(void)recv_buf;
	(void)recv_len;

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

					int type = http_searchContentType(name);
					http_sendOk(client, type);

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
					http_sendFileNotFound(client, HTTP_CONTENT_HTML);
					netconn_write(client, http_file_not_found, http_file_not_found_len - 1, NETCONN_NOCOPY);
				}
			}
		}
		f_mount(0, NULL);
		LOG_INFO("Umount FAT filesystem.\n");
	}
	else
	{
		http_sendFileNotFound(client, HTTP_CONTENT_HTML);
		netconn_write(client, http_sd_not_present, http_sd_not_present_len, NETCONN_NOCOPY);
	}

	return 0;
}


/*
 * Static cgi table where we associate callback to page.
 */
static HttpCGI cgi_table[] =
{
	{ CGI_MATCH_NAME, "echo",                cgi_echo          },
	{ CGI_MATCH_NAME, "get_temperature",     cgi_temp          },
	{ CGI_MATCH_NAME, "get_uptime",          cgi_uptime        },
	{ CGI_MATCH_NAME, "get_resistor",        cgi_resistor      },
	{ CGI_MATCH_NAME, "set_led",             cgi_led           },
	{ CGI_MATCH_NAME, "get_ledStatus",       cgi_ledStatus     },
	{ CGI_MATCH_NAME, "error",               cgi_error         },
	{ CGI_MATCH_NAME, "status",              cgi_status        },
	{ CGI_MATCH_NAME, "get_chipinfo",        cgi_chipInfo      },
	{ CGI_MATCH_NAME, "display",             cgi_displayMsg    },
	{ CGI_MATCH_NAME, "bertos_logo_jpg",     cgi_logo          },
	{ CGI_MATCH_NONE,  NULL,                 NULL              }
};


int main(void)
{
	struct netconn *server;

	/* Hardware initialization */
	init();
	http_init(http_htmPageLoad, cgi_table);

	lcd_hx8347_blitBitmap24(12, 52, BMP_LOGO_WIDTH, BMP_LOGO_HEIGHT, bmp_logo);

	dhcp_start(&netif);
	while (!netif.ip_addr.addr)
		timer_delay(DHCP_FINE_TIMER_MSECS);

	sprintf(status.local_ip, "%d.%d.%d.%d", IP_ADDR_TO_INT_TUPLE(netif.ip_addr.addr));
	sprintf(status.last_connected_ip, "%d.%d.%d.%d", IP_ADDR_TO_INT_TUPLE(0));
	LOG_INFO("dhcp ok: ip = %s\n", status.local_ip);

	text_xprintf(lcd_bitmap, 14, 0, 0, "Board ip: %s", status.local_ip);
	lcd_hx8347_blitBitmap(lcd_bitmap);


	proc_new(proc_displayRefresh, NULL, KERN_MINSTACKSIZE * 2, NULL);

	server = netconn_new(NETCONN_TCP);
	netconn_bind(server, IP_ADDR_ANY, 80);
	netconn_listen(server);

	while (1)
	{
		http_poll(server);
	}
}
