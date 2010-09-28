#ifndef SEC_BENCHMARKS_H
#define SEC_BENCHMARKS_H

#include <sec/hash.h>
#include <sec/prng.h>

void hash_benchmark(Hash *h, const char *hname, int numk);
void prng_benchmark(PRNG *prng, const char *hname, int numk);

#endif /* SEC_BENCHMARKS_H */
