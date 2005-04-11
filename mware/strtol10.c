/*!
 * \file
 * <!--
 * Copyright 2005 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Poor man's hex arrays (implementation).
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2005/04/11 18:10:45  bernie
 *#* Doxygen fixes.
 *#*
 *#* Revision 1.1  2005/03/15 00:06:30  bernie
 *#* Simpler, smaller, faster.
 *#*
 *#*/

#include "strtol10.h"

/*!
 * Convert a formatted base-10 ASCII number to unsigned long binary representation.
 *
 * Unlike the standard strtoul(), this function has an interface
 * that makes it better suited for protocol parsers.  It's also
 * much simpler and smaller than a full featured strtoul().
 *
 * \param first  Pointer to first byte of input range (STL-style).
 * \param last   Pointer to end of input range (STL-style).
 *               Pass NULL to parse up to the first \\0.
 * \param val    Pointer to converted value.
 *
 * \return true for success, false for failure.
 *
 * \see strtol10()
 */
bool strtoul10(const char *first, const char *last, unsigned long *val)
{
	// Check for no input
	if (*first == '\0')
		return false;

	*val = 0;
	for(/*nop*/; first != last && *first != '\0'; ++first)
	{
		if ((*first < '0') || (*first > '9'))
			return false;

		*val = (*val * 10L) + (*first - '0');
	}

	return true;
}


/*!
 * Convert a formatted base-10 ASCII number to signed long binary representation.
 *
 * \see strtoul10()
 */
bool strtol10(const char *first, const char *last, long *val)
{
	bool negative = false;

	if (*first == '+')
		++first; /* skip unary plus sign */
	else if (*first == '-')
	{
		negative = true;
		++first;
	}

	bool result = strtoul10(first, last, (unsigned long *)val);

	if (negative)
		*val = - *val;

	return result;
}

