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
 *
 * notest: avr
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

static const char uri0[] = "12345";
static const char uri_check0[] = "1234";

static const char uri1[] = "!*'();:@&=%2B%24%2C/?#%5B%5D%3C%3E%7E.%22%7B%7D%7C%5C-%60_%5E%25";
static const char uri_check1[] = "!*'();:@&=+$,/?#[]<>~.\"{}|\\-`_^%";

static const char uri2[] = "test+test1+test2";
static const char uri_check2[] = "test test1 test2";

static char token_str[] = "var1=1&var2=2&var3=3&var4=4";
static char token_str1[] = "var1=1&var2=2&=3&var4=";
static char token_str2[] = "var1=test+test&var2=2&var3=test%5B%5D!@;'%22%5C.%20&var4=4";

static struct {const char *content;} contents[] =
{
	{"one/two/three/test"},
	{"one/two/three.htm"},
	{"one/test.css"},
	{"one/two/test.js"},
	{"one/two/test.png"},
	{"one/two/test.ico"},
	{"one/two/test.jpg"},
	{"one/two/test.gif"},
};


static int contents_check_type[] =
{
	HTTP_CONTENT_JSON,
	HTTP_CONTENT_HTML,
	HTTP_CONTENT_CSS,
	HTTP_CONTENT_JS,
	HTTP_CONTENT_PNG,
	HTTP_CONTENT_JPEG,
	HTTP_CONTENT_JPEG,
	HTTP_CONTENT_GIF,
};

#define CONTENT_TEST_CNT  8


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


	char decoded0[5];
	http_decodeUrl(uri0, 6, decoded0, 5);

	if (strcmp(decoded0, uri_check0))
	{
		kprintf("error 04 %s\n", decoded0);
		goto error;
	}

	char decoded[sizeof(uri)];
	http_decodeUrl(uri,sizeof(uri), decoded, sizeof(uri));

	if (strcmp(decoded, uri_check))
	{
		kprintf("error 4 %s\n", decoded);
		goto error;
	}

	char decoded1[sizeof(uri1)];
	http_decodeUrl(uri1,sizeof(uri1), decoded1, sizeof(uri1));

	if (strcmp(decoded1, uri_check1))
	{
		kprintf("error 5 %s\n", decoded1);
		goto error;
	}

	char decoded2[sizeof(uri2)];
	http_decodeUrl(uri2,sizeof(uri2), decoded2, sizeof(uri2));

	if (strcmp(decoded2, uri_check2))
	{
		kprintf("error 5 %s\n", decoded2);
		goto error;
	}

	int len = http_tokenizeGetRequest(token_str, sizeof(token_str));
	if (len != 4)
	{
		kprintf("error 6 len %d expect %d\n", len, 4);
		goto error;
	}

	char value[80];
	http_getValue(token_str, sizeof(token_str), "var1", value, sizeof(value));
	if (strcmp(value, "1"))
	{
		kprintf("error 6 value %s expect %s\n", value, "1");
		goto error;

	}

	http_getValue(token_str, sizeof(token_str), "var4", value, sizeof(value));
	if (strcmp(value, "4"))
	{
		kprintf("error 6 value %s expect %s\n", value, "4");
		goto error;

	}


	len = http_tokenizeGetRequest(token_str1, sizeof(token_str1));
	if (len != 4)
	{
		kprintf("error 7 len %d expect %d\n", len, 4);
		goto error;
	}

	if (http_getValue(token_str1, sizeof(token_str1), "var1", value, sizeof(value)) < 0)
	{
		kprintf("error 7 during get key %s\n", "var1");
		goto error;
	}
	if (strcmp(value, "1"))
	{
		kprintf("error 7 value %s expect %s\n", value, "1");
		goto error;

	}

	if (http_getValue(token_str1, sizeof(token_str1), "var4", value, sizeof(value)) < 0)
	{
		kprintf("error 7 during get key %s\n", "var4");
		goto error;
	}

	if (strcmp(value, ""))
	{
		kprintf("error 7 value %s expect %s\n", value, "");
		goto error;

	}

	len = http_tokenizeGetRequest(token_str2, sizeof(token_str2));
	if (len != 4)
	{
		kprintf("error 8 len %d expect %d\n", len, 4);
		goto error;
	}

	if (http_getValue(token_str2, sizeof(token_str2), "var3", value, sizeof(value)) < 0)
	{
		kprintf("error 8 during get key %s\n", "var3");
		goto error;
	}

	if (strcmp(value, "test[]!@;'\"\\. "))
	{
		kprintf("error 8 value %s expect %s\n", value, "test[]!@;'\"\\. ");
		goto error;

	}

	if (http_getValue(token_str2, sizeof(token_str2), "var1", value, sizeof(value)) < 0)
	{
		kprintf("error 7 during get key %s\n", "var1");
		goto error;
	}
	if (strcmp(value, "test test"))
	{
		kprintf("error 7 value %s expect %s\n", value, "test test");
		goto error;

	}


	for (int i = 0; i < CONTENT_TEST_CNT; i++)
	{
		int type = http_searchContentType(contents[i].content);
		if (type != contents_check_type[i])
		{
			kprintf("error 8-%d return type %d expect %d\n", i, type, contents_check_type[i]);
			kprintf("error 8-%d ext %s\n", i, contents[i].content);
			goto error;
		}
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
