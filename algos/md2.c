/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief MD2 Message-Digest algorithm.
 *
 * \version $Id$
 * \author Daniele Basile <asterix@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.4  2007/01/31 13:51:57  asterix
 *#* Write md2_compute function.
 *#*
 *#* Revision 1.2  2007/01/30 17:31:44  asterix
 *#* Add function prototypes.
 *#*
 *#* Revision 1.1  2007/01/30 15:53:26  batt
 *#* Add first md2 skel.
 *#*
 *#*/

#include "md2.h"

#include <string.h> //memset();
#include <cfg/compiler.h>

/*
 * Official array of 256 byte pemutation contructed from digits of pi, defined
 * in the RFC 1319.
 */
static uint8_t md2_perm[256] =
{
  41, 46, 67, 201, 162, 216, 124, 1, 61, 54, 84, 161, 236, 240, 6,
  19, 98, 167, 5, 243, 192, 199, 115, 140, 152, 147, 43, 217, 188,
  76, 130, 202, 30, 155, 87, 60, 253, 212, 224, 22, 103, 66, 111, 24,
  138, 23, 229, 18, 190, 78, 196, 214, 218, 158, 222, 73, 160, 251,
  245, 142, 187, 47, 238, 122, 169, 104, 121, 145, 21, 178, 7, 63,
  148, 194, 16, 137, 11, 34, 95, 33, 128, 127, 93, 154, 90, 144, 50,
  39, 53, 62, 204, 231, 191, 247, 151, 3, 255, 25, 48, 179, 72, 165,
  181, 209, 215, 94, 146, 42, 172, 86, 170, 198, 79, 184, 56, 210,
  150, 164, 125, 182, 118, 252, 107, 226, 156, 116, 4, 241, 69, 157,
  112, 89, 100, 113, 135, 32, 134, 91, 207, 101, 230, 45, 168, 2, 27,
  96, 37, 173, 174, 176, 185, 246, 28, 70, 97, 105, 52, 64, 126, 15,
  85, 71, 163, 35, 221, 81, 175, 58, 195, 92, 249, 206, 186, 197,
  234, 38, 44, 83, 13, 110, 133, 40, 132, 9, 211, 223, 205, 244, 65,
  129, 77, 82, 106, 220, 55, 200, 108, 193, 171, 250, 36, 225, 123,
  8, 12, 189, 177, 74, 120, 136, 149, 139, 227, 99, 232, 109, 233,
  203, 213, 254, 59, 0, 29, 57, 242, 239, 183, 14, 102, 88, 208, 228,
  166, 119, 114, 248, 235, 117, 75, 10, 49, 68, 80, 180, 143, 237,
  31, 26, 219, 153, 141, 51, 159, 17, 131, 20
};
/**
 * Pad function. Fill input array with unsigned char until 
 * lenght of block is equal to CONFIG_MD2_BLOCK_LEN.
 *
 */
static void md2_pad(void *block, size_t len_pad)
{
	if (len_pad <= CONFIG_MD2_BLOCK_LEN)
	{
		for(int i=(CONFIG_MD2_BLOCK_LEN-len_pad);i<len_pad; i++)
		{
			block[i]=(uint8_t)len_pad;
		}	
	}
}

static void md2_compute(void *state, void *checksum, void *block)
{
	int t=0;	
	uint8_t compute_array[CONFIG_MD2_BLOCK_LEN * 3];
	
	/*
 	 * Copy state and checksum context in compute array. 
 	 */
	memcpy(compute_array, state, CONFIG_MD2_BLOCK_LEN);
	memcpy(compute_array + CONFIG_MD2_BLOCK_LEN, state, CONFIG_MD2_BLOCK_LEN);
	
	/*
 	 * Fill compute array with state XOR block
 	 */
	for(int i=0; i< CONFIG_MD2_BLOCK_LEN; i++)
		compute_array[i + (CONFIG_MD2_BLOCK_LEN * 2)] = state[i] ^ block[i];
	
	/*
	 * Encryt block.
	 */
	for(i=0; i<NUM_COMPUTE_ROUNDS; i++)
	{
		for(int j=0; j<COMPUTE_ARRAY_LEN; j++)
		{
			compute_array[j] ^= md2_perm [t];
			t = compute_array[j];
		}
		
		t = (t + i) & 0xff; //modulo 256.
	}
	/*
	 * Update checksum.	
	 */
	t = checksum[CONFIG_MD2_BLOCK_LEN - 1];

	for(i=0; i< CONFIG_MD2_BLOCK_LEN; i++)
	{
		checksum[i]  ^= md2_perm [block[i] ^ t];
		t = checksum[i];
	}
	
	/*
	 * Update state and clean compute array. 
	 */
	memcpy(state, compute_array, CONFIG_MD2_BLOCK_LEN);
	memset(compute_array, 0, sizeof(compute_array));
}

/**
 * Algorithm initialization.
 *
 * \param empty context.
 */
void md2_init(Md2Context *context)
{
	context->counter=0;
	memset(context->state, 0, sizeof(context->state));
	memset(context->checksum, 0, sizeof(context->checksum));

}
/**
 * Update bock.
 *
 *
 *
 */
void md2_update(Md2Context *context, void *block_in, size_t block_len)
{

}

/**
 * 
 */
void md2_end(Md2Context *context, void *msg_digest)
{

}
