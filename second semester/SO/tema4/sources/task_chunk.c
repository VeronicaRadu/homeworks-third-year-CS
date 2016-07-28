/* Rotaru Alexandru Andrei 335CC*/
#include "../headers/task_chunk.h"
#include <stdlib.h>
#include <stdio.h>

task_chunk_t *new_task_chunk(tid_t id, unsigned p, so_handler *h, sem_t s)
{
	task_chunk_t tsk = {
		.tid = id,
		.spriority = p,
		.handler = h,
		.sem = s
	};
	task_chunk_t *ret = (task_chunk_t *)malloc(sizeof(task_chunk_t));

	*ret = tsk;
	return ret;
}

int compare_thread_id(const void *a1, const void *a2)
{
	task_chunk_t t1 = *(task_chunk_t *)a1;
	task_chunk_t t2 = *(task_chunk_t *)a2;

	int diff = t1.tid - t2.tid;
	return diff;
}

int compare_thread_priority(const void *a1, const void *a2)
{
	task_chunk_t t1 = *(task_chunk_t *)a1;
	task_chunk_t t2 = *(task_chunk_t *)a2;

	int diff = (t2.spriority - t1.spriority);
	if (0 == diff)
		return (t1.time_stamp - t2.time_stamp);

	return diff;
}

void printf_task(const void *t)
{
	task_chunk_t *tsk = (task_chunk_t *)t;

	if (NULL == t)
		return;
	printf("(id=%lu, prio=%u, time=%d) ", tsk->tid, tsk->spriority, tsk->time_stamp);
}
