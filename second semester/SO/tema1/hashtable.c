/*
 * hashtable.c
 *
 *  Created on: Mar 8, 2016
 *      Author: arotaru
 */
#include "hashtable.h"

pHashTable new_hash_table(size_t size, h has, comparer c)
{
	pHashTable ret = NULL;
	size_t idx = 0;

	TRACE("", "%zd, %p, %p", size, has, c);
	if (0 == size || NULL == has || NULL == c)
		return NULL;

	ret = (pHashTable)malloc(sizeof(HashTable));
	ret->size = size;
	ret->hashfunct = has;
	ret->cp = c;

	ret->table = (ppList)malloc(ret->size * sizeof(pList));
	for (; idx < ret->size; ++idx)
		ret->table[idx] = new_list(ret->cp);

	return ret;
}

int addhash(pHashTable pht, void *d, size_t dl)
{
	size_t idx = 0;

	TRACE("", "%p, %s, %zd", pht, (char *)d, dl);
	if (NULL == pht || NULL == d || 0 == dl)
		return ERROR_FAILURE;

	idx = pht->hashfunct((char *)d, pht->size);
	return insert(pht->table[idx], d, dl);
}

int removeh(pHashTable pht, void *d, size_t dl)
{
	size_t idx = 0;

	TRACE("", "%p, %s, %zd", pht, (char *)d, dl);
	if (NULL == pht || NULL == d || 0 == dl)
		return ERROR_FAILURE;

	idx = pht->hashfunct((char *)d, pht->size);
	return removelst(pht->table[idx], d, dl);
}

int findhas(pHashTable pht, void *d, size_t dl)
{
	size_t idx = 0;
	pNode target = NULL;

	TRACE("", "%p, %s, %zd", pht, (char *)d, dl);
	if (NULL == pht || NULL == d || 0 == dl)
		return ERROR_FAILURE;

	idx = pht->hashfunct((char *)d, pht->size);
	target = findlst(pht->table[idx], d, dl);
	return (NULL != target ? TRUE : FALSE);
}

void print_bucketh(pHashTable pht, size_t index, FILE *to)
{
	TRACE("", "%p, %zd", pht, index);
	if (NULL == pht || index >= pht->size)
		return;
	if (NULL == to)
		to = stdout;

	printlst(pht->table[index], to);
}

pNode next_node(pHashTable pht, pNode curr_node)
{
	pNode next = NULL;
	size_t idx = 0;

	TRACE("", "%p %p", pht, curr_node);
	if (NULL == pht)
		return NULL;

	if (NULL == curr_node) {
		for (idx = 0; idx < pht->size; ++idx) {
			if (NULL != pht->table[idx]->head && NULL != pht->table[idx]->head->next) {
				next = pht->table[idx]->head->next;
				break;
			}
		}
	} else if (NULL != curr_node->next) {
		next = curr_node->next;
	} else {
		idx = pht->hashfunct((char *)curr_node->data, pht->size) + 1;
		for (; idx < pht->size; ++idx) {
			if (NULL != pht->table[idx]->head && NULL != pht->table[idx]->head->next) {
				next = pht->table[idx]->head->next;
				break;
			}
		}
	}
	return next;
}

int resizeh(pHashTable pht, size_t new_size)
{
	int ret = ERROR_SUCCESS;
	size_t idx = 0;
	pNode curr = NULL;
	ppList new_table = NULL;

	TRACE("", "%p %zd", pht, new_size);
	if (NULL == pht || 0 == new_size)
		return ERROR_FAILURE;

	new_table = (ppList)malloc(new_size * sizeof(pList));
	for (; idx < new_size; ++idx)
		new_table[idx] = new_list(pht->cp);

	curr = next_node(pht, NULL);
	while (NULL != curr) {
		idx = pht->hashfunct((char *)curr->data, new_size);
		ret = ret | insert(new_table[idx], curr->data, curr->dlen);
		curr = next_node(pht, curr);
	}

	for (idx = 0; idx < pht->size; ++idx) {
		clearlst(pht->table[idx]);
		free(pht->table[idx]);
		pht->table[idx] = NULL;
	}

	free(pht->table);
	pht->size = new_size;
	pht->table = new_table;

	return ret;
}

int clearhs(pHashTable pht)
{
	size_t idx = 0;
	int ret = ERROR_SUCCESS;

	TRACE("", "%p", pht);
	if (NULL == pht)
		return ERROR_FAILURE;
	for (; idx < pht->size; ++idx)
		clearlst(pht->table[idx]);

	return ret;
}

int destroy(pHashTable pht)
{
	int ret = ERROR_SUCCESS;
	size_t idx = 0;

	TRACE("", "%p", pht);
	if (NULL == pht)
		return ERROR_FAILURE;
	for (; idx < pht->size; ++idx) {
		clearlst(pht->table[idx]);
		free(pht->table[idx]);
		pht->table[idx] = NULL;
	}
	free(pht->table);
	pht->table = NULL;
	pht->size = 0;
	pht->cp = NULL;
	pht->hashfunct = NULL;

	return ret;
}
