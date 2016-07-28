#ifndef HELPERS_H_
#define HELPERS_H_

#include "common.h"

typedef enum pagestate {
	STATE_IN_RAM,
	STATE_IN_SWAP,
	STATE_NOT_ALLOC
} page_state;

struct frame;

/*handle pages (virtual pages)*/
typedef struct page_table_entry {
	page_state state;
	page_state prev_state;
	w_boolean_t dirty;
	w_prot_t protection;
	w_ptr_t start;
	struct frame *frame; /* NULL in case page is not mapped */
} page_table_entry_t;

/* handle frames (physical pages) */
typedef struct frame {
	w_size_t index;
	page_table_entry_t *pte;
} frame_t;

#endif
