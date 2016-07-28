/*
 * hashtable.h
 *
 *  Created on: Mar 8, 2016
 *      Author: arotaru
 */

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#define TRUE 1
#define FALSE 0

#include "list.h"
#include "utils.h"
#include "hash.h"

typedef struct ht {
	size_t size;
	ppList table;
	h hashfunct;
	comparer cp;
} HashTable, *pHashTable, **ppHashTable;

pHashTable new_hash_table(size_t size, h has, comparer c);
int addhash(pHashTable pht, void *d, size_t dl);
int removeh(pHashTable pht, void *d, size_t dl);
int findhas(pHashTable pht, void *d, size_t dl);
void print_bucketh(pHashTable pht, size_t index, FILE *to);
int resizeh(pHashTable pht, size_t new_size);
int clearhs(pHashTable pht);
int destroy(pHashTable pht);

#endif /* HASHTABLE_H_ */
