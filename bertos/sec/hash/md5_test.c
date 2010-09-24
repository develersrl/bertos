
#include <cfg/test.h>
#include <cfg/debug.h>

#include "md5.h"
#include <string.h>

int MD5_testSetup(void)
{
	kdbg_init();
	return 0;
}

int MD5_testTearDown(void)
{
	return 0;
}

int MD5_testRun(void)
{
	int i;
	MD5_Context context;
	MD5_init(&context);

	hash_begin(&context.h);
	hash_update(&context.h, "abc", 3);
	ASSERT(memcmp(hash_final(&context.h), "\x90\x01\x50\x98\x3C\xD2\x4F\xB0\xD6\x96\x3F\x7D\x28\xE1\x7F\x72", 16) == 0);

	hash_begin(&context.h);
	hash_update(&context.h, "aaa", 3);
	ASSERT(memcmp(hash_final(&context.h), "\x47\xBC\xE5\xC7\x4F\x58\x9F\x48\x67\xDB\xD5\x7E\x9C\xA9\xF8\x08", 16) == 0);

	hash_begin(&context.h);
	hash_update(&context.h, "abcdefghijklmnopqrstuvwxyz", 26);
	ASSERT(memcmp(hash_final(&context.h), "\xC3\xFC\xD3\xD7\x61\x92\xE4\x00\x7D\xFB\x49\x6C\xCA\x67\xE1\x3B", 16) == 0);

	hash_begin(&context.h);
	hash_update(&context.h, "0123456789", 10);
	ASSERT(memcmp(hash_final(&context.h), "\x78\x1E\x5E\x24\x5D\x69\xB5\x66\x97\x9B\x86\xE2\x8D\x23\xF2\xC7", 16) == 0);

	hash_begin(&context.h);
	for (i = 0; i < 1000; i++)
		hash_update(&context.h, "a", 1);
	ASSERT(memcmp(hash_final(&context.h), "\xCA\xBE\x45\xDC\xC9\xAE\x5B\x66\xBA\x86\x60\x0C\xCA\x6B\x8B\xA8", 16) == 0);

	hash_begin(&context.h);
	for (i = 0; i < 1000000; i++)
		hash_update(&context.h, "a", 1);
	ASSERT(memcmp(hash_final(&context.h), "\x77\x07\xd6\xae\x4e\x02\x7c\x70\xee\xa2\xa9\x35\xc2\x29\x6f\x21", 16) == 0);
	
	return 0;
}

TEST_MAIN(MD5);
