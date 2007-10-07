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
 * \brief System irq handler for Atmel AT91 ARM7 processors (interface).
 */

#ifndef DRV_AT91_SYSIRQ_H
#define DRV_AT91_SYSIRQ_H

typedef void (* sysirq_handler_t)(void);   ///< Type for system irq handler.
typedef void (* sysirq_setEnable_t)(bool); ///< Type for system irq enable/disable function.
typedef bool (* sysirq_enable_t)(void);    ///< Type for system irq enable getter.
typedef bool (* sysirq_trigger_t)(void);   ///< Type for system irq trigger getter.

/**
 * Structure used to define a system interrupt source.
 */
typedef struct SysIrq
{
	sysirq_enable_t enable;       ///< Getter for irq enable/disable state.
	sysirq_trigger_t trigger;     ///< Getter for irq trigger state.
	sysirq_setEnable_t setEnable; ///< Setter for irq enable/disable state.
	sysirq_handler_t handler;     ///< IRQ handler.
} SysIrq;

/**
 * System IRQ ID list.
 */
typedef enum sysirq_t
{
	SYSIRQ_PIT, ///< Periodic Interval Timer
	/* TODO: add all system irqs */
	SYSIRQ_CNT
} sysirq_t;

void sysirq_init(void);
void sysirq_setHandler(sysirq_t irq, sysirq_handler_t handler);
void sysirq_setEnable(sysirq_t irq, bool enable);
bool sysirq_enable(sysirq_t irq);

#endif /* ARCH_ARM_SYSIRQ_H */
