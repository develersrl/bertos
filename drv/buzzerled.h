/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief Generic library to handle buzzers and leds
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */

/*
 * $Log$
 * Revision 1.4  2004/07/30 14:15:53  rasky
 * Nuovo supporto unificato per detect della CPU
 *
 * Revision 1.3  2004/07/14 14:04:29  rasky
 * Merge da SC: spostata bld_set inline perché si ottimizza parecchio tramite propagazione di costanti
 *
 * Revision 1.2  2004/06/03 11:27:09  bernie
 * Add dual-license information.
 *
 * Revision 1.1  2004/05/23 18:36:05  bernie
 * Import buzzerled driver.
 *
 */

#ifndef DRV_BUZZERLED_H
#define DRV_BUZZERLED_H

#include <cpu.h>

/*! Include hw.h. We expect hw.h to define enum BLD_DEVICE, which must contain
 *  an enumarator for each device, plus a special symbol NUM_BLDS containing the
 *  number of devices.
 */
#include <hw.h>


/* Include hw-level implementation. This allows inlining of bld_set, which in turns
 * should allow fast constant propagation for the common case (where the parameter
 * device is a constant).
 */
#include CPU_HEADER(buzzerled)


/*! Initialize the buzzerled library.
 *
 * \note This function must be called before any other function in the library.
 */
void bld_init(void);


/*! Set or reset a device.
 *
 * \param device Device to be set
 * \param enable Enable/disable status
 */
#define bld_set(device, enable)  bld_hw_set(device, enable)


/*! Enable a device for a certain interval of time
 *
 * \param device Device to be enabled
 * \param duration Number of milliseconds the device must be enabled
 *
 * \note This function is non-blocking, so it will return immediately.
 */
void bld_beep(enum BLD_DEVICE device, uint16_t duration);


/*! Enable a device for a certain interval of time and wait.
 *
 * \param device Device to be enabled
 * \param duration Number of milliseconds the device must be enabled
 *
 * \note This function is blocking, so it will return after the specified period of time.
 */
void bld_beep_and_wait(enum BLD_DEVICE device, uint16_t duration);

#endif /* DRV_BUZZERLED_H */
