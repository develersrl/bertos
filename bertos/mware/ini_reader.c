/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief Ini file reader module.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "ini_reader.h"
#include "cfg/cfg_ini_reader.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h> //strtol
#include <ctype.h>

static bool lineEmpty(const char *line)
{
	while (*line)
	{
		if (!isspace((unsigned char)*line++))
			return false;
	}
	return true;
}

/*
 * Returns when the line containing the section is found.
 * The file pointer is positioned at the start of the next line or
 * after the last non-empty line if the section is not found.
 * Returns EOF if no section was found, 0 otherwise.
 */
static int findSection(KFile *fd, const char *section, size_t section_len, char *line, size_t size)
{
	kfile_off_t last_full = fd->seek_pos;

	int err;
	do
	{
		char *ptr = line;
		unsigned i;
		err = kfile_gets(fd, line, size);

		/* Remember the last filled line in file */
		if (!lineEmpty(line))
			last_full = fd->seek_pos;

		/* accept only sections that begin at first char */
		if (*ptr++ != '[')
			continue;

		/* find the end-of-section character */
		for (i = 0; i < size && *ptr != ']'; ++i, ++ptr)
			;

		/* The found section could be long that our section key */
		if (section_len != i)
			continue;

		/* did we find the correct section? */
		if(strncmp(&line[1], section, section_len))
			continue;
		else
			return 0;
	}
	while (err != EOF);

	kfile_seek(fd, last_full, KSM_SEEK_SET);
	return EOF;
}

/*
 * Fills the argument with the key found in line
 */
static char *getKey(const char *line, char *key, size_t size)
{
	/* null-terminated string */
	while (isspace((unsigned char)*line))
		++line;
	int i = 0;
	while (*line != '=' && !isspace((unsigned char)*line) && size)
	{
		key[i++] = *line;
		++line;
		--size;
	}
	size ? (key[i] = '\0') : (key[i-1] = '\0');
	return key;
}

/*
 * Fills the argument with the value found in line.
 */
static char *getValue(const char *line, char *value, size_t size)
{
	while (*line++ != '=')
		;
	while (isspace((unsigned char)*line))
		++line;
	int i = 0;
	while (*line && size)
	{
		value[i++] = *line++;
		--size;
	}
	size ? (value[i] = '\0') : (value[i-1] = '\0');
	return value;
}

/**
 * Look for key inside a section.
 *
 * The function reads lines from input file. It fills the line parameter to allow splitting
 * the key-value couple. It returns with error if a new section begins and no key was found.
 * \return 0 if key was found, EOF on errors.
 */
static int findKey(KFile *fd, const char *key, char *line, size_t size)
{
	int err;
	char curr_key[30];
	kfile_off_t last_full = fd->seek_pos;
	kfile_off_t key_pos = fd->seek_pos;

	do
	{
		err = kfile_gets(fd, line, size);

		getKey(line, curr_key, 30);
		/* check key */
		if (!strcmp(curr_key, key))
		{
			kfile_seek(fd, key_pos, KSM_SEEK_SET);
			return 0;
		}

		/* Remember the last filled line in the section */
		if (!lineEmpty(line) && *line != '[')
			last_full = fd->seek_pos;
		key_pos = fd->seek_pos;
	}
	while (err != EOF && *line != '[');
	kfile_seek(fd, last_full, KSM_SEEK_SET);
	return EOF;
}

/*
 * On errors, the function returns EOF and fills the buffer with the default value.
 */
int ini_getString(KFile *fd, const char *section, const char *key, const char *default_value, char *buf, size_t size)
{
	char line[CONFIG_INI_MAX_LINE_LEN];

	if (kfile_seek(fd, 0, KSM_SEEK_SET) == EOF)
	    goto error;

	if (findSection(fd, section, strlen(section), line, CONFIG_INI_MAX_LINE_LEN) == EOF)
		goto error;

	if (findKey(fd, key, line, CONFIG_INI_MAX_LINE_LEN) == EOF)
		goto error;
	else
		getValue(line, buf, size);
	return 0;

error:
	strncpy(buf, default_value, size);
	if (size > 0)
		buf[size - 1] = '\0';
	return EOF;
}

int ini_getInteger(KFile *fd, const char *section, const char *key, long default_value, long *val, int base)
{
	char buf[CONFIG_INI_MAX_LINE_LEN];

	if (ini_getString(fd, section, key, "", buf, sizeof(buf)) == EOF)
		goto error;

	char **endptr = NULL;
	*val = strtol(buf, endptr, base);

	if (buf[0] == 0 || **endptr != 0)
		goto error;

	return 0;

error:
	*val = default_value;
	return EOF;
}

/*
 * Return the position immediatly following the last non-empty line in the file,
 * starting from current position.
 * The file seek position is unchanged at function exit.
 */
static kfile_off_t findLastLine(KFile *fd, char *line, size_t size)
{
	kfile_off_t start = fd->seek_pos;
	kfile_off_t last_full = start;

	int err;
	do
	{
		err = kfile_gets(fd, line, size);
		if (!lineEmpty(line))
			last_full = fd->seek_pos;
	}
	while (err != EOF);
	kfile_seek(fd, start, KSM_SEEK_SET);
	return last_full;
}

int ini_setString(KFile *in, KFile *out, const char *section, const char *key, const char *value)
{
	char line[CONFIG_INI_MAX_LINE_LEN];

	if (kfile_seek(in, 0, KSM_SEEK_SET) == EOF)
	    return EOF;

	if (kfile_seek(out, 0, KSM_SEEK_SET) == EOF)
	    return EOF;

	bool section_found = false;
	bool key_found = false;
	if (findSection(in, section, strlen(section), line, CONFIG_INI_MAX_LINE_LEN) != EOF)
	{
		section_found = true;
		key_found = (findKey(in, key, line, CONFIG_INI_MAX_LINE_LEN) != EOF);
	}

	kfile_off_t len = in->seek_pos;

	/* Copy until key */
	if (kfile_seek(in, 0, KSM_SEEK_SET) == EOF)
			return EOF;
	if (kfile_copy(in, out, len) != len)
			return EOF;

	if (!section_found && value)
	{
		if ((size_t)kfile_printf(out, "\n[%s]\n", section) != (strlen(section) + 4))
			return EOF;
	}
	if (value)
	{
		if ((size_t)kfile_printf(out, "%s=%s\n", key, value) != (strlen(key) + strlen(value) + 2))
			return EOF;
	}

	/* Skip the old line with the key */
	if (key_found)
		kfile_gets(in, line, CONFIG_INI_MAX_LINE_LEN);

	/*
	 * Copy the rest of the input file.
	 * Do not return error if the copied bytes are less than expected
	 * but at least enough to write the last non-empty line.
	 * This is needed for example if the out KFile is of fixed size (like memories).
	 */
	len = in->size - in->seek_pos;
	kfile_off_t bytes_needed = findLastLine(in, line, CONFIG_INI_MAX_LINE_LEN) - in->seek_pos;
	if (kfile_copy(in, out, len) < bytes_needed)
		return EOF;

	/*
	 * Clear the rest of the out file in order to wipe out garbage if the resulting
	 * file is shorter than before.
	 */
	kfile_off_t fill = out->size - out->seek_pos;

	if (fill--)
	{
		if (kfile_putc('\n', out) == EOF)
			return EOF;
		while (fill--)
		{
			if (kfile_putc(' ', out) == EOF)
				return EOF;
		}
	}

	return 0;
}

