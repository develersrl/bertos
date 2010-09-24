
#include <cfg/test.h>
#include <cfg/debug.h>

#include "sha1.h"
#include <string.h>

int SHA1_testSetup(void)
{
	kdbg_init();
	return 0;
}

int SHA1_testTearDown(void)
{
	return 0;
}

int SHA1_testRun(void)
{
	int i;
	SHA1_Context context;
	SHA1_init(&context);

	hash_begin(&context.h);
	hash_update(&context.h, "abc", 3);
	ASSERT(memcmp(hash_final(&context.h), "\xA9\x99\x3E\x36\x47\x06\x81\x6A\xBA\x3E\x25\x71\x78\x50\xC2\x6C\x9C\xD0\xD8\x9D", 20) == 0);

	hash_begin(&context.h);
	hash_update(&context.h, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);
	ASSERT(memcmp(hash_final(&context.h), "\x84\x98\x3E\x44\x1C\x3B\xD2\x6E\xBA\xAE\x4A\xA1\xF9\x51\x29\xE5\xE5\x46\x70\xF1", 20) == 0);

	hash_begin(&context.h);
	for (i = 0; i < 1000000; i++)
		hash_update(&context.h, "a", 1);
	ASSERT(memcmp(hash_final(&context.h), "\x34\xAA\x97\x3C\xD4\xC4\xDA\xA4\xF6\x1E\xEB\x2B\xDB\xAD\x27\x31\x65\x34\x01\x6F", 20) == 0);
	
	return 0;
}

TEST_MAIN(SHA1);
