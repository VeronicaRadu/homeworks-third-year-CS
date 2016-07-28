/*Rotaru Alexandru Andrei 335CC*/
#ifndef __TASK_CHUNK_H__
#define __TASK_CHUNK_H__

#include "../headers/so_scheduler.h"
#include <semaphore.h>

/*
 * A structure that represents my internal thread structure. The tid is for the result of pthread
 * create, the spriority is static and set at beginning, handler is a function that the thread has
 * to execute,
 */
typedef struct my_task_struct {
	tid_t tid;
	unsigned spriority;
	so_handler *handler;
	sem_t sem;
	unsigned time_stamp;
} task_chunk_t;

task_chunk_t *new_task_chunk(tid_t id, unsigned p, so_handler *h, sem_t s);
int compare_thread_id(const void *a1, const void *a2);
int compare_thread_priority(const void *a1, const void *a2);
void printf_task(const void *t);

#endif
