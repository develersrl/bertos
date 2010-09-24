#ifndef SEC_HASH_RIPEMD_H
#define SEC_HASH_RIPEMD_H

#include <cfg/compiler.h>
#include <sec/hash.h>
#include <alloca.h>

typedef struct {
	Hash hash;
    uint32_t h[5];      /* The current hash state */
    uint64_t length;    /* Total number of _bits_ (not bytes) added to the
                           hash.  This includes bits that have been buffered
                           but not not fed through the compression function yet. */
    union {
        uint32_t w[16];
        uint8_t b[64];
    } buf;
    uint8_t bufpos;     /* number of bytes currently in the buffer */

} RIPEMD_Context;

void RIPEMD_init(RIPEMD_Context *ctx);

#define RIPEMD_stackinit(...) \
	({ RIPEMD_Context *ctx = alloca(sizeof(RIPEMD_Context)); RIPEMD_init(ctx , ##__VA_ARGS__); &ctx->hash; })

int RIPEMD_testSetup(void);
int RIPEMD_testRun(void);
int RIPEMD_testTearDown(void);


#endif /* SEC_HASH_RIPEMD_H */