/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * AT91 advanced interrupt controller.
 * This file is based on NUT/OS implementation. See license below.
 */

/*
 * Copyright (C) 2005-2006 by egnite Software GmbH. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

#ifndef AT91_AIC_H
#define AT91_AIC_H

#include <cfg/compiler.h>



/**
 * Source mode register array.
 */
#define AIC_SMR(i)  (*((volatile uint32_t *)(AIC_BASE + (i) * 4)))

/**
 * Priority mask.
 * Priority levels can be between 0 (lowest) and 7 (highest).
 */
#define AIC_PRIOR_MASK 0x00000007

/** 
 * Interrupt source type mask.
 * Internal interrupts can level sensitive or edge triggered.
 *
 * External interrupts can triggered on positive or negative levels or 
 * on rising or falling edges.
 */
#define AIC_SRCTYPE_MASK 0x00000060

#define AIC_SRCTYPE_INT_LEVEL_SENSITIVE 0x00000000      ///< Internal level sensitive.
#define AIC_SRCTYPE_INT_EDGE_TRIGGERED  0x00000020      ///< Internal edge triggered.
#define AIC_SRCTYPE_EXT_LOW_LEVEL       0x00000000      ///< External low level.
#define AIC_SRCTYPE_EXT_NEGATIVE_EDGE   0x00000020      ///< External falling edge.
#define AIC_SRCTYPE_EXT_HIGH_LEVEL      0x00000040      ///< External high level.
#define AIC_SRCTYPE_EXT_POSITIVE_EDGE   0x00000060      ///< External rising edge.
/*\}*/


/**
 * Type for interrupt handlers.
 */
typedef void (*irq_handler_t)(void);

/** Interrupt Source Vector Registers */
/*\{*/
/** Source vector register array.
 *
 * Stores the addresses of the corresponding interrupt handlers.
 */
#define AIC_SVR(i)  (*((volatile irq_handler_t *)(AIC_BASE + 0x80 + (i) * 4)))
/*\}*/

/** Interrupt Vector Register */
/*\{*/
#define AIC_IVR_OFF 0x00000100  ///< IRQ vector register offset.
#define AIC_IVR     (*((volatile uint32_t *)(AIC_BASE + AIC_IVR_OFF))) ///< IRQ vector register address.
/*\}*/

/** Fast Interrupt Vector Register */
/*\{*/
#define AIC_FVR_OFF 0x00000104  ///< FIQ vector register offset.
#define AIC_FVR     (*((volatile uint32_t *)(AIC_BASE + AIC_FVR_OFF))) ///< FIQ vector register address.
/*\}*/

/** Interrupt Status Register */
/*\{*/
#define AIC_ISR_OFF    0x00000108  ///< Interrupt status register offset.
#define AIC_ISR        (*((volatile uint32_t *)(AIC_BASE + AIC_ISR_OFF))) ///< Interrupt status register address.
#define AIC_IRQID_MASK 0x0000001F  ///< Current interrupt identifier mask.
/*\}*/

/** Interrupt Pending Register */
/*\{*/
#define AIC_IPR_OFF 0x0000010C  ///< Interrupt pending register offset.
#define AIC_IPR     (*((volatile uint32_t *)(AIC_BASE + AIC_IPR_OFF))) ///< Interrupt pending register address.
/*\}*/

/** Interrupt Mask Register */
/*\{*/
#define AIC_IMR_OFF 0x00000110  ///< Interrupt mask register offset.
#define AIC_IMR     (*((volatile uint32_t *)(AIC_BASE + AIC_IMR_OFF))) ///< Interrupt mask register address.
/*\}*/

/** Interrupt Core Status Register */
/*\{*/
#define AIC_CISR_OFF 0x00000114  ///< Core interrupt status register offset.
#define AIC_CISR     (*((volatile uint32_t *)(AIC_BASE + AIC_CISR_OFF))) ///< Core interrupt status register address.
#define AIC_NFIQ     1  ///< Core FIQ Status
#define AIC_NIRQ     2  ///< Core IRQ Status
/*\}*/

/** Interrupt Enable Command Register */
/*\{*/
#define AIC_IECR_OFF 0x00000120  ///< Interrupt enable command register offset.
#define AIC_IECR     (*((volatile uint32_t *)(AIC_BASE + AIC_IECR_OFF)))   ///< Interrupt enable command register address.
/*\}*/

/** Interrupt Disable Command Register */
/*\{*/
#define AIC_IDCR_OFF 0x00000124  ///< Interrupt disable command register offset.
#define AIC_IDCR     (*((volatile uint32_t *)(AIC_BASE + AIC_IDCR_OFF)))   ///< Interrupt disable command register address.
/*\}*/

/** Interrupt Clear Command Register */
/*\{*/
#define AIC_ICCR_OFF 0x00000128  ///< Interrupt clear command register offset.
#define AIC_ICCR     (*((volatile uint32_t *)(AIC_BASE + AIC_ICCR_OFF)))   ///< Interrupt clear command register address.
/*\}*/

/** Interrupt Set Command Register */
/*\{*/
#define AIC_ISCR_OFF 0x0000012C  ///< Interrupt set command register offset.
#define AIC_ISCR     (*((volatile uint32_t *)(AIC_BASE + AIC_ISCR_OFF)))   ///< Interrupt set command register address.
/*\}*/

/** End Of Interrupt Command Register */
/*\{*/
#define AIC_EOICR_OFF 0x00000130  ///< End of interrupt command register offset.
#define AIC_EOICR     (*((volatile uint32_t *)(AIC_BASE + AIC_EOICR_OFF)))  ///< End of interrupt command register address.
/*\}*/

/** Spurious Interrupt Vector Register */
/*\{*/
#define AIC_SPU_OFF 0x00000134  ///< Spurious vector register offset.
#define AIC_SPU     (*((volatile uint32_t *)(AIC_BASE + AIC_SPU_OFF)==    ///< Spurious vector register address.
/*\}*/

/** Debug Control Register */
/*\{*/
#define AIC_DCR_OFF 0x0000138   ///< Debug control register offset.
#define AIC_DCR     (*((volatile uint32_t *)(AIC_BASE + AIC_DCR_OFF)))    ///< Debug control register address.
/*\}*/

/** Fast Forcing Enable Register */
/*\{*/
#define AIC_FFER_OFF 0x00000140  ///< Fast forcing enable register offset.
#define AIC_FFER     (*((volatile uint32_t *)(AIC_BASE + AIC_FFER_OFF)))   ///< Fast forcing enable register address.
/*\}*/

/** Fast Forcing Disable Register */
/*\{*/
#define AIC_FFDR_OFF 0x00000144  ///< Fast forcing disable register address.
#define AIC_FFDR     (*((volatile uint32_t *)(AIC_BASE + AIC_FFDR_OFF)))   ///< Fast forcing disable register address.
/*\}*/

/** Fast Forcing Status Register */
/*\{*/
#define AIC_FFSR_OFF 0x00000148  ///< Fast forcing status register address.
#define AIC_FFSR     (*((volatile uint32_t *)(AIC_BASE + AIC_FFSR_OFF)))   ///< Fast forcing status register address.
/*\}*/

#endif /* AT91_AIC_H */
