
#include "ripemd.h"

#include <cfg/test.h>
#include <cfg/debug.h>

#include <string.h>

int RIPEMD_testSetup(void)
{
	kdbg_init();
	return 0;
}

int RIPEMD_testTearDown(void)
{
	return 0;
}

int RIPEMD_testRun(void)
{
	Hash *h = RIPEMD_stackinit();

	hash_begin(h);
	ASSERT(memcmp(hash_final(h), "\x9c\x11\x85\xa5\xc5\xe9\xfc\x54\x61\x28\x08\x97\x7e\xe8\xf5\x48\xb2\x25\x8d\x31", 20) == 0);

	hash_begin(h);
	hash_update(h, "a", 1);
	ASSERT(memcmp(hash_final(h), "\x0b\xdc\x9d\x2d\x25\x6b\x3e\xe9\xda\xae\x34\x7b\xe6\xf4\xdc\x83\x5a\x46\x7f\xfe", 20) == 0);
	
	hash_begin(h);
	hash_update(h, "abc", 3);
	ASSERT(memcmp(hash_final(h), "\x8e\xb2\x08\xf7\xe0\x5d\x98\x7a\x9b\x04\x4a\x8e\x98\xc6\xb0\x87\xf1\x5a\x0b\xfc", 20) == 0);

	hash_begin(h);
	hash_update(h, "message digest", 14);
	ASSERT(memcmp(hash_final(h), "\x5d\x06\x89\xef\x49\xd2\xfa\xe5\x72\xb8\x81\xb1\x23\xa8\x5f\xfa\x21\x59\x5f\x36", 20) == 0);

	hash_begin(h);
	hash_update(h, "abcdefghijklmnopqrstuvwxyz", 26);
	ASSERT(memcmp(hash_final(h), "\xf7\x1c\x27\x10\x9c\x69\x2c\x1b\x56\xbb\xdc\xeb\x5b\x9d\x28\x65\xb3\x70\x8d\xbc", 20) == 0);

	hash_begin(h);
	hash_update(h, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);
	ASSERT(memcmp(hash_final(h), "\x12\xa0\x53\x38\x4a\x9c\x0c\x88\xe4\x05\xa0\x6c\x27\xdc\xf4\x9a\xda\x62\xeb\x2b", 20) == 0);

	hash_begin(h);
	hash_update(h, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 62);
	ASSERT(memcmp(hash_final(h), "\xb0\xe2\x0b\x6e\x31\x16\x64\x02\x86\xed\x3a\x87\xa5\x71\x30\x79\xb2\x1f\x51\x89", 20) == 0);

	hash_begin(h);
	for (int i=0;i<8;++i)
		hash_update(h, "1234567890", 10);
	ASSERT(memcmp(hash_final(h), "\x9b\x75\x2e\x45\x57\x3d\x4b\x39\xf4\xdb\xd3\x32\x3c\xab\x82\xbf\x63\x32\x6b\xfb", 20) == 0);

	hash_begin(h);
	for (int i=0;i<1000000;++i)
		hash_update(h, "a", 1);
	ASSERT(memcmp(hash_final(h), "\x52\x78\x32\x43\xc1\x69\x7b\xdb\xe1\x6d\x37\xf9\x7f\x68\xf0\x83\x25\xdc\x15\x28", 20) == 0);

	return 0;
}

TEST_MAIN(RIPEMD);
