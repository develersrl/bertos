/*!
 * \file
 * <!--
 * Copyright (C) 2004 Giovanni Bajo
 * Copyright (C) 2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Portable hash table implementation
 *
 * Some rationales of our choices in implementation:
 *
 * \li For embedded systems, it is vital to allocate the table in static memory. To do
 * so, it is necessary to expose the \c HashNode and \c HashTable structures in the header file.
 * Nevertheless, they should be used as opaque types (that is, the users should not
 * access the structure fields directly).
 *
 * \li To statically allocate the structures, a macro is provided. With this macro, we
 * are hiding completely \c HashNode to the user (who only manipulates \c HashTable). Without
 * the macro, the user would have had to define both the \c HashNode and the \c HashTable
 * manually, and pass both of them to \c ht_init() (which would have created the link between
 * the two). Instead, the link is created with a literal initialization.
 *
 * \li The hash table is created as power of two to remove the divisions from the code.
 * Of course, hash functions work at their best when the table size is a prime number.
 * When calculating the modulus to convert the hash value to an index, the actual operation
 * becomes a bitwise AND: this is fast, but truncates the value losing bits. Thus, the higher
 * bits are first "merged" with the lower bits through some XOR operations (see the last line of
 * \c calc_hash()).
 *
 * \li To minimize the memory occupation, there is no flag to set for the empty node. An
 * empty node is recognized by its data pointer set to NULL. It is then invalid to store
 * NULL as data pointer in the table.
 *
 * \li The visiting interface through iterators is implemented with pass-by-value semantic.
 * While this is overkill for medium-to-stupid compilers, it is the best designed from an
 * user point of view. Moreover, being totally inlined (defined completely in the header),
 * even a stupid compiler should be able to perform basic optimizations on it.
 * We thought about using a pass-by-pointer semantic but it was much more awful to use, and
 * the compiler is then forced to spill everything to the stack (unless it is *very* smart).
 *
 * \li The current implementation allows to either store the key internally (that is, copy
 * the key within the hash table) or keep it external (that is, a hook is used to extract
 * the key from the data in the node). The former is more memory-hungry of course, as it
 * allocated static space to store the key copies. The overhead to keep both methods at
 * the same time is minimal:
 *    <ul>
 *    <li>There is a run-time check in node_get_key which is execute per each node visited.</li>
 *    <li>Theoretically, there is no memory overhead. In practice, there were no
 *        flags in \c struct HashTable till now, so we had to add a first bit flag, but the
 *        overhead will disappear if a second flag is added for a different reason later.</li>
 *    <li>There is a little interface overhead, since we have two different versions of
 *        \c ht_insert(), one with the key passed as parameter and one without, but in
 *        the common case (external keys) both can be used.</li>
 *    </ul>
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */

/*
 * $Log$
 * Revision 1.1  2004/07/14 14:08:16  rasky
 * Implementazione di una tabella hash
 *
 * Revision 1.13  2004/07/12 16:33:36  rasky
 * Aggiunta nuova ASSERT2, con stringa di descrizione del problema (disabilitabile tramite una macro di configurazione)
 * Modificato il codice del firmware per utilizzare ASSERT2
 * Modificato il progetto in modo da disabilitare le stringhe di errore nel target xROM-xRAM
 *
 * Revision 1.12  2004/06/14 15:15:24  rasky
 * Cambiato key_data in un union invece di castare
 * Aggiunto un ASSERT sull'indice calcolata nella key_internal_get_ptr
 *
 * Revision 1.11  2004/06/14 15:09:04  rasky
 * Cambiati i messaggi di assert (è inutile citare il nome della funzione)
 *
 * Revision 1.10  2004/06/14 15:07:38  rasky
 * Convertito il loop di calc_hash a interi (per farlo ottimizzare maggiormente)
 *
 * Revision 1.9  2004/06/14 14:59:40  rasky
 * Rinominanta la macro di configurazione per rispettare il namespace, e aggiunta in un punto in cui mancava
 *
 * Revision 1.8  2004/06/12 15:18:05  rasky
 * Nuova hashtable con chiave esterna o interna a scelta, come discusso
 *
 * Revision 1.7  2004/06/04 17:16:31  rasky
 * Fixato un bug nel caso in cui la chiave ecceda la dimensione massima: il clamp non può essere fatto dentro la perform_lookup perché anche la ht_insert deve avere il valore clampato a disposizione per fare la memcpy
 *
 * Revision 1.6  2004/05/26 16:36:50  rasky
 * Aggiunto il rationale per l'interfaccia degli iteratori
 *
 * Revision 1.5  2004/05/24 15:28:20  rasky
 * Sistemata la documentazione, rimossa keycmp in favore della memcmp
 *
 */
#include "hashtable.h"
#include <drv/kdebug.h>
#include <compiler.h>
#include <string.h>


#define ROTATE_LEFT_16(num, count)     (((num) << (count)) | ((num) >> (16-(count))))
#define ROTATE_RIGHT_16(num, count)    ROTATE_LEFT_16(num, 16-(count))

typedef const void** HashNodePtr;
#define NODE_EMPTY(node)               (!*(node))
#define HT_HAS_INTERNAL_KEY(ht)        (CONFIG_HT_OPTIONAL_INTERNAL_KEY && ht->flags.key_internal)

/*! For hash tables with internal keys, compute the pointer to the internal key for a given \a node. */
INLINE uint8_t* key_internal_get_ptr(struct HashTable* ht, HashNodePtr node)
{
	uint8_t* key_buf = ht->key_data.mem;
	size_t index;

	// Compute the index of the node and use it to move within the whole key buffer
	index = node - &ht->mem[0];
	ASSERT(index < (1 << ht->max_elts_log2));
	key_buf += index * (INTERNAL_KEY_MAX_LENGTH + 1);

	return key_buf;
}


INLINE void node_get_key(struct HashTable* ht, HashNodePtr node, const void** key, uint8_t* key_length)
{
	if (HT_HAS_INTERNAL_KEY(ht))
	{
		uint8_t* k = key_internal_get_ptr(ht, node);

		// Key has its length stored in the first byte
		*key_length = *k++;
		*key = k;
	}
	else
		*key = ht->key_data.hook(*node, key_length);
}

INLINE bool node_key_match(struct HashTable* ht, HashNodePtr node, const void* key, uint8_t key_length)
{
	const void* key2;
	uint8_t key2_length;

	node_get_key(ht, node, &key2, &key2_length);

	return (key_length == key2_length && memcmp(key, key2, key_length) == 0);
}

static uint16_t calc_hash(const void* _key, uint8_t key_length)
{
	const char* key = (const char*)_key;
	uint16_t hash = key_length;
	int i;
	int len = (int)key_length;

	for (i = 0; i < len; ++i)
		hash = ROTATE_LEFT_16(hash, 4) ^ key[i];

	return hash ^ (hash >> 6) ^ (hash >> 13);
}

static HashNodePtr perform_lookup(struct HashTable* ht,
                                  const void* key, uint8_t key_length)
{
	uint16_t hash = calc_hash(key, key_length);
	uint16_t mask = ((1 << ht->max_elts_log2) - 1);
	uint16_t index = hash & mask;
	uint16_t first_index = index;
	uint16_t step;
	HashNodePtr node;

	// Fast-path optimization: we check immediately if the current node
	//  is the one we were looking for, so we save the computation of the
	//  increment step in the common case.
	node = &ht->mem[index];
	if (NODE_EMPTY(node)
		|| node_key_match(ht, node, key, key_length))
		return node;

	// Increment while going through the hash table in case of collision.
	//  This implements the double-hash technique: we use the higher part
	//  of the hash as a step increment instead of just going to the next
	//  element, to minimize the collisions. 
	// Notice that the number must be odd to be sure that the whole table
	//  is traversed. Actually MCD(table_size, step) must be 1, but 
	//  table_size is always a power of 2, so we just ensure that step is
	//  never a multiple of 2.
	step = (ROTATE_RIGHT_16(hash, ht->max_elts_log2) & mask) | 1;

	do
	{
		index += step;
		index &= mask;

		node = &ht->mem[index];
		if (NODE_EMPTY(node)
			|| node_key_match(ht, node, key, key_length))
			return node;

		// The check is done after the key compare. This actually causes
		//  one more compare in the case the table is full (since the first
		//  element was compared at the very start, and then at the end),
		//  but it makes faster the common path where we enter this loop
		//  for the first time, and index will not match first_index for
		//  sure.
	} while (index != first_index);

	return NULL;
}

void ht_init(struct HashTable* ht)
{
	memset(ht->mem, 0, sizeof(ht->mem[0]) * (1 << ht->max_elts_log2));
}

static bool insert(struct HashTable* ht, const void* key, uint8_t key_length, const void* data)
{
	HashNodePtr node;

	if (!data)
		return false;

	if (HT_HAS_INTERNAL_KEY(ht))
		key_length = MIN(key_length, INTERNAL_KEY_MAX_LENGTH);

	node = perform_lookup(ht, key, key_length);
	if (!node)
		return false;

	if (HT_HAS_INTERNAL_KEY(ht))
	{
		uint8_t* k = key_internal_get_ptr(ht, node);
		*k++ = key_length;
		memcpy(k, key, key_length);
	}

	*node = data;
	return true;
}

bool ht_insert_with_key(struct HashTable* ht, const void* key, uint8_t key_length, const void* data)
{
#ifdef _DEBUG
	if (!HT_HAS_INTERNAL_KEY(ht))
	{
		// Construct a fake node and use it to match the key
		HashNodePtr node = &data;
		if (!node_key_match(ht, node, key, key_length))
		{
			ASSERT2(0, "parameter key is different from the external key");
			return false;
		}
	}
#endif

	return insert(ht, key, key_length, data);
}

bool ht_insert(struct HashTable* ht, const void* data)
{
	const void* key;
	uint8_t key_length;

#ifdef _DEBUG
	if (HT_HAS_INTERNAL_KEY(ht))
	{
		ASSERT("parameter cannot be a hash table with internal keys - use ht_insert_with_key()"
		       && 0);
		return false;
	}
#endif

	key = ht->key_data.hook(data, &key_length);

	return insert(ht, key, key_length, data);
}

const void* ht_find(struct HashTable* ht, const void* key, uint8_t key_length)
{
	HashNodePtr node;
	
	if (HT_HAS_INTERNAL_KEY(ht))
		key_length = MIN(key_length, INTERNAL_KEY_MAX_LENGTH);

	node = perform_lookup(ht, key, key_length);

	if (!node || NODE_EMPTY(node))
		return NULL;

	return *node;
}


#if 0

#include <stdlib.h>

bool ht_test(void);

static const void* test_get_key(const void* ptr, uint8_t* length)
{
	const char* s = ptr;
	*length = strlen(s);
	return s;
}

#define NUM_ELEMENTS   256
DECLARE_HASHTABLE_STATIC(test1, 256, test_get_key);
DECLARE_HASHTABLE_INTERNALKEY_STATIC(test2, 256);

static char data[NUM_ELEMENTS][10];
static char keydomain[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

static bool single_test(void)
{
	int i;

	ht_init(&test1);
	ht_init(&test2);

	for (i=0;i<NUM_ELEMENTS;i++)
	{
		int k;
		int klen;
		
		do 
		{
			klen = (rand() % 8) + 1;
			for (k=0;k<klen;k++)
				data[i][k] = keydomain[rand() % (sizeof(keydomain)-1)];
			data[i][k]=0;
		} while (ht_find_str(&test1, data[i]) != NULL);

		ASSERT(ht_insert(&test1, data[i]));
		ASSERT(ht_insert_str(&test2, data[i], data[i]));
	}

	for (i=0;i<NUM_ELEMENTS;i++)
	{
		char *found1, *found2;
		
		found1 = (char*)ht_find_str(&test1, data[i]);
		if (strcmp(found1, data[i]) != 0)
		{
			ASSERT(strcmp(found1,data[i]) == 0);
			return false;
		}

		found2 = (char*)ht_find_str(&test2, data[i]);
		if (strcmp(found2, data[i]) != 0)
		{
			ASSERT(strcmp(found2,data[i]) == 0);
			return false;
		}
	}

	return true;
}

static uint16_t rand_seeds[] = { 1, 42, 666, 0xDEAD, 0xBEEF, 0x1337, 0xB00B };

bool ht_test(void)
{
	int i;

	for (i=0;i<countof(rand_seeds);++i)
	{
		srand(rand_seeds[i]);
		if (!single_test())
		{
			kprintf("ht_test failed\n");
			return false;
		}
	}

	kprintf("ht_test successful\n");
	return true;
}

#endif
