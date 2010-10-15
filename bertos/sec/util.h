#ifndef SEC_UTIL_H
#define SEC_UTIL_H

#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <string.h>

/**
 * Purge local variables, by zeroing them.
 *
 * This can be used to clear stack from intermediate results in crypto
 * calculations that might somehow be leaked.
 */
#define PURGE(x) \
	memset(&x, 0, sizeof(x))

/**
 * Convert a generic "password" (low-diffused) to a generic "key"
 * (high-diffused).
 *
 * In common terminology, a "password" is a key with weak cryptographic
 * characteristics, such as commonly used password input by an user,
 * which are usually short and use only a few different characters from
 * the 0-255 byte range.
 *
 * This function derives a strong key from the password using a one-way
 * process.
 *
 * \note Uses PBKDF2 as key-derivation function, with a fixed salt that
 * changes for each Bertos project.
 */
void password2key(const char *pwd, size_t pwd_len,
				  uint8_t *key, size_t key_len);

/**
 * Perform a bitwise xor between \a in and \a inout, and store
 * the result into \a inout.
 */
INLINE void xor_block(uint8_t *out, const uint8_t *in1, const uint8_t* in2, size_t len);

/**
 * Perform a bitwise xor over \a inout with constant \a k.
 */
INLINE void xor_block_const(uint8_t *out, const uint8_t *in, uint8_t k, size_t len);


// FIXME: provide non-32bit fallback
// FIXME: proper ifdef conditional
#if 1 // 32-bit optimized versions

// FIXME: this code is currently buggy because it ignores alignment issues.
INLINE void xor_block(uint8_t *out, const uint8_t *in1, const uint8_t* in2, size_t len)
{
	ASSERT(((size_t)in1 % 4) == 0);
	ASSERT(((size_t)in2 % 4) == 0);
	ASSERT(((size_t)out % 4) == 0);

	const uint32_t *ibuf1 = (const uint32_t *)in1;
	const uint32_t *ibuf2 = (const uint32_t *)in2;
	uint32_t *obuf = (uint32_t *)out;
	size_t rem = (len & 3);

	len /= 4;
	while (len--)
		*obuf++ = *ibuf1++ ^ *ibuf2++;

	in1 = (const uint8_t*)ibuf1;
	in2 = (const uint8_t*)ibuf2;
	out = (uint8_t*)obuf;
	while (rem--)
		*out++ = *in1++ ^ *in2++;
}

INLINE void xor_block_const(uint8_t *out, const uint8_t *in, uint8_t k, size_t len)
{
	ASSERT(((size_t)in % 4) == 0);
	ASSERT(((size_t)out % 4) == 0);

	uint32_t k32 = k | ((uint32_t)k<<8) | ((uint32_t)k<<16) | ((uint32_t)k<<24);
	const uint32_t *ibuf = (const uint32_t *)in;
	uint32_t *obuf = (uint32_t *)out;
	size_t rem = (len & 3);

	len /= 4;
	while (len--)
		*obuf++ = *ibuf++ ^ k32;

	in = (const uint8_t*)ibuf;
	out = (uint8_t*)obuf;
	while (rem--)
		*out++ = *in++ ^ k;
}

#endif

#endif /* SEC_UTIL_H */
