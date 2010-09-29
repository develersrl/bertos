#ifndef SEC_CIPHER_AES_H
#define SEC_CIPHER_AES_H

#include <sec/cipher.h>
#include <alloca.h>

typedef struct
{
	BlockCipher c;
	int num_rounds;
	uint8_t expkey[44*4];
} AES128_Context;

typedef struct
{
	BlockCipher c;
	int num_rounds;
	uint8_t expkey[52*4];
} AES192_Context;

typedef struct
{
	BlockCipher c;
	int num_rounds;
	uint8_t expkey[60*4];
} AES256_Context;

void AES128_init(AES128_Context *c);
void AES192_init(AES192_Context *c);
void AES256_init(AES256_Context *c);

#define AES128_stackinit(...) \
	({ AES128_Context *ctx = alloca(sizeof(AES128_Context)); AES128_init(ctx, ##__VA_ARGS__); &ctx->c; })

#define AES192_stackinit(...) \
	({ AES192_Context *ctx = alloca(sizeof(AES192_Context)); AES192_init(ctx, ##__VA_ARGS__); &ctx->c; })

#define AES256_stackinit(...) \
	({ AES256_Context *ctx = alloca(sizeof(AES256_Context)); AES256_init(ctx, ##__VA_ARGS__); &ctx->c; })

int AES_testSetup(void);
int AES_testRun(void);
int AES_testTearDown(void);

#endif /* SEC_CIPHER_AES_H */
