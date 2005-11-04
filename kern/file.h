/*!
 * \file
 * <!--
 * Copyright 2003,2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999,2001,2003 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Interface to KFile virtual class
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.6  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.5  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.4  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.3  2004/06/06 15:11:38  bernie
 *#* Doxygenize.
 *#*
 *#* Revision 1.2  2004/06/03 11:27:09  bernie
 *#* Add dual-license information.
 *#*
 *#* Revision 1.1  2004/05/23 17:27:00  bernie
 *#* Import kern/ subdirectory.
 *#*/
#ifndef KERN_KFILE_H
#define KERN_KFILE_H

#ifndef COMPILER_H
#include <cfg/compiler.h>
#endif

struct _KFile;

typedef size_t	(*ReadFunc_t)	(struct _KFile *fd, char *buf, size_t size);
typedef size_t	(*WriteFunc_t)	(struct _KFile *fd, const char *buf, size_t size);
typedef	bool	(*SeekFunc_t)	(struct _KFile *fd, int32_t offset);
typedef bool	(*CloseFunc_t)	(struct _KFile *fd);
typedef bool	(*OpenFunc_t)	(struct _KFile *fd, const char *name, int mode);


/*!
 * Context data for callback functions which operate on
 * pseudo files.
 */
typedef struct _KFile
{
	ReadFunc_t		Read;
	WriteFunc_t		Write;
	SeekFunc_t		Seek;
/*	OpenFunc_t		Open;	unused */
	CloseFunc_t		Close;

	/* NOTE: these must _NOT_ be size_t on 16bit CPUs! */
	uint32_t		SeekPos;
	uint32_t		Size;
} KFile;


#endif /* KERN_KFILE_H */
