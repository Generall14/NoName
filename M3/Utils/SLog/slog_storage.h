#ifndef _SLOG_STORAGE_H_
#define _SLOG_STORAGE_H_
#include <stdint.h>

#define WORDS_PER_PAGE 1024/4

typedef struct
{
	uint32_t words[WORDS_PER_PAGE];
} slog_page;

extern uint32_t* slog_storage_base_ptr;
extern uint16_t slog_storage_tail_idx;
extern uint16_t slog_storage_head_idx;
extern uint8_t slog_storage_pages;

/**
 * storage_ptr have to be alignment to 1024B!
 */
void slog_storage_init(uint32_t* storage_ptr, uint8_t pages_1024B); // TODO: description, utests, implementation
void erase_page(uint8_t page);

#ifdef UTEST
void STUB_erase_page(uint8_t page);
#endif
#endif
