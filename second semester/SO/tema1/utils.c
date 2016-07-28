/*
 * utils.c
 *
 *  Created on: Mar 7, 2016
 *      Author: arotaru
 */
#include "utils.h"

int cmprer(void *d1, size_t dl1, void *d2, size_t dl2)
{
	if (NULL == d1 || 0 == dl1 || NULL == d2 || 0 == dl2)
		return ERROR_FAILURE;
	return strcmp((char *)d1, (char *)d2);
}

void correct_buffer(char *b)
{
	if (NULL == b)
		return;
	b[strlen(b) - 1] = '\0';
}
