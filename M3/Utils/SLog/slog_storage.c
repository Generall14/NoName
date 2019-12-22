#include "slog_storage.h"
#include <assert.h>

uint32_t* slog_storage_base_ptr=0;
uint16_t slog_storage_tail_idx=0;
uint16_t slog_storage_head_idx=0;
uint8_t slog_storage_pages=0;

void slog_storage_init(uint32_t* storage_ptr, uint8_t pages_1024B)
{
	assert((((long)storage_ptr)&0x3FF)==0);
	slog_storage_base_ptr = storage_ptr;
	slog_storage_pages = pages_1024B;
	// TODO: description, implementation

	int first_clear = -1, first_writed = -1;
	for(uint16_t idx=0;idx<pages_1024B*WORDS_PER_PAGE;idx++)
	{

	}
}
