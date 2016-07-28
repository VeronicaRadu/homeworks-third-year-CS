/*
 * list.h
 *
 *  Created on: Mar 7, 2016
 *      Author: arotaru
 */

#ifndef LIST_H_
#define LIST_H_

#include <stdio.h>
#include "nodes.h"
#include "utils.h"

typedef struct l {
	pHNode head;
	pTNode tail;
	comparer cmp;
} List, *pList, **ppList;

pList new_list(comparer);
int insert(pList l, void *d, size_t dl);
pNode findlst(pList l, void *d, size_t dl);
int removelst(pList l, void *d, size_t dl);
int clearlst(pList l);
int printlst(pList l, FILE *to);


#endif /* LIST_H_ */
