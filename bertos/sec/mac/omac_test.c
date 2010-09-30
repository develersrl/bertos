
#include <sec/mac/omac.h>
#include <sec/cipher/aes.h>
#include <cfg/test.h>
#include <cfg/debug.h>
#include <string.h>

int omac_testSetup(void)
{
	kdbg_init();
	return 0;
}

int omac_testTearDown(void)
{
	return 0;
}

struct OmacTest
{
	void *key;
	size_t klen;
	uint8_t *msg;
	size_t mlen;
	uint8_t digest[16*2];
};

static const struct OmacTest tests1_aes128[] =
{
	{
		"2b7e151628aed2a6abf7158809cf4f3c", 16,
		"", 0,
		"bb1d6929e95937287fa37d129b756746",
	},
	{
		"2b7e151628aed2a6abf7158809cf4f3c", 16,
		"6bc1bee22e409f96e93d7e117393172a", 16,
		"070a16b46b4d4144f79bdd9dd04a287c",
	},
	{
		"2b7e151628aed2a6abf7158809cf4f3c", 16,
		"6bc1bee22e409f96e93d7e117393172a"
		"ae2d8a571e03ac9c9eb76fac45af8e51"
		"30c81c46a35ce411e5fbc1191a0a52ef"
		"f69f2445df4f9b17ad2b417be66c3710", 64,
		"51f0bebf7e3b9d92fc49741779363cfe",
	},
};

static const struct OmacTest tests1_aes192[] =
{
	{
		"8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", 24,
		"", 0,
		"d17ddf46adaacde531cac483de7a9367",
	},
	{
		"8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", 24,
		"6bc1bee22e409f96e93d7e117393172a", 16,
		"9e99a7bf31e710900662f65e617c5184",
	},
	{
		"8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", 24,
		"6bc1bee22e409f96e93d7e117393172a"
		"ae2d8a571e03ac9c9eb76fac45af8e51"
		"30c81c46a35ce411", 40,
		"8a1de5be2eb31aad089a82e6ee908b0e",
	},
	{
		"8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", 24,
		"6bc1bee22e409f96e93d7e117393172a"
		"ae2d8a571e03ac9c9eb76fac45af8e51"
		"30c81c46a35ce411e5fbc1191a0a52ef"
		"f69f2445df4f9b17ad2b417be66c3710", 64,
		"a1d5df0eed790f794d77589659f39a11",
	},
};

static const struct OmacTest tests1_aes256[] =
{
	{
		"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", 32,
		"", 0,
		"028962f61b7bf89efc6b551f4667d983",
	},
	{
		"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", 32,
		"6bc1bee22e409f96e93d7e117393172a", 16,
		"28a7023f452e8f82bd4bf28d8c37c35c",
	},
	{
		"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", 32,
		"6bc1bee22e409f96e93d7e117393172a"
		"ae2d8a571e03ac9c9eb76fac45af8e51"
		"30c81c46a35ce411", 40,
		"aaf3d8f1de5640c232f5b169b9c911e6",
	},
	{
		"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", 32,
		"6bc1bee22e409f96e93d7e117393172a"
		"ae2d8a571e03ac9c9eb76fac45af8e51"
		"30c81c46a35ce411e5fbc1191a0a52ef"
		"f69f2445df4f9b17ad2b417be66c3710", 64,
		"e1992190549f6ed5696a2c056c315410",
	},
};

static const struct OmacTest tests2_aes128[] =
{
	{
		"2b7e151628aed2a6abf7158809cf4f3c", 16,
		"", 0,
		"f6bc6a41f4f84593809e59b719299cfe",
	},
	{
		"2b7e151628aed2a6abf7158809cf4f3c", 16,
		"6bc1bee22e409f96e93d7e117393172a", 16,
		"070a16b46b4d4144f79bdd9dd04a287c",
	},
	{
		"2b7e151628aed2a6abf7158809cf4f3c", 16,
		"6bc1bee22e409f96e93d7e117393172a"
		"ae2d8a571e03ac9c9eb76fac45af8e51"
		"30c81c46a35ce411", 40,
		"23fdaa0831cd314491ce4b25acb6023b",
	},
	{
		"2b7e151628aed2a6abf7158809cf4f3c", 16,
		"6bc1bee22e409f96e93d7e117393172a"
		"ae2d8a571e03ac9c9eb76fac45af8e51"
		"30c81c46a35ce411e5fbc1191a0a52ef"
		"f69f2445df4f9b17ad2b417be66c3710", 64,
		"51f0bebf7e3b9d92fc49741779363cfe",
	},
};

static const struct OmacTest tests2_aes192[] =
{
	{
		"8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", 24,
		"", 0,
		"149f579df2129d45a69266898f55aeb2",
	},
	{
		"8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", 24,
		"6bc1bee22e409f96e93d7e117393172a", 16,
		"9e99a7bf31e710900662f65e617c5184",
	},
	{
		"8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", 24,
		"6bc1bee22e409f96e93d7e117393172a"
		"ae2d8a571e03ac9c9eb76fac45af8e51"
		"30c81c46a35ce411", 40,
		"b35e2d1b73aed49b78bdbdfe61f646df",
	},
	{
		"8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", 24,
		"6bc1bee22e409f96e93d7e117393172a"
		"ae2d8a571e03ac9c9eb76fac45af8e51"
		"30c81c46a35ce411e5fbc1191a0a52ef"
		"f69f2445df4f9b17ad2b417be66c3710", 64,
		"a1d5df0eed790f794d77589659f39a11",
	},
};

static const struct OmacTest tests2_aes256[] =
{
	{
		"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", 32,
		"", 0,
		"47fbde71866eae6080355b5fc7ff704c",
	},
	{
		"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", 32,
		"6bc1bee22e409f96e93d7e117393172a", 16,
		"28a7023f452e8f82bd4bf28d8c37c35c",
	},
	{
		"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", 32,
		"6bc1bee22e409f96e93d7e117393172a"
		"ae2d8a571e03ac9c9eb76fac45af8e51"
		"30c81c46a35ce411", 40,
		"f018e6053611b34bc872d6b7ff24749f",
	},
	{
		"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", 32,
		"6bc1bee22e409f96e93d7e117393172a"
		"ae2d8a571e03ac9c9eb76fac45af8e51"
		"30c81c46a35ce411e5fbc1191a0a52ef"
		"f69f2445df4f9b17ad2b417be66c3710", 64,
		"e1992190549f6ed5696a2c056c315410",
	},
};

static void hexunlify(uint8_t *out, const uint8_t *in, size_t len)
{
	#define TO_DEC(x) ((x >= '0' && x <= '9') ? x-'0' : \
					   (x >= 'a' && x <= 'f') ? x-'a'+10 : \
					   (x >= 'A' && x <= 'F') ? x-'A'+10 : 0)
	while (len--)
	{
		*out++ = TO_DEC(in[0])*16 + TO_DEC(in[1]);
		in += 2;
	}
}

static void runTest(Mac *m, const struct OmacTest *t, size_t num)
{
	for (size_t i=0; i<num; ++i, ++t)
	{
		uint8_t key[t->klen];
		hexunlify(key, t->key, t->klen);

		uint8_t msg[t->mlen];
		hexunlify(msg, t->msg, t->mlen);

		uint8_t digest[16];
		hexunlify(digest, t->digest, 16);

		mac_set_key(m, key, t->klen);
		mac_begin(m);
		mac_update(m, msg, t->mlen);
		uint8_t *result = mac_final(m);

		ASSERT(memcmp(result, digest, 16) == 0);
	}
}

int omac_testRun(void)
{
	if (1)
	{
		Mac *m = omac1_stackinit(AES128_stackinit());
		runTest(m, tests1_aes128, countof(tests1_aes128));
	}

	if (1)
	{
		Mac *m = omac1_stackinit(AES192_stackinit());
		runTest(m, tests1_aes192, countof(tests1_aes192));
	}

	if (1)
	{
		Mac *m = omac1_stackinit(AES256_stackinit());
		runTest(m, tests1_aes256, countof(tests1_aes256));
	}

	if (1)
	{
		Mac *m = omac2_stackinit(AES128_stackinit());
		runTest(m, tests2_aes128, countof(tests2_aes128));
	}

	if (1)
	{
		Mac *m = omac2_stackinit(AES192_stackinit());
		runTest(m, tests2_aes192, countof(tests2_aes192));
	}

	if (1)
	{
		Mac *m = omac2_stackinit(AES256_stackinit());
		runTest(m, tests2_aes256, countof(tests2_aes256));
	}

	return 0;
}

TEST_MAIN(omac);
