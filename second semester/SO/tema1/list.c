#include <stdio.h>
#include "list.h"
#include "utils.h"

pList new_list(comparer cmp)
{
	pList lst = NULL;

	if (NULL == cmp)
		return NULL;

	lst = (pList)malloc(sizeof(List));
	lst->head = NULL;
	lst->tail = NULL;
	lst->cmp = cmp;
	return lst;
}

int insert(pList l, void *d, size_t dl)
{
	int ret = ERROR_SUCCESS;
	pNode n = NULL;
	pNode iterator = NULL;

	TRACE("", "%p, %s, %zd", l, (char *)d, dl);
	if (NULL == l || NULL == d || 0 == dl)
		return ERROR_FAILURE;

	if (NULL == l->head) {
		n = new_node(NULL, d, dl, NULL);
		l->head = new_hnode(n);
		l->tail = new_tnode(n);
	} else if (NULL == l->head->next) {
		n = new_node(NULL, d, dl, NULL);
		l->head->next = n;
		l->tail->prev = n;
	} else {
		iterator = l->head->next;
		for (; NULL != iterator; iterator = iterator->next)
			if (0 == l->cmp(iterator->data, iterator->dlen, d, dl))
				return ret;

		iterator = l->tail->prev;
		iterator->next = new_node(iterator, d, dl, NULL);
		l->tail->prev = iterator->next;
	}
	return ret;
}

pNode findlst(pList l, void *d, size_t dl)
{
	pNode ret = NULL;
	pNode iterator = NULL;

	TRACE("", "%p, %s, %zd", l, (char *)d, dl);
	if (NULL == l || NULL == d || 0 == dl || NULL == l->head)
		return NULL;

	iterator = l->head->next;
	for (; NULL != iterator; iterator = iterator->next)
		if (0 == l->cmp(iterator->data, iterator->dlen, d, dl))
			return iterator;
	return ret;
}

int removelst(pList l, void *d, size_t dl)
{
	int ret = ERROR_SUCCESS;
	pNode ptr = NULL;

	TRACE("", "%p, %s, %zd", l, (char *)d, dl);
	if (NULL == l || NULL == d || 0 == dl)
		return ERROR_FAILURE;

	ptr = findlst(l, d, dl);
	if (NULL == ptr)
		return ret;
	if (ptr == l->head->next && ptr == l->tail->prev) {
		l->head->next = NULL;
		l->tail->prev = NULL;
	} else if (ptr == l->head->next) {
		l->head->next = ptr->next;
		l->head->next->prev = NULL;
	} else if (ptr == l->tail->prev) {
		l->tail->prev = ptr->prev;
		l->tail->prev->next = NULL;
	} else {
		ptr->next->prev = ptr->prev;
		ptr->prev->next = ptr->next;
	}
	del_node(&ptr);
	return ret;
}

int clearlst(pList l)
{
	int ret = ERROR_SUCCESS;
	pNode iterator = NULL;

	TRACE("", "%p", l);
	if (NULL == l)
		return ERROR_FAILURE;
	if (NULL == l->head)
		return ERROR_FAILURE;
	if (NULL == l->head->next)
		return ERROR_FAILURE;

	iterator = l->head->next;
	while (NULL != iterator) {
		pNode temp = iterator;

		iterator = iterator->next;
		del_node(&temp);
	}
	del_hnode(&l->head);
	del_tnode(&l->tail);
	return ret;
}

int printlst(pList l, FILE *to)
{
	int ret = ERROR_SUCCESS;
	pNode iterator = NULL;

	TRACE("", "%p, %p", l, to);
	if (NULL == l)
		return ERROR_FAILURE;
	if (NULL == l->head)
		return ERROR_FAILURE;
	if (NULL == to)
		to = stdout;

	iterator = l->head->next;
	for (; NULL != iterator; iterator = iterator->next) {
		char *word = (char *)iterator->data;

		fprintf(to, "%s ", word);
	}
	fprintf(to, "\n");
	return ret;
}
