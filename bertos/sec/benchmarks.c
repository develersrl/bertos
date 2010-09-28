#include "benchmarks.h"
#include <sec/hash.h>
#include <drv/timer.h>
#include <string.h>

static uint8_t buf[512];

void hash_benchmark(Hash *h, const char *hname, int numk)
{
	memset(buf, 0x12, sizeof(buf));
	ticks_t t = timer_clock();

	for (int j=0;j<64;++j) {
		hash_begin(h);
		for (int i=0; i<numk*2; ++i)
			hash_update(h, buf, 512);
		hash_final(h);
	}

	t = timer_clock() - t;

	utime_t usec = ticks_to_us(t) / 64;
	kprintf("%s @ %ldMhz: %s of %dKiB of data: %lu.%lu ms\n", CPU_CORE_NAME, CPU_FREQ/1000000, hname, numk, (usec/1000), (usec % 1000));
}

void prng_benchmark(PRNG *prng, const char *hname, int numbytes)
{
	memset(buf, 0x12, sizeof(buf));

	ASSERT(sizeof(buf) >= prng_seed_len(prng));
	prng_reseed(prng, buf);

	ticks_t t = timer_clock();

	for (int j=0;j<2048;++j) {
		for (int i=0; i<(numbytes+511)/512-1; ++i)
			prng_generate(prng, buf, 512);
		prng_generate(prng, buf, numbytes%512);
	}

	t = timer_clock() - t;

	utime_t usec = ticks_to_us(t) / 2048;
	kprintf("%s @ %ldMhz: %s generation of %d random bytes: %lu.%lu ms\n", CPU_CORE_NAME, CPU_FREQ/1000000, hname, numbytes, (usec/1000), (usec % 1000));
	kprintf("Sample of random data:\n");
	kdump(buf, MIN(numbytes, 64));
}
