/*!
 * \file
 * <!--
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 2004 Giovanni Bajo
 * All Rights Reserved.
 * -->
 *
 * \brief Portable hash table
 *
 * This file implements a portable hash table, with the following features:
 *
 * \li Open double-hashing. The maximum number of elements is fixed. The double hashing
 * function improves recovery in case of collisions.
 * \li Configurable size (which is clamped to a power of two)
 * \li Visiting interface through iterator (returns the element in random order).
 * \li The key is stored within the data and a hook is used to extract it. Optionally, it
 * is possible to store a copy of the key within the hash table.
 *
 * Since the hashing is open, there is no way to remove elements from the table. Instead, a
 * function is provided to clear the table completely.
 *
 * The data stored within the table must be a pointer. The NULL pointer is used as
 * a marker for a free node, so it is invalid to store a NULL pointer in the table
 * with \c ht_insert().
 *
 * \version $Id$
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.5  2004/10/03 20:43:22  bernie
 *#* Import changes from sc/firmware.
 *#*
 *#* Revision 1.9  2004/06/14 15:15:24  rasky
 *#* Cambiato key_data in un union invece di castare
 *#* Aggiunto un ASSERT sull'indice calcolata nella key_internal_get_ptr
 *#*
 *#* Revision 1.8  2004/06/14 14:59:40  rasky
 *#* Rinominanta la macro di configurazione per rispettare il namespace, e aggiunta in un punto in cui mancava
 *#*
 *#* Revision 1.7  2004/06/12 15:18:05  rasky
 *#* Nuova hashtable con chiave esterna o interna a scelta, come discusso
 *#*
 *#* Revision 1.6  2004/05/26 16:33:31  rasky
 *#* Aggiunta interfaccia per visita della hashtable tramite iteratori
 *#*
 *#* Revision 1.5  2004/05/24 18:42:23  rasky
 *#* Fixato un commento doxygen
 *#*
 *#* Revision 1.4  2004/05/24 15:28:20  rasky
 *#* Sistemata la documentazione, rimossa keycmp in favore della memcmp
 *#*/

#ifndef MWARE_HASHTABLE_H
#define MWARE_HASHTABLE_H

#include <compiler.h>
#include <macros.h>
#include <debug.h>

/*!
 * Enable/disable support to declare special hash tables which maintain a copy of
 * the key internally instead of relying on the hook to extract it from the data.
 */
#define CONFIG_HT_OPTIONAL_INTERNAL_KEY      1

//! Maximum length of the internal key (use (2^n)-1 for slight speedup)
#define INTERNAL_KEY_MAX_LENGTH     15

/*!
 * Hook to get the key from \a data, which is an element of the hash table. The
 * key must be returned together with \a key_length (in words).
 */
typedef const void* (*hook_get_key)(const void* data, uint8_t* key_length);


/*!
 * Hash table description
 *
 * \note This structures MUST NOT be accessed directly. Its definition is
 * provided in the header file only for optimization purposes (see the rationale
 * in hashtable.c).
 *
 * \note If new elements must be added to this list, please double check
 * \c DECLARE_HASHTABLE, which requires the existing elements to be at the top.
 */
struct HashTable
{
	const void** mem;            //!< Buckets of data
	uint16_t max_elts_log2;      //!< Log2 of the size of the table
	struct {
		bool key_internal : 1;   //!< true if the key is copied internally
	} flags;
	union {
		hook_get_key hook;       //!< Hook to get the key
		uint8_t* mem;            //!< Pointer to the key memory
	} key_data;
};


//! Iterator to walk the hash table
typedef struct
{
	const void** pos;
	const void** end;
} HashIterator;


/*!
 * Declare a hash table in the current scope
 *
 * \param name Variable name
 * \param size Number of elements
 * \param hook_gk Hook to be used to extract the key from the node
 *
 * \note The number of elements will be rounded down to the nearest
 * power of two.
 *
 */
#define DECLARE_HASHTABLE(name, size, hook_gk) \
	static const void* name##_nodes[1 << UINT32_LOG2(size)]; \
	struct HashTable name = { name##_nodes, UINT32_LOG2(size), { false }, hook_gk }

/*! Exactly like \c DECLARE_HASHTABLE, but the variable will be declared as static. */
#define DECLARE_HASHTABLE_STATIC(name, size, hook_gk) \
	static const void* name##_nodes[1 << UINT32_LOG2(size)]; \
	static struct HashTable name = { name##_nodes, UINT32_LOG2(size), { false }, hook_gk }

#if CONFIG_HT_OPTIONAL_INTERNAL_KEY
	/*! Declare a hash table with internal copies of the keys. This version does not
	 *  require a hook, nor it requires the user to allocate static memory for the keys.
	 *  It is mostly suggested for tables whose keys are computed on the fly and need
	 *  to be stored somewhere.
	 */
	#define DECLARE_HASHTABLE_INTERNALKEY(name, size) \
		static uint8_t name##_keys[(1 << UINT32_LOG2(size)) * (INTERNAL_KEY_MAX_LENGTH + 1)]; \
		static const void* name##_nodes[1 << UINT32_LOG2(size)]; \
		struct HashTable name = { name##_nodes, UINT32_LOG2(size), { true }, name##_keys }

	/*! Exactly like \c DECLARE_HASHTABLE_INTERNALKEY, but the variable will be declared as static. */
	#define DECLARE_HASHTABLE_INTERNALKEY_STATIC(name, size) \
		static uint8_t name##_keys[(1 << UINT32_LOG2(size)) * (INTERNAL_KEY_MAX_LENGTH + 1)]; \
		static const void* name##_nodes[1 << UINT32_LOG2(size)]; \
		static struct HashTable name = { name##_nodes, UINT32_LOG2(size), { true }, name##_keys }
#endif

/*!
 * Initialize (and clear) a hash table in a memory buffer.
 *
 * \param ht Hash table declared with \c DECLARE_HASHTABLE
 *
 * \note This function must be called before using the hash table. Optionally,
 * it can be called later in the program to clear the hash table, 
 * removing all its elements.
 */
void ht_init(struct HashTable* ht);

/*!
 * Insert an element into the hash table
 *
 * \param ht Handle of the hash table
 * \param data Data to be inserted into the table
 * \return true if insertion was successful, false otherwise (table is full)
 *
 * \note The key for the element to insert is extract from the data with
 * the hook. This means that this function cannot be called for hashtables
 * with internal keys.
 *
 * \note If an element with the same key already exists in the table,
 * it will be overwritten.
 *
 * \note It is not allowed to store NULL in the table. If you pass NULL as data,
 * the function call will fail.
 */
bool ht_insert(struct HashTable* ht, const void* data);

/*!
 * Insert an element into the hash table
 *
 * \param ht Handle of the hash table
 * \param key Key of the element
 * \param key_length Length of the key in characters
 * \param data Data to be inserted into the table
 * \return true if insertion was successful, false otherwise (table is full)
 *
 * \note If this function is called for hash table with external keys,
 * the key provided must be match the key that would be extracted with the
 * hook, otherwise the function will fail.
 *
 * \note If an element with the same key already exists in the table,
 * it will be overwritten.
 *
 * \note It is not allowed to store NULL in the table. If you pass NULL as data,
 * the function call will fail.
 */
bool ht_insert_with_key(struct HashTable* ht, const void* key, uint8_t key_length, const void* data);

/*!
 * Find an element in the hash table
 *
 * \param ht Handle of the hash table
 * \param key Key of the element
 * \param key_length Length of the key in characters
 * \return Data of the element, or NULL if no element was found for the given key.
 */
const void* ht_find(struct HashTable* ht, const void* key, uint8_t key_length);

/*! Similar to \c ht_insert_with_key() but \a key is an ASCIIZ string */
#define ht_insert_str(ht, key, data)         ht_insert_with_key(ht, key, strlen(key), data)

/*! Similar to \c ht_find() but \a key is an ASCIIZ string */
#define ht_find_str(ht, key)                 ht_find(ht, key, strlen(key))

//! Get an iterator to the begin of the hash table \a ht
INLINE HashIterator ht_iter_begin(struct HashTable* ht)
{
	HashIterator h;

	h.pos = &ht->mem[0];
	h.end = &ht->mem[1 << ht->max_elts_log2];

	while (h.pos != h.end && !*h.pos)
		++h.pos;

	return h;
}

/*!
 * Get an iterator to the (exclusive) end of the hash table \a ht
 *
 * \note Like in STL, the end iterator is not a valid iterator (you
 *       cannot call \c ht_iter_get() on it), and it must be used only to
 *       detect if we reached the end of the iteration (through \c ht_iter_cmp()).
 */
INLINE HashIterator ht_iter_end(struct HashTable* ht)
{
	HashIterator h;

	h.pos = h.end = &ht->mem[1 << ht->max_elts_log2];

	return h;
}

//! Compare \a it1 and \a it2 for equality
INLINE bool ht_iter_cmp(HashIterator it1, HashIterator it2)
{
	ASSERT(it1.end == it2.end);
	return it1.pos == it2.pos;
}

//! Get the element within the hash table \a ht pointed by the iterator \a iter
INLINE const void* ht_iter_get(HashIterator iter)
{ return *iter.pos; }

/*! Return an iterator pointing to the element following \a h
 *
 * \note The order of the elements visited during the iteration is casual,
 * and depends on the implementation.
 *
 */
INLINE HashIterator ht_iter_next(HashIterator h)
{
	++h.pos;
	while (h.pos != h.end && !(*h.pos))
		++h.pos;

	return h;
}

#endif /* MWARE_HASHTABLE_H */
