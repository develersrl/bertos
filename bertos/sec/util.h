#ifndef SEC_UTIL_H
#define SEC_UTIL_H

#include <cfg/macros.h>
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

INLINE void xor_block_8(uint8_t *out,
		const uint8_t *in1, const uint8_t *in2, size_t len)
{
	while (len--)
                *out++ = *in1++ ^ *in2++;
}

INLINE void xor_block_const_8(uint8_t *out,
			const uint8_t *in, uint8_t k, size_t len)
{
	while (len--)
                *out++ = *in++ ^ k;
}

INLINE void xor_block_32(uint32_t *out, const uint32_t *in1,
				const uint32_t *in2, size_t len)
{
	size_t rem = (len & (sizeof(uint32_t) - 1));

	len /= sizeof(uint32_t);
	while (len--)
		*out++ = *in1++ ^ *in2++;
	xor_block_8((uint8_t *)out,
		(const uint8_t *)in1, (const uint8_t *)in2, rem);
}

INLINE void xor_block_const_32(uint32_t *out, const uint32_t *in,
					uint8_t k, size_t len)
{
	uint32_t k32 = k | ((uint32_t)k << 8) |
			((uint32_t)k << 16) | ((uint32_t)k << 24);
	size_t rem = (len & (sizeof(uint32_t) - 1));

	len /= sizeof(uint32_t);
	while (len--)
		*out++ = *in++ ^ k32;
	xor_block_const_8((uint8_t *)out, (const uint8_t *)in, k, rem);
}

/**
 * Perform a bitwise xor between \a in and \a inout, and store
 * the result into \a inout.
 */
INLINE void xor_block(void *out, const void *in1, const void *in2, size_t len)
{
	if (is_aligned(out, sizeof(uint32_t)) &&
			is_aligned(in1, sizeof(uint32_t)) &&
			is_aligned(in2, sizeof(uint32_t)))
	{
		uint32_t *obuf = (uint32_t *)((size_t)out);
		const uint32_t *ibuf1 = (const uint32_t *)((size_t)in1);
		const uint32_t *ibuf2 = (const uint32_t *)((size_t)in2);

		xor_block_32(obuf, ibuf1, ibuf2, len);
	}
	else
	{
		uint8_t *obuf = (uint8_t *)((size_t)out);
		const uint8_t *ibuf1 = (const uint8_t *)((size_t)in1);
		const uint8_t *ibuf2 = (const uint8_t *)((size_t)in2);

		xor_block_8(obuf, ibuf1, ibuf2, len);
	}
}

/**
 * Perform a bitwise xor over \a inout with constant \a k.
 */
INLINE void xor_block_const(uint8_t *out, const uint8_t *in, uint8_t k, size_t len)
{
	if (is_aligned(out, sizeof(uint32_t)) &&
			is_aligned(in, sizeof(uint32_t)))
	{
		uint32_t *obuf = (uint32_t *)((size_t)out);
		const uint32_t *ibuf = (const uint32_t *)((size_t)in);

		xor_block_const_32(obuf, ibuf, k, len);
	}
	else
	{
		uint8_t *obuf = (uint8_t *)((size_t)out);
		const uint8_t *ibuf = (const uint8_t *)((size_t)in);

		xor_block_const_8(obuf, ibuf, k, len);
	}
}

#endif /* SEC_UTIL_H */
