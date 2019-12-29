#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <mem.h>

extern uint8_t end;

void *sbrk(intptr_t increment) {
	static void * heap_end = &end;
	void *old_heap_end = heap_end;
	uint32_t tmp, new, pages, i;
	
	if(increment > 0) {
		tmp = ((((uint32_t) heap_end) + (PAGE_SIZE - 1)) & ~PAGE_MASK);
		new = ((((uint32_t) heap_end) + increment + (PAGE_SIZE - 1)) & ~PAGE_MASK);
		pages = (new - tmp)/PAGE_SIZE;
		
		for(i = 0; i < pages; i++) {
			//mmu_alloc_at((void *) (tmp + i*PAGE_SIZE), true, false);
			memset((void *) (tmp + i*PAGE_SIZE), 0, PAGE_SIZE);
		}			
	} else if(increment < 0) {
		//TODO: implement
	}
	
	heap_end += increment;
	return old_heap_end;
}
