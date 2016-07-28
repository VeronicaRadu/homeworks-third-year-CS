/*
 * nodes.h
 *
 *  Created on: Mar 7, 2016
 *      Author: arotaru
 */

#ifndef NODES_H_
#define NODES_H_

#include <stdlib.h>

typedef struct nd {
	void *data;
	size_t dlen;
	struct nd *next;
	struct nd *prev;
} Node, *pNode, **ppNode;

/*shallow copy !*/
pNode new_node(pNode prev, void *data, size_t dlen, pNode next);
void del_node(ppNode n);

/*guard for head*/
typedef struct hnd {
	pNode next;
} HNode, *pHNode, **ppHNode;

pHNode new_hnode(pNode next);
void del_hnode(ppHNode n);

/*guard for tail*/
typedef struct tnd {
	pNode prev;
} TNode, *pTNode, **ppTNode;

pTNode new_tnode(pNode prev);
void del_tnode(ppTNode n);
#endif /* NODES_H_ */
