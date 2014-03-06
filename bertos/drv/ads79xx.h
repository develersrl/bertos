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
 * \brief Texas ADS79xx ADC driver.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "ads79xx"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_ads79xx.h"
 * $WIZ$ module_depends = "lwip", "kern"
 */

#ifndef DRV_ADS79XX_H
#define DRV_ADS79XX_H

#include "ads79xx_macros.h"
#include "cfg/cfg_ads79xx.h"

#include <cfg/compiler.h>
#include <drv/adc.h>
#include <net/tcp_socket.h>

#include <mware/event.h>

#define ADC_TCP_SAMPLE_COUNT 192

#define SAMPLE_RATE_RATIO (CONFIG_ADS79XX_SAMPLE_RATE / CONFIG_ADCTCP_SAMPLE_RATE)
STATIC_ASSERT(CONFIG_ADS79XX_SAMPLE_RATE % CONFIG_ADCTCP_SAMPLE_RATE == 0);

// TODO: AUTO2 not handled
typedef enum
{
	ADS_MANUAL,
	ADS_AUTO1,
}Ads79xxMode;


typedef enum
{
	ADS_RANGE25 = 0, /// Selects +2.5V i/p range
	ADS_RANGE50 = 1, /// Selects +5V i/p range
}Ads79xxRange;

typedef enum
{
	ADC_STREAMING,
	ADC_FILL_BUF,
} AdcMode;

typedef struct Ads79xx
{
	AdcContext adc;
	uint32_t mask;
	AdcMode mode;
	Event dma_done;
	Ads79xxRange range;
} Ads79xx;

#define ADC_ADS79XX MAKE_ID('A', 'D', '7', '9')

INLINE Ads79xx *ADS79XX_CAST(AdcContext *ctx)
{
	ASSERT(ctx->_type == ADC_ADS79XX);
	return (Ads79xx *)ctx;
}

void ads79xx_init(Ads79xx *ctx, Ads79xxRange range);
void adc_reset(void);


typedef struct Ads79xxTcp
{
	Ads79xx ads;
	TcpSocket insock;
	uint8_t * volatile tail_tcp;
	uint8_t * volatile head_tcp;
	uint8_t *tcp_buf;
	size_t tcpbuf_len;
	Event buffer_ready;
	int server_sock, in_sock;
} Ads79xxTcp;

#define ADC_ADS79XXTCP MAKE_ID('A', 'T', 'C', 'P')

INLINE Ads79xxTcp *ADS79XXTCP_CAST(AdcContext *ctx)
{
	return (Ads79xxTcp *)ctx;
}

void ads79xxtcp_init(Ads79xxTcp *ctx, Ads79xxRange range, uint8_t *tcp_buf, size_t tcpbuf_len);



#endif /* DRV_ADS79XX_H */
