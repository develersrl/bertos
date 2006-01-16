/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 *
 * \brief dot-matrix LCD test.
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2006/01/16 03:51:35  bernie
 *#* Add LCD Qt emulator.
 *#*
 *#*/

#include <emul/emul.h>
#include <drv/lcd_gfx.h>

int main(int argc, char *argv[])
{
	emul_init(&argc, argv);
	lcd_init();

	for(;;)
		emul_idle();

	emul_cleanup();
	return 0;
}
