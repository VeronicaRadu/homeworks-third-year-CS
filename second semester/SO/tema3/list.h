#ifndef LIST_H_
#define LIST_H_

typedef struct list_node {
	void *info;
	struct list_node *next;
} list_node_t;

/*
 * Add a new element if the at least a node exists, namely the first node
 * should already be allocated
 */
void list_add(void *data, list_node_t *node);

/*
 * Remove an element from a list if the list has at least an element
 */
void list_rem(void *data, list_node_t *node);

/*
 * Only for debug purpose
 */
void list_shw(list_node_t *node);

/*
 * Nice way to call the functions
 */
#define into(list) ,list)
#define insert(data) list_add(data
#define from(list) ,list)
#define remove(data) list_rem(data
/*
 *	insert (data) into (&list);
 *	remove (data) from (&list);
 *	list is on the stacks
 *	data is the information
*/

#endif
