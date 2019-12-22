#include "header.h"
#include "SLog/slog_storage.h"
#include <stdbool.h>

#define ALLOCATED_PAGES 2
#define PAGE_BYTES 1024
#define PAGE_IDX 1024/4
#define HALF_PAGE_BYTES PAGE_BYTES/2
#define HALF_PAGE_IDX PAGE_IDX/2
#define PAGES 10

uint8_t c_pages[PAGES];
uint8_t calls;

void STUB_erase_page(uint8_t page)
{
	c_pages[calls++] = page;
}

void setUp(slog_page* pages)
{
	memset(pages, 0xFF, ALLOCATED_PAGES*sizeof(slog_page));
	slog_storage_base_ptr=0;
	slog_storage_tail_idx=66;
	slog_storage_head_idx=66;
	slog_storage_pages=66;
	calls = 0;
	for(int i=0;i<PAGES;i++)
		c_pages[i] = 66;
}

bool verify(uint32_t* e_base_ptr, uint16_t e_tail_idx, uint16_t e_head_idx, uint8_t e_pages, \
			uint8_t e_calls, uint8_t e_c_pages[])
{
	if(slog_storage_base_ptr != e_base_ptr)
	{
		printf("\nError, slog_storage_base_ptr: %d, should be: %i\n", \
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
	if(calls != e_calls)
	{
		printf("\nError, calls: %d, should be: %d\n", \
		calls, e_calls);
		return false;
	}
	for(int i=0;i<e_calls;i++)
	{
		if(c_pages[i] != e_c_pages[i])
		{
			printf("\nError, c_pages[%d]: %d, should be: %d\n", \
			i, c_pages[i], e_c_pages[i]);
			return false;
		}
	}
	
	return true;
}

void test_slog_storage()
{
	slog_page *pages = aligned_alloc(1024, ALLOCATED_PAGES*sizeof(slog_page));
	
	// Empty pages
	setUp(pages);
	uint8_t e_c_pages[] = {0};
	slog_storage_init((uint32_t*)pages, ALLOCATED_PAGES);
	TEST_CHECK_P(verify((uint32_t*)pages, 0, 0, 2, 0, e_c_pages), \
	"Empty pages.");
	
	// Half page, empty page
	setUp(pages);
	memset((void*)pages, 0x55, HALF_PAGE_BYTES);
	slog_storage_init((uint32_t*)pages, ALLOCATED_PAGES);
	TEST_CHECK_P(verify((uint32_t*)pages, 0, HALF_PAGE_IDX, 2, 0, e_c_pages), \
	"Half page, empty page.");
	
	// Full page, empty page
	setUp(pages);
	memset((void*)pages, 0x55, PAGE_BYTES);
	slog_storage_init((uint32_t*)pages, ALLOCATED_PAGES);
	TEST_CHECK_P(verify((uint32_t*)pages, 0, PAGE_IDX, 2, 0, e_c_pages), \
	"Full page, empty page.");
	
	// Full page, half page
	setUp(pages);
	memset((void*)pages, 0x55, PAGE_BYTES+HALF_PAGE_BYTES);
	slog_storage_init((uint32_t*)pages, ALLOCATED_PAGES);
	TEST_CHECK_P(verify((uint32_t*)pages, 0, PAGE_IDX+HALF_PAGE_IDX, 2, 0, e_c_pages), \
	"Full page, half page.");
	
	// Empty page, half page
	setUp(pages);
	memset((void*)pages+PAGE_BYTES, 0x55, HALF_PAGE_BYTES);
	slog_storage_init((uint32_t*)pages, ALLOCATED_PAGES);
	TEST_CHECK_P(verify((uint32_t*)pages, PAGE_IDX, PAGE_IDX+HALF_PAGE_IDX, 2, 0, e_c_pages), \
	"Empty page, half page.");
	
	// Half page, full page
	setUp(pages);
	memset((void*)pages+PAGE_BYTES, 0x55, PAGE_BYTES);
	memset((void*)pages, 0x55, HALF_PAGE_BYTES);
	slog_storage_init((uint32_t*)pages, ALLOCATED_PAGES);
	TEST_CHECK_P(verify((uint32_t*)pages, PAGE_IDX, HALF_PAGE_IDX, 2, 0, e_c_pages), \
	"Half page, full page.");
	
	// Empty page, full page
	setUp(pages);
	memset((void*)pages+PAGE_BYTES, 0x55, PAGE_BYTES);
	slog_storage_init((uint32_t*)pages, ALLOCATED_PAGES);
	TEST_CHECK_P(verify((uint32_t*)pages, PAGE_IDX, 0, 2, 0, e_c_pages), \
	"Empty page, full page.");
	
	// Full page, full page
	setUp(pages);
	memset((void*)pages, 0x55, PAGE_BYTES*2);
	uint8_t e_c_pages2[] = {0, 1};
	slog_storage_init((uint32_t*)pages, ALLOCATED_PAGES);
	TEST_CHECK_P(verify((uint32_t*)pages, PAGE_IDX, 0, 2, 2, e_c_pages2), \
	"Full page, full page.");
	
	free(pages);
}
