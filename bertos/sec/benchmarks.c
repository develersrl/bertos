#include "benchmarks.h"
#include <sec/hash.h>
#include <drv/timer.h>
#include <string.h>

void hash_benchmark(Hash *h, const char *hname, int numk)
{
	static uint8_t buf[512];
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
