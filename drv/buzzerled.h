/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 * All Rights Reserved.
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
 * Revision 1.1  2004/05/23 18:36:05  bernie
 * Import buzzerled driver.
 *
 */

#ifndef DRV_BUZZERLED_H
#define DRV_BUZZERLED_H

/*! Include hw.h. We expect hw.h to define enum BLD_DEVICE, which must contain
 *  an enumarator for each device, plus a special symbol NUM_BLDS containing the
 *  number of devices.
 */
#include <hw.h>

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
void bld_set(enum BLD_DEVICE device, bool enable);

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
