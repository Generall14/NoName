#include "slog_storage.h"
#include <assert.h>
#include <stdbool.h>

uint32_t* slog_storage_base_ptr=0;
uint16_t slog_storage_tail_idx=0;
uint16_t slog_storage_head_idx=0;
uint8_t slog_storage_pages=0;

void slog_storage_init(uint32_t* storage_ptr, uint8_t pages_1024B)
{
	assert((((long)storage_ptr)&0x3FF)==0);
	slog_storage_base_ptr = storage_ptr;
	slog_storage_pages = pages_1024B;
	slog_storage_tail_idx=0;
	slog_storage_head_idx=0;

	bool clears_found = false;
	uint16_t next_idx;
	for(uint16_t idx=0;idx<pages_1024B*WORDS_PER_PAGE;idx++)
	{
		if(storage_ptr[idx] == CLEAR_VAL)
			clears_found = true;
		next_idx = idx + 1;
		if (next_idx >= pages_1024B*WORDS_PER_PAGE)
			next_idx = 0;
		if( (storage_ptr[idx] == CLEAR_VAL) && (storage_ptr[next_idx] != CLEAR_VAL))
			slog_storage_tail_idx = next_idx;
		else if( (storage_ptr[idx] != CLEAR_VAL) && (storage_ptr[next_idx] == CLEAR_VAL))
			slog_storage_head_idx = next_idx;
	}

	if(!clears_found)
	{
		for(int i=0;i<pages_1024B;i++)
			STUB(erase_page(i));
	}
}

void erase_page(uint8_t page)
{

}
