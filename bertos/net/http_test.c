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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief HTTP Server test
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#include <cfg/compiler.h>
#include <cfg/test.h>
#include <cfg/debug.h>

#include <net/http.h>

#include <string.h>

static const char get_str[] = "\
GET /test/page1 HTTP/1.1 Host: 10.3.3.199 Connection: keep-alive \
User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/535.1 \
(KHTML, like Gecko) Chrome/14.0.835.186 Safari/535.1 \
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8 \
Accept-Encoding: gzip,deflate,sdch Accept-Language: it-IT,it;q=0.8,en-US;\
q=0.6,en;q=0.4 Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3";


static const char get_str1[] = "\
GET /test/page1";

static const char get_str2[] = "\
GET ";

static const char get_str3[] = "\
GAT /test/page1 HTTP/1.1 Host: 10.3.3.199 Connection: keep-alive \
User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/535.1 \
(KHTML, like Gecko) Chrome/14.0.835.186 Safari/535.1 \
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8 \
Accept-Encoding: gzip,deflate,sdch Accept-Language: it-IT,it;q=0.8,en-US;\
q=0.6,en;q=0.4 Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3";


static const char uri[] = "test%5B%5D!@;'%22%5C.%20";
static const char uri_check[] = "test[]!@;'\"\\. ";

static const char uri1[] = "!*'();:@&=%2B%24%2C/?#%5B%5D%3C%3E%7E.%22%7B%7D%7C%5C-%60_%5E%25";
static const char uri_check1[] = "!*'();:@&=+$,/?#[]<>~.\"{}|\\-`_^%";


int http_testSetup(void)
{
	kdbg_init();
	return 0;
}

int http_testRun(void)
{
	char name[80];
	memset(name, 0, sizeof(name));
	http_getPageName(get_str, sizeof(get_str), name, sizeof(name));

	if (strcmp("test/page1", name))
	{
		kprintf("error 0 %s\n", name);
		goto error;
	}

	http_getPageName(get_str1, sizeof(get_str1), name, sizeof(name));

	if (strcmp("test/page1", name))
	{
		kprintf("error 1 %s\n", name);
		goto error;
	}


	http_getPageName(get_str2, sizeof(get_str2), name, sizeof(name));

	if (name[0] != '\0')
	{
		kprintf("error 2 %s\n", name);
		goto error;
	}


	http_getPageName(get_str2, sizeof(get_str2), name, sizeof(name));

	if (name[0] != '\0')
	{
		kprintf("error 3 %s\n", name);
		goto error;
	}


	char decoded[sizeof(uri)];
	http_decodeUri(uri,sizeof(uri), decoded, sizeof(uri));

	if (strcmp(decoded, uri_check))
	{
		kprintf("error 4 %s\n", decoded);
		goto error;
	}

	char decoded1[sizeof(uri1)];
	http_decodeUri(uri1,sizeof(uri1), decoded1, sizeof(uri1));

	if (strcmp(decoded1, uri_check1))
	{
		kprintf("error 5 %s\n", decoded1);
		goto error;
	}

	return 0;

error:
	kprintf("Error!\n");
	return -1;
}

int http_testTearDown(void)
{
	return 0;
}

TEST_MAIN(http);
