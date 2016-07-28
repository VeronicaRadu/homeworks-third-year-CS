#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#include "vmsim.h"
#include "common.h"
#include "debug.h"
#include "helpers.h"
#include "list.h"
#include "util.h"

typedef struct allc {
	vm_map_t vmap;
	page_table_entry_t *vpages;
	frame_t *frames;
	w_size_t num_pages;
	w_size_t num_frames;
	char *swp_file_name;
	char *ram_file_name;
} allocation_t;

static list_node_t allocations_list;

/*maps the page from pte to frame with index in ram handle of allc*/
static void swap_in(allocation_t *alloc, page_table_entry_t *pte, w_size_t frame_idx)
{
	DIE(NULL == alloc || NULL == pte, "Invalid pointer argument.");

	w_size_t swap_offset = (char *)pte->start - (char *)alloc->vmap.start;
	w_boolean_t r = FALSE;
	w_size_t page_size = p_sz;

	int rm = munmap(pte->start, p_sz);
	DIE(0 > rm, "Couldn't unmap.");
	pte->start = mmap(pte->start,
					page_size,
					PROT_READ | PROT_WRITE,
					MAP_SHARED | MAP_FIXED,
					alloc->vmap.ram_handle,
					frame_idx * page_size);
	DIE(NULL == pte->start, "Couldn't map");
	pte->dirty = FALSE;
	pte->prev_state = pte->state;
	pte->state = STATE_IN_RAM;
	pte->protection = PROTECTION_READ;
	pte->frame = &alloc->frames[frame_idx];
	alloc->frames[frame_idx].pte = pte;

	r = w_set_file_pointer(alloc->vmap.swap_handle, swap_offset);
	DIE(FALSE == r, "Couldn't set file pointer");
	r = w_read_file(alloc->vmap.swap_handle, pte->start, page_size);
	DIE(FALSE == r, "Couldn't read file from pointer");
	r = w_protect_mapping(pte->start, 1, pte->protection);
	DIE(FALSE == r, "Couldn't protect");
}

static void swap_out(allocation_t *alloc, page_table_entry_t *pte, w_size_t frame_idx)
{
	w_size_t page_size = p_sz;

	if (STATE_NOT_ALLOC == pte->prev_state || TRUE == pte->dirty) {
		/*compute the position to map the memory in the swap*/
		w_boolean_t r = FALSE;
		w_size_t offset = (char *)pte->start - (char *)alloc->vmap.start;

		r = w_set_file_pointer(alloc->vmap.swap_handle, offset);
		DIE(FALSE == r, "Coudln't set the file pointer.");
		r = w_write_file(alloc->vmap.swap_handle, pte->start, page_size);
		DIE(FALSE == r, "Couldn't write the file.");
	}
	int ru = munmap(pte->start, page_size);
	DIE(0 > ru, "Couldn't unmap");
	pte->start = mmap(pte->start,
						page_size,
						PROT_NONE,
						MAP_SHARED | MAP_ANONYMOUS,
						-1,
						0);
	DIE(NULL == pte->start, "Couldn't map");
	pte->dirty = FALSE;
	pte->frame->pte = NULL;
	pte->frame = NULL;
	pte->prev_state = pte->state;
	pte->state = STATE_IN_SWAP;
	pte->protection = PROTECTION_NONE;
}



/*
 * Searches in the ram a page that is free, and if isn't any then
 * a page is swaped out and the positions of it is returned
 */
static w_size_t get_a_frame(allocation_t *allc)
{
	w_size_t idx = 0;
	/*I hate linear search*/
	for (idx = 0; idx < allc->num_frames; ++idx) {
		if (NULL == allc->frames[idx].pte) {
			return idx;
		}
	}

	/*goint to use the first frame*/
	page_table_entry_t *pte = allc->frames[0].pte;
	swap_out(allc, pte, 0);
	return 0;
}

/*Defining our own sigsegv signal handler*/
void custom_sigsegv_handler(int sig_num, siginfo_t *info, void *ctx)
{
	dlog(LOG_INFO, "%d, %p, %p", sig_num);
	//printf("Page fault at %p\n", info->si_addr);

	w_size_t idx = 0;
	char *addr = (char *)info->si_addr;
	allocation_t *alloc = NULL;
	w_size_t page_size = w_get_page_size();
	list_node_t *iter = allocations_list.next;
	/*determining what allocation causes the page fault*/

	w_size_t allc_cnt = 0;
	for (; iter != NULL; iter = iter->next) {
		alloc = (allocation_t *)iter->info;
		char *low_limit =  (char *)alloc->vmap.start;
		char *high_limit = low_limit + alloc->num_pages * page_size;
		++allc_cnt;
		if (low_limit <= addr && addr < high_limit)
			break;
	}
	/*the page fault is not cause by any allocation*/
	if (NULL == alloc)
		return;


	/*determining what page causes the page fault*/
	for (idx = 0; idx < alloc->num_pages; ++idx) {
		char *start_addr = (char *)alloc->vpages[idx].start;
		char *limit_addr = start_addr + page_size;
		if (start_addr <= addr && addr < limit_addr)
			break;
	}
	//printf("Page index is %d\n", idx);
	/*now index is the idex of the page that caused the page fault*/
	page_table_entry_t *pte = &(alloc->vpages[idx]);
	switch (pte->state) {
	case STATE_NOT_ALLOC:
	{
		w_size_t frame_idx = get_a_frame(alloc);
		munmap(pte->start, page_size);
		pte->start = mmap(pte->start,
			page_size,
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_FIXED,
			alloc->vmap.ram_handle,
			frame_idx * page_size);
		memset(pte->start, 0, page_size);
		w_boolean_t pr = w_protect_mapping(pte->start, 1, PROTECTION_READ);
		DIE (FALSE == pr, "Couldn't protect the page.");
		pte->prev_state = STATE_NOT_ALLOC;
		pte->state = STATE_IN_RAM;
		pte->frame = &(alloc->frames[frame_idx]);
		pte->protection = PROTECTION_READ;
		alloc->frames[frame_idx].pte = pte;
		break;
	}
	case STATE_IN_RAM:
	{
		pte->protection = PROTECTION_WRITE;
		w_boolean_t r = w_protect_mapping(pte->start, 1, pte->protection);
		DIE(r == FALSE, "Failed to protect memory.\n");
		pte->dirty = TRUE;
		break;
	}
	case STATE_IN_SWAP:
	{
		w_size_t frame_idx = get_a_frame(alloc);
		swap_in(alloc, pte, frame_idx);
		break;
	}
	default:
	break;
	}
}

w_boolean_t vmsim_init(void)
{
	dlog(LOG_INFO, "");
	return w_set_exception_handler(custom_sigsegv_handler);
}

w_boolean_t vmsim_cleanup(void)
{
	dlog(LOG_INFO, "");
	return w_set_exception_handler(empty_exception_handler);
}

w_boolean_t vm_alloc(w_size_t num_pages, w_size_t num_frames, vm_map_t *map)
{
	w_boolean_t ret = TRUE;
	w_size_t i = 0;
	w_size_t page_size = p_sz;
	int flags = MAP_SHARED | MAP_ANONYMOUS;
	w_prot_t prot = PROTECTION_NONE;
	char temp_ram[] = "ramXXXXXX";
	char temp_swp[] = "swpXXXXXX";

	if (num_pages < num_frames)
		return FALSE;

	w_size_t byte_cnt = page_size * num_pages;
	vm_map_t vmap;
	vmap.start = mmap(NULL, byte_cnt, PROT_NONE, flags, -1, 0);
	DIE(NULL == vmap.start, "Couldn't map");
	vmap.ram_handle = mkstemp(temp_ram);
	DIE(0 > vmap.ram_handle, "Couldn't make ram file");
	vmap.swap_handle = mkstemp(temp_swp);
	DIE(0 > vmap.ram_handle, "Couldn't make swap file");
	*map = vmap;

	int tr = ftruncate(vmap.ram_handle, num_frames * page_size);
	DIE(-1 == tr, "Couldn't truncate ram.");
	tr = ftruncate(vmap.swap_handle, num_pages * page_size);
	DIE(-1 == tr, "Couldn't truncate swp.");

	page_table_entry_t *vpages =
	(page_table_entry_t *)malloc(num_pages * sizeof(page_table_entry_t));
	for (i = 0; i < num_pages; ++i) {
		page_table_entry_t temp = {
			.state = STATE_NOT_ALLOC,
			.prev_state = STATE_NOT_ALLOC,
			.dirty = FALSE,
			.protection = prot,
			.start = (char *)vmap.start + i * page_size,
			.frame = NULL
		};
		vpages[i] = temp;
	}
	frame_t *frames = (frame_t *)malloc(num_frames * sizeof(frame_t));
	for (i = 0; i < num_frames; ++i) {
		frames[i].index = i;
		frames[i].pte = NULL;
	}

	allocation_t *new_alloc_node = (allocation_t *)malloc(sizeof(allocation_t));
	*new_alloc_node = (allocation_t) {
		.vmap = vmap,
		.vpages = vpages,
		.frames = frames,
		.num_pages = num_pages,
		.num_frames = num_frames,
		.swp_file_name = NULL,
		.ram_file_name = NULL
	};

	insert(new_alloc_node)into(&allocations_list);
	return ret;
}

w_boolean_t vm_free(w_ptr_t start)
{
	if (NULL == start)
		return FALSE;
	char *addr = (char *)start;
	allocation_t *alloc = NULL;
	w_size_t page_size = w_get_page_size();
	list_node_t *iter = allocations_list.next;

	for (; iter != NULL; iter = iter->next) {
		//printf("*\n");
		alloc = (allocation_t *)iter->info;
		char *low_limit =  (char *)alloc->vmap.start;
		char *high_limit = (char *)low_limit + alloc->num_pages * page_size;
		if (low_limit <= addr && addr < high_limit) {
			//printf("+\n");
			munmap(alloc->vmap.start, alloc->num_pages * page_size);
			w_close_file(alloc->vmap.ram_handle);
			w_close_file(alloc->vmap.swap_handle);
			free(alloc->vpages);
			free(alloc->frames);
			remove(alloc)from(&allocations_list);
			break;
		}
	}
	char temp_ram[] = "ramXXXXXX";
	char temp_swp[] = "swpXXXXXX";
	//w_delete_file(temp_ram);
	//w_delete_file(temp_swp);
	return TRUE;
}
