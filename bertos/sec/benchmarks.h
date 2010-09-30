#ifndef SEC_BENCHMARKS_H
#define SEC_BENCHMARKS_H

#include <sec/hash.h>
#include <sec/prng.h>
#include <sec/cipher.h>

void hash_benchmark(Hash *h, const char *hname, int numk);
void prng_benchmark(PRNG *prng, const char *hname, int numk);
void cipher_benchmark(BlockCipher *c, const char *cname, int msg_len);

#endif /* SEC_BENCHMARKS_H */
