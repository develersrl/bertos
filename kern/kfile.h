/**
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001, 2003 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Virtual KFile I/O interface.
 *
 * \version $Id$
 * \author Bernardo Innocenti <bernie@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.2  2007/10/03 12:06:54  batt
 *#* Add whence parameter to seek; change from char * to void * read/write input buffers.
 *#*
 *#* Revision 1.1  2007/06/14 14:42:48  batt
 *#* Move kfile to kern/ directory; remove duplicate file.h.
 *#*
 *#* Revision 1.7  2007/06/08 14:25:43  batt
 *#* Merge from project_ks.
 *#*
 *#* Revision 1.6  2006/07/19 12:56:28  bernie
 *#* Convert to new Doxygen style.
 *#*
 *#* Revision 1.5  2005/11/04 16:20:02  bernie
 *#* Fix reference to README.devlib in header.
 *#*
 *#* Revision 1.4  2005/04/11 19:10:28  bernie
 *#* Include top-level headers from cfg/ subdir.
 *#*
 *#* Revision 1.3  2004/12/31 16:43:23  bernie
 *#* Move seek function last in VT.
 *#*
 *#* Revision 1.2  2004/08/25 14:12:09  rasky
 *#* Aggiornato il comment block dei log RCS
 *#*
 *#* Revision 1.1  2004/08/04 02:40:25  bernie
 *#* Add virtual file I/O interface.
 *#*
 *#*/
#ifndef MWARE_KFILE_H
#define MWARE_KFILE_H

#include <cfg/compiler.h>

/* fwd decl */
struct _KFile;

typedef size_t	(*ReadFunc_t)  (struct _KFile *fd, void *buf, size_t size);
typedef size_t	(*WriteFunc_t) (struct _KFile *fd, const void *buf, size_t size);
typedef	int32_t (*SeekFunc_t)  (struct _KFile *fd, int32_t offset, int whence);
typedef bool	(*OpenFunc_t)  (struct _KFile *fd, const char *name, int mode);
typedef bool	(*CloseFunc_t) (struct _KFile *fd);


/* Context data for callback functions which operate on
 * pseudo files.
 */
typedef struct _KFile
{
	ReadFunc_t		read;
	WriteFunc_t		write;
	OpenFunc_t		open;
	CloseFunc_t		close;
	SeekFunc_t		seek;

	/* NOTE: these must _NOT_ be size_t on 16bit CPUs! */
	uint32_t		seek_pos;
	uint32_t		size;
} KFile;

#endif /* MWARE_KFILE_H */
