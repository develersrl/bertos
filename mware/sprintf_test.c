#include "sprintf.c"
#include "formatwr.c"
#include "hex.c"

#include <assert.h> /* assert() */
#include <string.h> /* strcmp() */


int main(UNUSED_ARG(int, argc), UNUSED_ARG(char **,argv))
{
	char buf[256];
	static const char test_string[] = "Hello, world!\n";
	static const pgm_char test_string_pgm[] = "Hello, world!\n";

	snprintf(buf, sizeof buf, "%s", test_string);
	assert(strcmp(buf, test_string) == 0);

	snprintf(buf, sizeof buf, "%S", test_string_pgm);
	assert(strcmp(buf, test_string_pgm) == 0);

	snprintf(buf, sizeof buf, "%d", 12345);
	assert(strcmp(buf, "12345") == 0);

	snprintf(buf, sizeof buf, "%ld", 123456789L);
	assert(strcmp(buf, "123456789") == 0);

	snprintf(buf, sizeof buf, "%lu", 4294967295UL);
	assert(strcmp(buf, "4294967295") == 0);

	/*
	 * Stress tests.
	 */
	snprintf(buf, sizeof buf, "%s", NULL);
	assert(strcmp(buf, "<NULL>") == 0);
	snprintf(buf, sizeof buf, "%k");
	assert(strcmp(buf, "???") == 0);
	sprintf(NULL, test_string); /* must not crash */

	return 0;
}
