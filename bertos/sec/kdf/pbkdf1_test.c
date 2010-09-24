#include <string.h>
#include <cpu/detect.h>
#include <cfg/test.h>
#include <cfg/debug.h>
#include <sec/mac/hmac.h>
#include <sec/hash/sha1.h>
#include <sec/kdf/PBKDF1.h>

int PBKDF1_testSetup(void)
{
	kdbg_init();
	return 0;
}

int PBKDF1_testTearDown(void)
{
	return 0;
}

int PBKDF1_testRun(void)
{
	Kdf *kdf = PBKDF1_stackinit(SHA1_stackinit());

	uint8_t res[16];
	
	PBKDF1_set_iterations(kdf, 1000);
	kdf_begin(kdf, "password", 8, (const uint8_t*)"\x78\x57\x8E\x5A\x5D\x63\xCB\x06", 8);
	kdf_read(kdf, res, 16);
	ASSERT(memcmp(res, "\xDC\x19\x84\x7E\x05\xC6\x4D\x2F\xAF\x10\xEB\xFB\x4A\x3D\x2A\x20", 16) == 0);
	
	return 0;
}

TEST_MAIN(PBKDF1);
