/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \brief General-purpose run-length {en,de}coding algorithm (implementation)
 *
 * Original source code from http://www.compuphase.com/compress.htm
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*
 * $Log$
 * Revision 1.1  2004/08/04 02:35:54  bernie
 * Import simple RLE algorithm.
 *
 */

#include "rle.h"


/*!
 * Run-length encode \a len bytes from the \a input buffer
 * to the \a output buffer.
 */
int rle(unsigned char *output, const unsigned char *input, int len)
{
	int count, index, i;
	unsigned char first;
	unsigned char *out;


	out = output;
	count = 0;
	while (count < len)
	{
		index = count;
		first = input[index++];

		/* Scan for bytes identical to the first one */
		while ((index < len) && (index - count < 127) && (input[index] == first))
			index++;

		if (index - count == 1)
		{
			/* Failed to "replicate" the current byte. See how many to copy.
			 */
			while ((index < len) && (index - count < 127))
			{
				/* Avoid a replicate run of only 2-bytes after a literal run.
				 * There is no gain in this, and there is a risc of loss if the
				 * run after the two identical bytes is another literal run.
				 * So search for 3 identical bytes.
				 */
				if ((input[index] == input[index - 1]) &&
					((index > 1) && (input[index] == input[index - 2])))
				{
					/* Reset the index so we can back up these three identical
					 * bytes in the next run.
					 */
					index -= 2;
					break;
				}

				index++;
			}

			/* Output a run of uncompressed bytes: write length and values */
			*out++ = (unsigned char)(count - index);
			for (i = count; i < index; i++)
				*out++ = input[i];
	    }
		else
		{
			/* Output a compressed run: write length and value */
			*out++ = (unsigned char)(index - count);
			*out++ = first;
	    }

		count = index;
	}

	/* Output EOF marker */
	*out++ = 0;

	return (out - output);
}


/*!
 * Run-length decode from the \a input buffer to the \a output
 * buffer.
 *
 * \note The output buffer must be large enough to accomodate
 *       all decoded output.
 */
int unrle(unsigned char *output, const unsigned char *input)
{
	signed char count;
	unsigned char *out;
	unsigned char value;


	out = output;

	for (;;)
	{
		count = (signed char)*input++;
		if (count > 0)
		{
			/* replicate run */
			value = *input++;
			while (count--)
				*out++ = value;
		}
		else if (count < 0)
		{
			/* literal run */
			while (count++)
				*out++ = *input++;
		}
		else
			/* EOF */
			break;
	}

	return (out - output);
}
