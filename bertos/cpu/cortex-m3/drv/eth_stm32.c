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
  * Copyright 2012 Develer S.r.l. (http://www.develer.com/)
  * All Rights Reserved.
  * -->
  *
  * \brief Ethernet driver for STM-32 CPU family.
  *
  * \author Mattia Barbon <mattia@develer.com>
  */

#include "cfg/cfg_eth.h"

#define LOG_LEVEL  ETH_LOG_LEVEL
#define LOG_FORMAT ETH_LOG_FORMAT

#include <cfg/log.h>

#include <drv/eth.h>
#include <drv/irq_cm3.h>
#include <drv/timer.h>

#include <mware/event.h>

#include "eth_stm32.h"
#include "gpio_stm32.h"
#include "clock_stm32.h"

#include <string.h>

static char rx_buffer[MAC_RX_BUFFER] ALIGNED(4);
static struct stm32_eth_dma_descr rx_descr ALIGNED(4);
static struct stm32_eth_dma_descr tx_descr ALIGNED(4);
static char tx_buffer[MAC_TX_BUFFER] ALIGNED(4);

static Event recv_wait, send_wait;

static void stm32_eth_irqHandler(void)
{
	/* Receiver interrupt */
	if (ETH->DMASR & MAC_DMA_RX_INT)
	{
		/* reset interrupt */
		ETH->DMASR = MAC_DMA_RX_INT;

		event_do(&recv_wait);
	}
	/* Transmitter interrupt */
	if (ETH->DMASR & (MAC_DMA_TX_INT | MAC_DMA_TX_NOBUF_INT))
	{
		/* reset interrupt */
		ETH->DMASR = MAC_DMA_TX_INT | MAC_DMA_TX_NOBUF_INT;

		event_do(&send_wait);
	}
	ETH->DMASR = MAC_DMA_NORM_INT;
	ETH->DMASR = MAC_DMA_ABN_INT;
}

static uint16_t eth_read_phy_register(uint16_t phy_address, uint16_t reg)
{
	uint32_t tmpreg = 0;
	volatile uint32_t timeout = 0;

	/* Get the ETHERNET MACMIIAR value */
	tmpreg = ETH->MACMIIAR;
	/* Keep only the CSR Clock Range CR[2:0] bits value */
	tmpreg &= ~ETH_MACMIIAR_CR_MASK;
	/* Prepare the MII address register value */
	tmpreg |=(((uint32_t)phy_address<<11) & ETH_MACMIIAR_PA); /* Set the PHY device address */
	tmpreg |=(((uint32_t)reg<<6) & ETH_MACMIIAR_MR);      /* Set the PHY register address */
	tmpreg &= ~ETH_MACMIIAR_MW;                              /* Set the read mode */
	tmpreg |= ETH_MACMIIAR_MB;                               /* Set the MII Busy bit */
	/* Write the result value into the MII Address register */
	ETH->MACMIIAR = tmpreg;
	/* Check for the Busy flag */
	do
	{
		timeout++;
		tmpreg = ETH->MACMIIAR;
	} while ((tmpreg & ETH_MACMIIAR_MB) && (timeout < (uint32_t)PHY_READ_TIMEOUT));
	/* Return ERROR in case of timeout */
	if(timeout == PHY_READ_TIMEOUT)
	{
		return -1;
	}

	/* Return data register value */
	return (uint16_t)(ETH->MACMIIDR);
}

static uint32_t eth_write_phy_register(uint16_t phy_address, uint16_t reg, uint16_t value)
{
	uint32_t tmpreg = 0;
	volatile uint32_t timeout = 0;

	/* Get the ETHERNET MACMIIAR value */
	tmpreg = ETH->MACMIIAR;
	/* Keep only the CSR Clock Range CR[2:0] bits value */
	tmpreg &= ~ETH_MACMIIAR_CR_MASK;
	/* Prepare the MII register address value */
	tmpreg |=(((uint32_t)phy_address<<11) & ETH_MACMIIAR_PA); /* Set the PHY device address */
	tmpreg |=(((uint32_t)reg<<6) & ETH_MACMIIAR_MR);      /* Set the PHY register address */
	tmpreg |= ETH_MACMIIAR_MW;                               /* Set the write mode */
	tmpreg |= ETH_MACMIIAR_MB;                               /* Set the MII Busy bit */
	/* Give the value to the MII data register */
	ETH->MACMIIDR = value;
	/* Write the result value into the MII Address register */
	ETH->MACMIIAR = tmpreg;
	/* Check for the Busy flag */
	do
	{
		timeout++;
		tmpreg = ETH->MACMIIAR;
	} while ((tmpreg & ETH_MACMIIAR_MB) && (timeout < (uint32_t)PHY_WRITE_TIMEOUT));
	/* Return ERROR in case of timeout */
	if(timeout == PHY_WRITE_TIMEOUT)
	{
		return 0;
	}

	/* Return SUCCESS */
	return 1;
}

static void eth_init_phy(void)
{
	uint32_t tmp = ETH->MACMIIAR;
	uint i;

	tmp &= ETH_MACMIIAR_CR_MASK; /* clear PHY clock range */
#if CPU_FREQ <= 35000000
	tmp |= ETH_MACMIIAR_CR_16;
#elif CPU_FREQ <= 60000000
	tmp |= ETH_MACMIIAR_CR_26;
#elif CPU_FREQ <= 100000000
	tmp |= ETH_MACMIIAR_CR_42;
#elif CPU_FREQ <= 168000000
	tmp |= ETH_MACMIIAR_CR_62;
#else
#error "Unhandled CPU frequency range"
#endif
	tmp |= 1 << 2; /* 62 divisor, good for 120Mhz, use table for other frequencies */

	ETH->MACMIIAR = tmp;

	if (!eth_write_phy_register(DP83848_PHY_ADDRESS, PHY_BCR, PHY_Reset))
		LOG_WARN("PHY reset failed\n");
	/* according to datasheet, it takes 1 us for reset and 3 us before driver can communicate again */
	timer_udelay(10);

	/* wait for link up */
	for (i = 0; i < PHY_READ_TIMEOUT && !(eth_read_phy_register(DP83848_PHY_ADDRESS, PHY_BSR) & PHY_Link_Up); ++i)
		;
	if (i == PHY_READ_TIMEOUT)
		LOG_WARN("PHY link-up timeout\n");

	/* wait for auto-negotiation */
	if (!eth_write_phy_register(DP83848_PHY_ADDRESS, PHY_BCR, PHY_Autonegotiation))
		LOG_WARN("PHY auto-negotiation\n");

	for (i = 0; i < PHY_READ_TIMEOUT && !(eth_read_phy_register(DP83848_PHY_ADDRESS, PHY_BSR) & PHY_Autonegotiation_Complete); ++i)
		;
	if (i == PHY_READ_TIMEOUT)
		LOG_WARN("PHY auto-negotiation timeout\n");

	/* configure duplex mode and speed on mac */
	uint32_t phy_sr = eth_read_phy_register(DP83848_PHY_ADDRESS, PHY_SR);

	if (phy_sr & PHY_SR_FULL_DUPLEX)
	{
		LOG_INFO("PHY full-duplex\n");
		ETH->MACCR |= MAC_FULL_DUPLEX;
	}
	else
	{
		LOG_INFO("PHY half-duplex\n");
		ETH->MACCR &= ~MAC_FULL_DUPLEX;
	}

	if (phy_sr & PHY_SR_10_MBIT)
	{
		LOG_INFO("PHY 10Mbit\n");
		ETH->MACCR &= ~MAC_100_MBIT;
	}
	else
	{
		LOG_INFO("PHY 100Mbit\n");
		ETH->MACCR |= MAC_100_MBIT;
	}
}

int eth_init(void)
{
	/* enable GPIO A, B, C, G, H, I, used below */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_GPIOHEN | RCC_AHB1ENR_GPIOIEN;

	/* enable SYSCFG clock */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* Configure MCO1 (PA8) */
	stm32_gpioPinConfig(GPIOA, BV(8),
			    GPIO_MODE_AF_PP | GPIO_AF_SYS, GPIO_SPEED_100MHZ);

	/* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
	uint32_t cfgr = RCC->CFGR;

	cfgr &= ~(RCC_CFGR_MCO1_SRC_MASK | RCC_CFGR_MCO_DIV_MASK);
	cfgr |= RCC_CFGR_MCO1_SRC_HSE | RCC_CFGR_MCO_DIV_1;

	RCC->CFGR = cfgr;

	/* 0 = MII, 1 = RMII, default is MII */
	/* SYSCFG->PMC &= ~BV(23); */

	/*
		For STM322xG-EVAL Board

		ETH_MDIO                            PA2
		ETH_MDC                             PC1
		ETH_PPS_OUT                         PB5
		ETH_MII_CRS                         PH2
		ETH_MII_COL                         PH3
		ETH_MII_RX_ER                       PI10
		ETH_MII_RXD2                        PH6
		ETH_MII_RXD3                        PH7
		ETH_MII_TX_CLK                      PC3
		ETH_MII_TXD2                        PC2
		ETH_MII_TXD3                        PB8
		ETH_MII_RX_CLK/ETH_RMII_REF_CLK     PA1
		ETH_MII_RX_DV/ETH_RMII_CRS_DV       PA7
		ETH_MII_RXD0/ETH_RMII_RXD0          PC4
		ETH_MII_RXD1/ETH_RMII_RXD1          PC5
		ETH_MII_TX_EN/ETH_RMII_TX_EN        PG11
		ETH_MII_TXD0/ETH_RMII_TXD0          PG13
		ETH_MII_TXD1/ETH_RMII_TXD1          PG14
	 */

	/* Configure PA1, PA2 and PA7 */
	stm32_gpioPinConfig(GPIOA, BV(1) | BV(2) | BV(7),
			    GPIO_MODE_AF_PP | GPIO_AF_ETH, GPIO_SPEED_100MHZ);

	/* Configure PB5 and PB8 */
	stm32_gpioPinConfig(GPIOB, BV(5) | BV(8),
			    GPIO_MODE_AF_PP | GPIO_AF_ETH, GPIO_SPEED_100MHZ);

	/* Configure PC1, PC2, PC3, PC4 and PC5 */
	stm32_gpioPinConfig(GPIOC, BV(1) | BV(2) | BV(3) | BV(4) | BV(5),
			    GPIO_MODE_AF_PP | GPIO_AF_ETH, GPIO_SPEED_100MHZ);

	/* Configure PG11, PG14 and PG13 */
	stm32_gpioPinConfig(GPIOG, BV(11) | BV(13) | BV(14),
			    GPIO_MODE_AF_PP | GPIO_AF_ETH, GPIO_SPEED_100MHZ);

	/* Configure PH2, PH3, PH6, PH7 */
	stm32_gpioPinConfig(GPIOH, BV(2) | BV(3) | BV(6) | BV(7),
			    GPIO_MODE_AF_PP | GPIO_AF_ETH, GPIO_SPEED_100MHZ);

	/* Configure PI10 */
	stm32_gpioPinConfig(GPIOI, BV(10),
			    GPIO_MODE_AF_PP | GPIO_AF_ETH, GPIO_SPEED_100MHZ);

	/* Enable Ethernet clocks (low power mode) */
	RCC->AHB1ENR |= RCC_AHB1LPENR_ETHMACLPEN | RCC_AHB1LPENR_ETHMACTXLPEN | RCC_AHB1LPENR_ETHMACRXLPEN;

	/* Reset Ethernet on AHB Bus */
	RCC->AHB1RSTR |= BV(25);
	RCC->AHB1RSTR &= ~BV(25);

	/* Software reset */
	ETH->DMABMR |= BV(0);

	/* Wait for software reset to complete */
	while (ETH->DMABMR & BV(0));

	/* reset PHY and wait for autoconfiguration, then set mode/speed in MACCR register */
	eth_init_phy();

	/* set-up RX DMA descriptors */
	rx_descr.descr0 = MAC_DESC0_DMA_OWN;
	rx_descr.descr1 = MAC_DESC1_RX_EOR | sizeof(rx_buffer);
	rx_descr.buffer1 = rx_buffer;
	rx_descr.buffer2 = NULL;

	/* TODO enable checksum offload? */

	event_initGeneric(&recv_wait);
	event_initGeneric(&send_wait);

	sysirq_setHandler(ETH_IRQHANDLER, stm32_eth_irqHandler);

	/* enable IRQ: transmit, receive, no txbuf, plus normal (for tx and rx) and abnormal (for no txbuf) */
	ETH->DMAIER |= MAC_DMA_TX_INT | MAC_DMA_RX_INT | MAC_DMA_TX_NOBUF_INT | MAC_DMA_ABN_INT | MAC_DMA_NORM_INT;

	/* enable transmit and receive */
	ETH->MACCR |= MAC_RX_ENABLE | MAC_TX_ENABLE;

	/* start RX DMA */
	ETH->DMARDLAR = (uint32_t) &rx_descr;
	ETH->DMAOMR |= MAC_DMA_RX | MAC_DMA_TSF;

	return 0;
}

void eth_cleanup(void)
{
	/* Disable all IRQs */
	ETH->DMAIER = 0;

	/* disable GPIOs */
	RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_GPIOHEN | RCC_AHB1ENR_GPIOIEN);

	/* disable system clock */
	RCC->APB2ENR &= ~RCC_APB2ENR_SYSCFGEN;

	/* disable DMAs and logic */
	ETH->DMAOMR &= ~(MAC_DMA_RX | MAC_DMA_TX);
	ETH->MACCR |= MAC_RX_ENABLE | MAC_TX_ENABLE;
}

static void eth_waitTxReady(void)
{
	/* Check if the transmit buffer is available */
	if (ETH->DMAOMR & MAC_DMA_TX)
	{
		if (!event_waitTimeout(&send_wait, 1000))
		{
			kprintf("DMASR %08lX\n", ETH->DMASR);
			kprintf("DMAOMR %08lX\n", ETH->DMAOMR);
			kprintf("MACCR %08lX\n", ETH->MACCR);
			kprintf("DMAIER %08lX\n", ETH->DMAIER);
			kprintf("MACDBGR %08lX\n", ETH->MACDBGR);
			kprintf("TXDESC:descr0 %08lX\n", tx_descr.descr0);
			kprintf("TXDESC:descr1 %08lX\n", tx_descr.descr1);
		}
	}
}

void eth_send(const struct EthernetDescriptor *list, size_t count)
{
	eth_waitTxReady();

	size_t tx_len = 0;
	for (size_t i = 0; i < count; i++)
	{
		ASSERT(tx_len + list[i].len < MAC_TX_BUFFER);
		memcpy(&tx_buffer[tx_len], list[i].data, list[i].len);
		tx_len += list[i].len;
	}

	tx_descr.descr0 = 0;
	tx_descr.buffer1 = tx_buffer;
	tx_descr.descr1 = tx_len;
	tx_descr.buffer2 = &tx_descr;

	/* mark first segment of frame */
	tx_descr.descr0 |= MAC_DESC0_TX_FS | MAC_DESC0_TX_LS | MAC_DESC0_TX_TCH | MAC_DESC0_TX_IC | MAC_DESC0_DMA_OWN;

	/* start transfer */
	ETH->DMATDLAR = (uint32_t) &tx_descr;
	ETH->DMAOMR |= MAC_DMA_TX;

	ETH->DMATPDR = 0; /* issue a DMA poll command */
}

size_t eth_recv(struct EthernetDescriptor *list, size_t count)
{
	if (rx_descr.descr0 & MAC_DESC0_DMA_OWN)
		event_wait(&recv_wait);

	char *base = rx_buffer;
	size_t length = (rx_descr.descr0 >> 16) & 0x3fff;

	for (size_t i = 0; i < count && length > 0; ++i)
	{
		size_t sz = MIN(list[i].len, length);

		memcpy(list[i].data, base, sz);
		base += sz;
		length -= sz;
	}

	rx_descr.descr0 |= MAC_DESC0_DMA_OWN;

	/* poll for new descriptor if DMA is paused */
	if ((ETH->DMASR & (0x7 << 17)) == (0x4 << 17))
		ETH->DMARPDR = 0;

	return base - rx_buffer;
}

void eth_setMac(MacAddress mac)
{
	ETH->MACA0HR = (ETH->MACA0HR & 0xffff0000) | (mac.addr[5] << 8) | (mac.addr[4] << 0);
	ETH->MACA0LR = (mac.addr[3] << 24) | (mac.addr[2] << 16) | (mac.addr[1] << 8) | (mac.addr[0] << 0);
}

MacAddress eth_mac(void)
{
	MacAddress mac;
	mac.addr[5] = (ETH->MACA0HR >>  8) & 0xff;
	mac.addr[4] = (ETH->MACA0HR >>  0) & 0xff;
	mac.addr[3] = (ETH->MACA0LR >> 24) & 0xff;
	mac.addr[2] = (ETH->MACA0LR >> 16) & 0xff;
	mac.addr[1] = (ETH->MACA0LR >>  8) & 0xff;
	mac.addr[0] = (ETH->MACA0LR >>  0) & 0xff;
	return mac;
}
