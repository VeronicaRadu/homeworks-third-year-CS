#include <stdlib.h>
#include <stdio.h>
#include "list.h"

void list_add(void *data, list_node_t *node)
{
	if (NULL == data || NULL == node)
		return;
	list_node_t *iter = NULL;

	for (iter = node; NULL != iter->next; iter = iter->next) {
		if (data == iter->info)
			return;
	}
	if (data == iter->info)
		return;
	iter->next = (list_node_t *)malloc(sizeof(list_node_t));
	iter->next->info = data;
	iter->next->next = NULL;
}

void list_rem(void *data, list_node_t *node)
{
	if (NULL == data || NULL == node)
		return;
	list_node_t *piter = node;
	list_node_t *iter = node->next;

	for (; NULL != iter->next; piter = iter, iter = iter->next) {
		if (data == iter->info) {
			list_node_t *temp = iter;

			piter->next = iter->next;
			free(temp);
			break;
		}
	}
}

void list_shw(list_node_t *node)
{
	if (NULL == node)
		return;
	list_node_t *iter = NULL;

	for (iter = node; NULL != iter->next; iter = iter->next)
		printf("%p ->", iter);
	printf("NULL\n");
}
