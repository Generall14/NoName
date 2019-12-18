#ifndef _SLOG_STORAGE_H_
#define _SLOG_STORAGE_H_
#include <stdint.h>

typedef struct
{
	uint32_t words[1024/4];
} slog_page;

extern uint32_t* slog_storage_base_ptr;
extern uint32_t* slog_storage_tail_ptr;
extern uint32_t* slog_storage_head_ptr;
extern uint8_t* slog_storage_pages;

void slog_storage_init(uint32_t* storage_ptr, uint8_t pages_1024B); // TODO: description, utests, implementation

#endif
