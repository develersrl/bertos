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
 * \brief AT91SAM7S-EK porting test.
 */

#include <drv/timer.h>
#include <cfg/macros.h>

int main(void)
{
//	kdbg_init();
	timer_init();
	IRQ_ENABLE;

	// Main loop
	for(;;)
	{

	}

	return 0;
}
