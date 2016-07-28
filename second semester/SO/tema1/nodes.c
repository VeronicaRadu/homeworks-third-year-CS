/*
 * nodes.c
 *
 *  Created on: Mar 7, 2016
 *      Author: arotaru
 */
#include "nodes.h"
#include "utils.h"

pNode new_node(pNode prev, void *data, size_t dlen, pNode next)
{
	pNode n = (pNode)malloc(sizeof(Node));

	n->data = strdup((char *)data);
	n->dlen = dlen;
	n->next = next;
	n->prev = prev;
	return n;
}

void del_node(ppNode n)
{
	if (!n)
		return;
	if (*n) {
		(*n)->dlen = 0;
		free((*n)->data);
		(*n)->data = NULL;

		free(*n);
	}
}

pHNode new_hnode(pNode next)
{
	pHNode n = (pHNode)malloc(sizeof(HNode));

	n->next = next;
	return n;
}

void del_hnode(ppHNode n)
{
	if (!n)
		return;
	if (*n) {
		(*n)->next = NULL;
		free((*n));
		*n = NULL;
	}
}

pTNode new_tnode(pNode prev)
{
	pTNode n = (pTNode)malloc(sizeof(TNode));

	n->prev = prev;
	return n;
}

void del_tnode(ppTNode n)
{
	if (!n)
		return;
	if (*n) {
		(*n)->prev = NULL;
		free((*n));
		*n = NULL;
	}
}
