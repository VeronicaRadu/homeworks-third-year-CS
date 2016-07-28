/* Rotaru Alexandru Andrei 335CC*/
#ifndef __PRIORITY_QUEUE_H__
#define __PRIORITY_QUEUE_H__

#define MAX_QUEUE_ENTRIES_COUNT 256

/**
 * Function type that makes the comparisons between the entries
 * when operations on the queue are made.
 */
typedef void *queue_entry_t;
typedef int (*entry_comparer_t)(const void*, const void *);

/**
 * The data structure that holds the entries aranged as a heap.
 * The member function pointer cmp decides the order the entries
 * are stored int the queue.
 */
typedef struct pqueue {
	unsigned elem_cnt;
	unsigned elem_cap;
	queue_entry_t *entries;
	entry_comparer_t cmp_priority;
	entry_comparer_t cmp_information;
} priority_queue_t;

/**
 * Constructor and destructor of the data structure.
 */
priority_queue_t
new_priority_queue(unsigned initial_capacity,
					entry_comparer_t prior,
					entry_comparer_t info);
int del_priority_queue(priority_queue_t *pqueue);


/**
 * Inserts into the queue given as argument the new entry based
 * on the priority of the entry, and keeps the heap property.
 */
int priority_queue_insert(queue_entry_t entry, priority_queue_t *queue);
/*insert(entry)into(queue)*/
#define into(queue) ,queue)
#define insert(data) priority_queue_insert(data


/**
 * Remove the data from the queue and keeps the heap property of the queue.
 */
int priority_queue_delete(queue_entry_t entry, priority_queue_t *queue);
/*delete(data)with_priority(priority)from(queue)*/
#define from(queue) ,queue)
#define delete(data) priority_queue_delete(data


/**
 * Updates the priority of the data  given int the queue (if exists)
 * and then repercolates the array so its keeps the heap property.
 */
int priority_queue_update(queue_entry_t entry, priority_queue_t *queue);
/*update(data)to_priority(priority)in(queue)*/
#define in(queue) ,queue)
#define update(data) priority_queue_update(data

/**
 * eliminates the element from the top
 */
void priority_queue_pop(priority_queue_t *queue);
/*top(queue)*/
#define pop(queue) priority_queue_pop(queue)

/**
 * Returns the data with the biggest prioerity in the queue.
 */
queue_entry_t priority_queue_top(priority_queue_t *queue);
/*top(queue)*/
#define top(queue) priority_queue_top(queue)


typedef void (*printf_queue_entry_t)(const void *);
/* only for debug*/
void print_queue(priority_queue_t *pq, printf_queue_entry_t);

#endif
