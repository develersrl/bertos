#if defined(__i386__)
	#include "switch_i386.S"
#elif defined(__x86_64__)
	#include "switch_x86_64.s"
#else
	#error Unknown CPU
#endif

