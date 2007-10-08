/**
 * \file
 * <!--
 * Copyright 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Very simple rand() algorithm.
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/* This would really belong to libc */
static int rand(void)
{
	static unsigned long seed;

	/* Randomize seed */
	seed = (seed ^ 0x4BAD5A39UL) + 6513973UL;

	return (int)(seed>>16);
}
