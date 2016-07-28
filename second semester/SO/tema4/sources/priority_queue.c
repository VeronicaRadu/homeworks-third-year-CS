/*Rotaru Alexandru Andrei 335CC*/
#include "../headers/priority_queue.h"
#include "../headers/utils.h"
#include "../headers/debug.h"
#include <stdlib.h>


priority_queue_t new_priority_queue(unsigned initial_capacity,
									entry_comparer_t cmp_prior,
									entry_comparer_t cmp_infor)
{
	priority_queue_t queue = {
		.elem_cnt = 0,
		.elem_cap = initial_capacity,
		.entries = (queue_entry_t *)malloc((initial_capacity + 1) * sizeof(queue_entry_t)),
		.cmp_priority = cmp_prior,
		.cmp_information = cmp_infor
	};
	return queue;
}

int del_priority_queue(priority_queue_t *pqueue)
{
	if (NULL == pqueue)
		return -1;

	free(pqueue->entries);
	pqueue->entries = NULL;
	pqueue->cmp_priority = NULL;
	pqueue->cmp_information = NULL;
	pqueue->elem_cap = 0;
	pqueue->elem_cnt = 0;
	return 0;
}

static void swap_entries(queue_entry_t *e1, queue_entry_t *e2)
{
	queue_entry_t etemp = *e2;
	*e2 = *e1;
	*e1 = etemp;
}

/*
 * Swaps the elemetn on the current position with its parent or one of its
 * children untill the heap property holds.
 */
static int percolate_or_swift(unsigned index, priority_queue_t *queue)
{
	if (index >= queue->elem_cnt)
		return 0;
	unsigned current_idx	= index;
	unsigned parent_idx		= (unsigned)((index - 1 )/ 2);
	unsigned child1_idx		= index * 2 + 1;
	unsigned child2_idx		= child1_idx + 1;

	while(1) {
		parent_idx		= (unsigned)((current_idx - 1)/ 2);
		child1_idx		= current_idx * 2 + 1;
		child2_idx		= child1_idx + 1;
		if (current_idx != 0
			&& 0 < queue->cmp_priority(queue->entries[parent_idx], queue->entries[current_idx])) {
			swap_entries(&queue->entries[parent_idx], &queue->entries[current_idx]);
			current_idx = parent_idx;
		} else if (child2_idx < queue->elem_cnt) {
			int diff = queue->cmp_priority(queue->entries[child1_idx], queue->entries[child2_idx]);

			if (diff < 0
				&& 0 <= queue->cmp_priority(queue->entries[current_idx], queue->entries[child1_idx])) {

				swap_entries(&queue->entries[current_idx], &queue->entries[child1_idx]);
				current_idx = child1_idx;
			} else if (diff > 0
				&& 0 <= queue->cmp_priority(queue->entries[current_idx], queue->entries[child2_idx])) {

				swap_entries(&queue->entries[current_idx], &queue->entries[child2_idx]);
				current_idx = child2_idx;
			} else
				break;
		} else if (child1_idx < queue->elem_cnt) {
			if (0 <= queue->cmp_priority(queue->entries[current_idx], queue->entries[child1_idx])) {
				swap_entries(&queue->entries[current_idx], &queue->entries[child1_idx]);
				current_idx = child1_idx;
			} else
				break;
		} else
			break;
	}
	return 0;
}

/* Return the index of the data in the queue->entries array*/
static int queue_find(queue_entry_t data, priority_queue_t *queue)
{
	unsigned pos = 0u;
	/*I hate linear search*/
	for (; pos < queue->elem_cnt; ++pos) {
		if (0 == queue->cmp_information(data, queue->entries[pos]))
			return pos;
	}
	return -1;
}

static int resize(priority_queue_t *queue)
{
	if (NULL == queue)
		return -1;

	queue->elem_cap = queue->elem_cap << 1;
	queue->entries = (queue_entry_t *)realloc(queue->entries, queue->elem_cap * sizeof(queue_entry_t));
	if (NULL == queue->entries)
		return -1;
	return 0;
}

/*
 * Inserts in the queue a new entry
 */
int priority_queue_insert(queue_entry_t entry, priority_queue_t *queue)
{
	if (NULL == queue)
		return -1;

	if (queue->elem_cnt >= queue->elem_cap)
		resize(queue);

	unsigned index = 0;
	queue->entries[index = queue->elem_cnt++] = entry;
	return percolate_or_swift(index, queue);
}

/*
 * Searches in the queue the entry withe the given data
 */
int priority_queue_delete(queue_entry_t entry, priority_queue_t *queue)
{
	if (NULL == queue)
		return -1;

	int index = queue_find(entry, queue);
	if (-1 == index)
		return -1;

	swap_entries(&queue->entries[--queue->elem_cnt], &queue->entries[index]);
	return percolate_or_swift(index, queue);
}

/*
 * Searches in the heap the element and updates the priority of the given data
 * an then swaps the elements so it keeps the heap property.
 */
int priority_queue_update(queue_entry_t entry, priority_queue_t *queue)
{
	if (NULL == queue)
		return -1;
	int index = queue_find(entry, queue);
	if (-1 == index)
		return -1;

	queue->entries[index] = entry;
	return percolate_or_swift(index, queue);
}

/*
 * Eliminates the elemenent from the top of the heap.
 */
void priority_queue_pop(priority_queue_t *queue)
{
	if (NULL == queue || queue->elem_cnt == 0)
		return;
	swap_entries(&queue->entries[queue->elem_cnt - 1], &queue->entries[0]);
	queue->elem_cnt--;
	percolate_or_swift(0, queue);
}

/*
 * Return the elements from the top of the heap.
 */
queue_entry_t priority_queue_top(priority_queue_t *queue)
{
	if (NULL == queue || queue->elem_cnt == 0)
		return NULL;

	return queue->entries[0];
}

/*
 * Print the internal structure of the heap, only for the debugging
 */
void print_queue(priority_queue_t *queue, printf_queue_entry_t printer)
{
	if (NULL == queue)
		return;

	unsigned idx = 0u;
	for (; idx < queue->elem_cnt; ++idx) {
		printer(queue->entries[idx]);
	}
	printf("\n");
}
