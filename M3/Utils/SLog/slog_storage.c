#include "slog_storage.h"

uint32_t* slog_storage_base_ptr=0;
uint16_t slog_storage_tail_idx=0;
uint16_t slog_storage_head_idx=0;
uint8_t slog_storage_pages=0;

void slog_storage_init(uint32_t* storage_ptr, uint8_t pages_1024B)
{
	// TODO: description, utests, implementation
}
