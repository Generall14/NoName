#include "header.h"
#include "SLog/slog_storage.h"
#include <stdbool.h>

#define ALLOCATED_PAGES 2

void setUp(slog_page* pages)
{
	memset(pages, 0xFF, ALLOCATED_PAGES*sizeof(slog_page));
	slog_storage_base_ptr=0;
	slog_storage_tail_idx=66;
	slog_storage_head_idx=66;
	slog_storage_pages=66;
}

bool verify(uint32_t* e_base_ptr, uint16_t e_tail_idx, uint16_t e_head_idx, uint8_t e_pages)
{
	if(slog_storage_base_ptr != e_base_ptr)
	{
		printf("\nError, slog_storage_base_ptr: %d, should be: %d\n", \
		slog_storage_base_ptr, (uint32_t*)e_base_ptr);
		return false;
	}
	if(slog_storage_tail_idx != e_tail_idx)
	{
		printf("\nError, slog_storage_tail_idx: %d, should be: %d\n", \
		slog_storage_tail_idx, e_tail_idx);
		return false;
	}
	if(slog_storage_head_idx != e_head_idx)
	{
		printf("\nError, slog_storage_head_idx: %d, should be: %d\n", \
		slog_storage_head_idx, e_head_idx);
		return false;
	}
	if(slog_storage_pages != e_pages)
	{
		printf("\nError, slog_storage_pages: %d, should be: %d\n", \
		slog_storage_pages, e_pages);
		return false;
	}
	
	return true;
}

void test_slog_storage()
{
// 	int *p1 = malloc(10*sizeof *p1);
//     printf("default-aligned addr:   %p\n", (void*)p1);
// 	TEST_CHECK_P(((unsigned long)p1)&0x3FF == 0, "malloc - not aligned");
//     free(p1);
//  
//     int *p2 = aligned_alloc(1024, 1024*sizeof *p2);
//     printf("1024-byte aligned addr: %p\n", (void*)p2);
// 	unsigned long align = ((unsigned long)p2)&0x3FF;
// 	TEST_CHECK_P(align == 0, "aligned_alloc - not aligned");
//     free(p2);
	
	
	
	
	
	
	slog_page *pages = aligned_alloc(1024, ALLOCATED_PAGES*sizeof(slog_page));
	
	// Empty pages
	setUp(pages);
	slog_storage_init((uint32_t*)pages, ALLOCATED_PAGES);
	TEST_CHECK_P(verify((uint32_t*)pages, 0, 0, 2), "Empty pages.");
	
	free(pages);
}
